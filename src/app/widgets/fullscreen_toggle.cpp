#include <QLabel>

#include "app/widgets/fullscreen_toggle.hpp"
#include "app/arbiter.hpp"

FullscreenToggle::FullscreenToggle(Arbiter &arbiter)
    : Dialog(arbiter, false, nullptr)
    , p()
    , last_pos()
{
    auto label = new QLabel("X");
    this->set_body(label);

    this->setWindowOpacity(.5);
}

void FullscreenToggle::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

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
}

void FullscreenToggle::mouseReleaseEvent(QMouseEvent *event)
{
    this->setWindowOpacity(.5);
}

void FullscreenToggle::mouseDoubleClickEvent(QMouseEvent *event)
{
    Dialog::mouseDoubleClickEvent(event);
    this->accept();
}

void FullscreenToggle::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX() - this->p.x(), event->globalY() - this->p.y());
}
