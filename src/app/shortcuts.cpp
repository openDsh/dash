#include <QDir>
#include <QElapsedTimer>
#include <QTimer>

#include <QDebug>

#include "app/shortcuts.hpp"

static const QRegExp GPIOX_REGEX("gpio\\d+");
static const QString GPIO_DIR("/sys/class/gpio");
static const QString GPIOX_DIR(GPIO_DIR + "/%1");
static const QString GPIOX_VALUE_PATH(GPIOX_DIR + "/value");
static const QString GPIOX_ACTIVE_LOW_PATH(GPIOX_DIR + "/active_low");

GpioWatcher::GpioWatcher(QObject *parent) : QObject(parent)
{
    this->watcher = new QFileSystemWatcher(this);
    for (auto gpio : QDir(GPIO_DIR).entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (GPIOX_REGEX.exactMatch(gpio) && QFile(GPIOX_ACTIVE_LOW_PATH.arg(gpio)).exists())
            this->watcher->addPath(GPIOX_VALUE_PATH.arg(gpio));
    }

    connect(this->watcher, &QFileSystemWatcher::fileChanged,
            [this](QString path) { emit gpio_triggered(QFileInfo(path).dir().dirName()); });

    this->disable();
}

ShortcutInput::ShortcutInput(QString shortcut, QWidget *parent) : QPushButton(shortcut, parent)
{
    this->gpio_watcher = new GpioWatcher(this);
    connect(this->gpio_watcher, &GpioWatcher::gpio_triggered, [this](QString gpio) {
        this->setText(gpio);
        emit shortcut_updated(gpio);
    });

    QElapsedTimer *timer = new QElapsedTimer();
    connect(this, &QPushButton::pressed, [timer]() { timer->start(); });
    connect(this, &QPushButton::released, [this, timer]() {
        if (timer->hasExpired(500)) {
            this->setText(QString());
            emit shortcut_updated(this->text());
        }
    });
}

void ShortcutInput::keyPressEvent(QKeyEvent *event)
{
    Qt::Key k = static_cast<Qt::Key>(event->key());
    if (k == Qt::Key_unknown || k == Qt::Key_Control || k == Qt::Key_Shift || k == Qt::Key_Alt || k == Qt::Key_Meta)
        return;

    QKeySequence shortcut(event->modifiers() + k);
    this->setText(shortcut.toString());
    emit shortcut_updated(this->text());
}

Shortcut::Shortcut(QString shortcut, QWidget *parent) : QObject(parent), gpio_value_attribute(parent)
{
    this->shortcut = shortcut;
    this->key = new QShortcut(parent);
    this->gpio = new QFileSystemWatcher(parent);

    connect(this->gpio, &QFileSystemWatcher::fileChanged, [this](QString) {
        if (this->gpio_value_attribute.isOpen()) {
            this->gpio_value_attribute.seek(0);
            if (this->gpio_active_low == this->gpio_value_attribute.read(1).at(0)) {
                this->gpio->blockSignals(true);
                emit activated();
                QTimer::singleShot(300, [gpio = this->gpio]() { gpio->blockSignals(false); });
            }
            else {
                qDebug() << "[Dash][Shortcut]" << this->shortcut << ": active low != value"; // temp
            }
        }
        else {
            qDebug() << "[Dash][Shortcut]" << this->shortcut << ":" << this->gpio_value_attribute.fileName() << "is not open"; // temp
        }
    });
    connect(this->key, &QShortcut::activated, [this]() { emit activated(); });
}

Shortcut::~Shortcut()
{
    if (this->gpio_value_attribute.isOpen()) this->gpio_value_attribute.close();
}

void Shortcut::set_shortcut(QString shortcut)
{
    this->key->setKey(QKeySequence());

    QStringList gpios = this->gpio->files();
    if (!gpios.isEmpty()) this->gpio->removePaths(gpios);
    if (this->gpio_value_attribute.isOpen()) this->gpio_value_attribute.close();

    this->shortcut = shortcut;
    if (this->shortcut.startsWith("gpio")) {
        qDebug() << "[Dash][Shortcut]" << this->shortcut << ": setting shortcut as gpio"; // temp
        this->gpio_value_attribute.setFileName(GPIOX_VALUE_PATH.arg(this->shortcut));
        if (this->gpio_value_attribute.open(QIODevice::ReadOnly)) {
            QFile active_low_attribute(GPIOX_ACTIVE_LOW_PATH.arg(this->shortcut));
            if (active_low_attribute.open(QIODevice::ReadOnly)) {
                this->gpio_active_low = active_low_attribute.read(1)[0];
                active_low_attribute.close();
                this->gpio->addPath(this->gpio_value_attribute.fileName());
            }
            else {
                qDebug() << "[Dash][Shortcut]" << this->shortcut << ": failed to open" << active_low_attribute; // temp
            }
        }
        else {
            qDebug() << "[Dash][Shortcut]" << this->shortcut << ": failed to open" << this->gpio_value_attribute.fileName(); // temp
        }
    }
    else if (!this->shortcut.isNull()) {
        qDebug() << "[Dash][Shortcut]" << this->shortcut << ": setting shortcut as key"; // temp
        this->key->setKey(QKeySequence::fromString(this->shortcut));
    }
}

void Shortcuts::add_shortcut(QString id, QString description, Shortcut *shortcut)
{
    this->shortcuts[id] = {description, shortcut};
    emit shortcut_added(id, description, shortcut);
}

void Shortcuts::initialize_shortcuts()
{
    for (auto shortcut : this->shortcuts)
        shortcut.second->initialize_shortcut();
}

Shortcuts *Shortcuts::get_instance()
{
    static Shortcuts shortcuts;
    return &shortcuts;
}
