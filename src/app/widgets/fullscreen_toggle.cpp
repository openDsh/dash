#include <QLabel>

#include "app/widgets/fullscreen_toggle.hpp"
#include "app/arbiter.hpp"

FullscreenToggle::FullscreenToggle(Arbiter &arbiter)
    : Dialog(arbiter, false, nullptr)
    , p()
    , last_pos()
    , touch_start()
{
    this->set_body(new QLabel("X"));

    connect(&this->arbiter, &Arbiter::fullscreen_changed, [this](bool fullscreen){
        if (fullscreen)
            this->open();
        else
            this->close();
    });
}

void FullscreenToggle::showEvent(QShowEvent *event)
{
    Dialog::showEvent(event);

    this->setWindowOpacity(.5);
    this->move(this->last_pos);
}

void FullscreenToggle::closeEvent(QCloseEvent *event)
{
    this->last_pos = this->pos();

    Dialog::closeEvent(event);
}

void FullscreenToggle::mousePressEvent(QMouseEvent *event)
{
    this->setWindowOpacity(1);
    this->p = event->pos();
    this->touch_start = QTime::currentTime();
}

void FullscreenToggle::mouseReleaseEvent(QMouseEvent *event)
{
    if (this->touch_start.msecsTo(QTime::currentTime()) < 100)
        this->arbiter.toggle_fullscreen(false);
    else
        QTimer::singleShot(100, this, [this]{ this->setWindowOpacity(.5); });
}

void FullscreenToggle::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX() - this->p.x(), event->globalY() - this->p.y());
}
