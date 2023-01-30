#include <QLabel>

#include "app/widgets/fullscreen_toggle.hpp"
#include "app/arbiter.hpp"

FullscreenToggle::FullscreenToggle(Arbiter &arbiter)
    : QDialog(arbiter.window(), Qt::FramelessWindowHint)
    , arbiter(arbiter)
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

    connect(&this->arbiter, &Arbiter::fullscreen_changed, [this](bool fullscreen){
        if (fullscreen)
            this->show();
        else
            this->close();
    });
}

void FullscreenToggle::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    this->setWindowOpacity(.5);
    this->move(this->last_pos);
}

void FullscreenToggle::closeEvent(QCloseEvent *event)
{
    this->last_pos = this->pos();
    this->arbiter.window()->activateWindow();
}

void FullscreenToggle::mousePressEvent(QMouseEvent *event)
{
    this->setWindowOpacity(1);
    QFont f = this->label->font();
    f.setPointSize(f.pointSize() * 1.5);
    this->label->setFont(f);
    this->p = event->pos();
    this->touch_start = QTime::currentTime();
}

void FullscreenToggle::mouseReleaseEvent(QMouseEvent *event)
{
    this->setWindowOpacity(.5);
    QFont f = this->label->font();
    f.setPointSize(f.pointSize() / 1.5);
    this->label->setFont(f);

    if (this->touch_start.msecsTo(QTime::currentTime()) < 100)
        this->arbiter.toggle_fullscreen(false);
}

void FullscreenToggle::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX() - this->p.x(), event->globalY() - this->p.y());
}
