#include <algorithm>
#include <functional>

#include <QDir>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QPushButton>
#include <QTimer>

#include <QDebug>

#include "app/arbiter.hpp"

#include "app/action.hpp"

const QRegExp GPIONotifier::GPIOX_REGEX("gpio\\d+");
const QString GPIONotifier::GPIO_DIR("/sys/class/gpio");
const QString GPIONotifier::GPIOX_DIR(GPIONotifier::GPIO_DIR + "/%1");
const QString GPIONotifier::GPIOX_VALUE_PATH(GPIONotifier::GPIOX_DIR + "/value");
const QString GPIONotifier::GPIOX_ACTIVE_LOW_PATH(GPIONotifier::GPIOX_DIR + "/active_low");

GPIONotifier::GPIONotifier()
    : QObject()
    , watcher()
{
    for (auto gpio : QDir(this->GPIO_DIR).entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (this->GPIOX_REGEX.exactMatch(gpio) && QFile(this->GPIOX_ACTIVE_LOW_PATH.arg(gpio)).exists())
            this->watcher.addPath(this->GPIOX_VALUE_PATH.arg(gpio));
    }

    this->disable();

    connect(&this->watcher, &QFileSystemWatcher::fileChanged, [this](QString path){
        emit triggered(QFileInfo(path).dir().dirName());
    });
}

ActionDialog::ActionDialog(Arbiter &arbiter)
    : Dialog(arbiter, true, arbiter.window())
    , notifier()
{
    this->label = new QLabel();
    this->label->setProperty("add_hint", true);
    this->label->setFont(arbiter.forge().font(14, true));
    this->label->setAlignment(Qt::AlignCenter);
    this->set_body(this->label);

    connect(&this->notifier, &GPIONotifier::triggered, [this](QString gpio){ this->label->setText(gpio); });
}

void ActionDialog::keyPressEvent(QKeyEvent *event)
{
    static const auto mod_keys = {Qt::Key_unknown, Qt::Key_Control, Qt::Key_Shift, Qt::Key_Alt, Qt::Key_Meta};

    const auto key = static_cast<Qt::Key>(event->key());
    if (std::any_of(mod_keys.begin(), mod_keys.end(), [key](Qt::Key mod_key){ return (key == mod_key); }))
        return;

    this->label->setText(QKeySequence(event->modifiers() + key).toString());
}

void ActionDialog::showEvent(QShowEvent *event)
{
    this->notifier.enable();
    this->label->setText(QString());

    Dialog::showEvent(event);

    this->label->setFocus();
}

void ActionDialog::closeEvent(QCloseEvent *event)
{
    this->notifier.disable();

    Dialog::closeEvent(event);
}

Action::GPIO::GPIO()
    : watcher()
    , value()
    , active_low(0xFF)
{
}

Action::GPIO::~GPIO()
{
    if (value.isOpen())
        value.close();
}

Action::Action(QString name, std::function<void()> callback, QWidget *parent)
    : QObject(parent)
    , shortcut(parent)
    , gpio()
    , name_(name)
    , key_()
{
    connect(&this->gpio.watcher, &QFileSystemWatcher::fileChanged, [this, callback](QString){
        if (this->gpio.value.isOpen()) {
            this->gpio.value.seek(0);
            if (this->gpio.active_low == this->gpio.value.read(1).at(0)) {
                this->gpio.watcher.blockSignals(true);
                callback();
                QTimer::singleShot(300, [this]{ this->gpio.watcher.blockSignals(false); });
            }
            else {
                qDebug() << "[Dash][Action]" << this->key_ << ": active low != value"; // temp
            }
        }
        else {
            qDebug() << "[Dash][Action]" << this->key_ << ":" << this->gpio.value.fileName() << "is not open"; // temp
        }
    });
    connect(&this->shortcut, &QShortcut::activated, [callback]{ callback(); });
}

void Action::set(QString key)
{
    this->shortcut.setKey(QKeySequence());

    auto gpios = this->gpio.watcher.files();
    if (!gpios.isEmpty())
        this->gpio.watcher.removePaths(gpios);
    if (this->gpio.value.isOpen())
        this->gpio.value.close();

    this->key_ = key;
    if (this->key_.startsWith("gpio")) {
        qDebug() << "[Dash][Action]" << this->key_ << ": setting action as gpio"; // temp
        this->gpio.value.setFileName(GPIONotifier::GPIOX_VALUE_PATH.arg(this->key_));
        if (this->gpio.value.open(QIODevice::ReadOnly)) {
            QFile active_low(GPIONotifier::GPIOX_ACTIVE_LOW_PATH.arg(this->key_), this);
            if (active_low.open(QIODevice::ReadOnly)) {
                this->gpio.active_low = active_low.read(1)[0];
                active_low.close();
                this->gpio.watcher.addPath(this->gpio.value.fileName());
            }
            else {
                qDebug() << "[Dash][Action]" << this->key_ << ": failed to open" << active_low; // temp
            }
        }
        else {
            qDebug() << "[Dash][Action]" << this->key_ << ": failed to open" << this->gpio.value.fileName(); // temp
        }
    }
    else if (!this->key_.isNull()) {
        qDebug() << "[Dash][Action]" << this->key_ << ": setting action as key"; // temp
        this->shortcut.setKey(QKeySequence::fromString(this->key_));
    }
}
