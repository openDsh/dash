#include <math.h>
#include <QFrame>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QRect>

#include "app/config.hpp"
#include "app/widgets/dialog.hpp"

Dialog::Dialog(bool fullscreen, QWidget *parent) : QDialog(parent, Qt::FramelessWindowHint)
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->fullscreen = fullscreen;

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(this->content_widget());

    this->timer = new QTimer(this);
    this->timer->setSingleShot(true);
    connect(this->timer, &QTimer::timeout, [this]() { this->close(); });

    this->installEventFilter(this);
}

void Dialog::open(int timeout)
{
    if (this->fullscreen) {
        this->exec();
    }
    else {
        this->show();
        if (timeout > 0)
            this->timer->start(timeout);
    }
}

QWidget *Dialog::content_widget()
{
    QFrame *frame = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(frame);

    this->title = new QVBoxLayout();
    this->title->setContentsMargins(0, 0, 0, 0);
    this->title->setSpacing(0);
    layout->addLayout(this->title);

    this->body = new QVBoxLayout();
    this->body->setContentsMargins(0, 0, 0, 0);
    this->body->setSpacing(0);
    layout->addLayout(this->body);

    this->buttons = new QHBoxLayout();
    this->buttons->setContentsMargins(0, 0, 0, 0);
    this->buttons->setSpacing(0);
    if (this->fullscreen)
        this->add_cancel_button();
    layout->addLayout(this->buttons);

    return frame;
}

void Dialog::set_position()
{
    if (QWidget *parent = this->parentWidget()) {
        QPoint point;
        if (this->fullscreen) {
            point = parent->geometry().center() - this->rect().center();
        }
        else {
            QWidget *window = parent->window();
            QPoint center = parent->mapToGlobal(parent->rect().center());

            int offset = std::ceil(4 * Config::get_instance()->get_scale());

            QPoint pivot;
            if (center.y() > (window->height() / 2)) {
                pivot = (center.x() > (window->width() / 2)) ? this->rect().bottomRight() : this->rect().bottomLeft();
                pivot.ry() += (parent->height() / 2) + offset;
            }
            else {
                pivot = (center.x() > (window->width() / 2)) ? this->rect().topRight() : this->rect().topLeft();
                pivot.ry() -= (parent->height() / 2) + offset;
            }
            if (center.x() > (window->width() / 2))
                pivot.rx() -= this->width() / 2;
            else
                pivot.rx() += this->width() / 2;
            point = this->mapFromGlobal(center) - pivot;
        }
        this->move(point);
    }
    this->position_set = true;
}

// void Dialog::set_position()
// {
//     if (QWidget *parent = this->parentWidget()) {
//         QPoint point;
//         if (this->fullscreen) {
//             point = parent->geometry().center() - this->rect().center();
//         }
//         else {
//             QWidget *window = parent->window();
//             QPoint window_center = window->mapToGlobal(window->rect().center());
//             QPoint parent_center = parent->mapToGlobal(parent->rect().center());

//             int offset = std::ceil(4 * Config::get_instance()->get_scale());

//             QPoint pivot;
//             if (parent_center.y() > window_center.y()) {
//                 pivot = (parent_center.x() > window_center.x()) ? this->rect().bottomRight() : this->rect().bottomLeft();
//                 pivot.ry() += (parent->height() / 2) + offset;
//             }
//             else {
//                 pivot = (parent_center.x() > window_center.x()) ? this->rect().topRight() : this->rect().topLeft();
//                 pivot.ry() -= (parent->height() / 2) + offset;
//             }
//             if (parent_center.x() > window_center.x())
//                 pivot.rx() -= this->width() / 2;
//             else
//                 pivot.rx() += this->width() / 2;
//             point = this->mapFromGlobal(parent_center) - pivot;
//         }
//         this->move(point);
//     }
// }

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_Escape || this->fullscreen) QDialog::keyPressEvent(event);
}

void Dialog::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    if (!this->position_set) {
        if (this->fullscreen) {
            if (QWidget *parent = this->parentWidget()) {
                int margin = std::ceil(48 * Config::get_instance()->get_scale()) * 2;
                this->setFixedWidth(std::min(this->width(), parent->width() - margin));
                this->setFixedHeight(std::min(this->height(), parent->height() - margin));
            }
        }

        this->set_position();
    }
}

bool Dialog::eventFilter(QObject *object, QEvent *event)
{
    // restart timer on any event
    if (this->timer->isActive())
        this->timer->start(this->timer->interval());

    return QWidget::eventFilter(object, event);
}

void SnackBar::resizeEvent(QResizeEvent* event)
{
    if (QWidget *parent = this->parentWidget())
        this->setFixedWidth(parent->width() * (2 / 3.0));

    Dialog::resizeEvent(event);
}

QWidget *SnackBar::get_ref()
{
    for (QWidget *widget : qApp->allWidgets()) {
        if (widget->objectName() == "MsgRef")
            return widget;
    }
    return nullptr;
}