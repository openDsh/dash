#include <QLabel>

#include "app/widgets/fullscreen_toggler.hpp"
#include "app/arbiter.hpp"

FullscreenToggler::FullscreenToggler(Arbiter &arbiter, QString name, QWidget *widget)
    : arbiter(arbiter)
    , name_(name)
    , widget_(widget)
{
}

NullFullscreenToggler::NullFullscreenToggler(Arbiter &arbiter)
    : QFrame(arbiter.window())
    , FullscreenToggler(arbiter, "none", this)
{
}

BarFullscreenToggler::BarFullscreenToggler(Arbiter &arbiter)
    : QFrame(arbiter.window())
    , FullscreenToggler(arbiter, "bar", this)
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto bar = new QPushButton();
    connect(bar, &QPushButton::clicked, [&arbiter]{ arbiter.set_fullscreen(false); });
    bar->setFixedHeight(10 * arbiter.layout().scale);
    bar->setObjectName("FullscreenBar");

    layout->addStretch(4);
    layout->addWidget(bar, 2);
    layout->addStretch(4);

    this->setVisible(false);
}

ButtonFullscreenToggler::ButtonFullscreenToggler(Arbiter &arbiter)
    : QDialog(arbiter.window(), Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowDoesNotAcceptFocus)
    , FullscreenToggler(arbiter, "button", this)
    , p()
    , last_pos()
    , touch_start()
    , label(new QLabel(" X "))
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    this->label->setMargin(2);
    this->label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    this->label->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(label);

    connect(&this->arbiter, &Arbiter::brightness_changed, [this](uint8_t brightness){
        this->setWindowOpacity(brightness / 510.0);
    });
}

void ButtonFullscreenToggler::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    this->setWindowOpacity(this->arbiter.system().brightness.value / 510.0);
    this->move(this->last_pos);
}

void ButtonFullscreenToggler::closeEvent(QCloseEvent *event)
{
    this->last_pos = this->pos();
    this->arbiter.window()->activateWindow();
}

void ButtonFullscreenToggler::mousePressEvent(QMouseEvent *event)
{
    this->setWindowOpacity(this->arbiter.system().brightness.value / 255.0);
    QFont f = this->label->font();
    f.setPointSize(f.pointSize() * 1.5);
    this->label->setFont(f);
    this->p = event->pos();
    this->touch_start = QTime::currentTime();
}

void ButtonFullscreenToggler::mouseReleaseEvent(QMouseEvent *event)
{
    this->arbiter.window()->activateWindow();
    this->setWindowOpacity(this->arbiter.system().brightness.value / 510.0);
    QFont f = this->label->font();
    f.setPointSize(f.pointSize() / 1.5);
    this->label->setFont(f);

    if (this->touch_start.msecsTo(QTime::currentTime()) < 100)
        this->arbiter.set_fullscreen(false);
}

void ButtonFullscreenToggler::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX() - this->p.x(), event->globalY() - this->p.y());
}
