#include <QLabel>

#include "app/widgets/fullscreen_toggle.hpp"
#include "app/arbiter.hpp"

FullscreenToggle::FullscreenToggle(Arbiter &arbiter)
    : QDialog(nullptr, Qt::FramelessWindowHint)
    , arbiter(arbiter)
    , p()
    , last_pos()
    , touch_start()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(new QLabel("X"));

    connect(&this->arbiter, &Arbiter::fullscreen_changed, [this](bool fullscreen){
        if (fullscreen)
            this->open();
        else
            this->close();
    });
}

void FullscreenToggle::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    this->setWindowOpacity(.5);
    this->move(this->last_pos);
}

void FullscreenToggle::closeEvent(QCloseEvent *event)
{
    this->last_pos = this->pos();

    QDialog::closeEvent(event);
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
