#include <math.h>
#include <QFrame>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QRect>

#include "app/arbiter.hpp"
#include "app/config.hpp"
#include "app/widgets/dialog.hpp"

Dialog::Dialog(Arbiter &arbiter, bool fullscreen, QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint)
    , arbiter(arbiter)
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->fullscreen = fullscreen;
    if (fullscreen)
        this->setModal(true);

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
    this->show();
    this->raise();
    this->activateWindow();
    if (timeout > 0)
        this->timer->start(timeout);
}

void Dialog::set_title(QString str)
{
    QLabel *label = new QLabel(str, this);
    QFont font(this->arbiter.forge().font(16));
    font.setBold(true);
    label->setFont(font);
    this->title->addWidget(label);
}

void Dialog::set_body(QWidget *widget)
{
    if (this->fullscreen) {
        QScrollArea *scroll_area = new QScrollArea(this);
        Session::Forge::to_touch_scroller(scroll_area);
        scroll_area->setWidgetResizable(true);
        scroll_area->setWidget(widget);

        this->body->addWidget(scroll_area);
    }
    else {
        this->body->addWidget(widget);
    }
}

void Dialog::set_button(QPushButton *button)
{
    if (this->buttons->count() == 0)
        this->add_cancel_button();
    button->setFlat(true);
    this->buttons->addWidget(button, 0, Qt::AlignRight);
    connect(button, &QPushButton::clicked, [this]() { this->close(); });
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
            QPoint window_center = window->mapToGlobal(window->rect().center());
            QPoint parent_center = parent->mapToGlobal(parent->rect().center());

            int offset = std::ceil(4 * this->arbiter.layout().scale);

            QPoint pivot;
            if (parent_center.y() > window_center.y()) {
                pivot = (parent_center.x() > window_center.x()) ? this->rect().bottomRight() : this->rect().bottomLeft();
                pivot.ry() += (parent->height() / 2) + offset;
            }
            else {
                pivot = (parent_center.x() > window_center.x()) ? this->rect().topRight() : this->rect().topLeft();
                pivot.ry() -= (parent->height() / 2) + offset;
            }
            if (parent_center.x() > window_center.x())
                pivot.rx() -= this->width() / 2;
            else
                pivot.rx() += this->width() / 2;
            point = this->mapFromGlobal(parent_center) - pivot;
        }
        this->move(point);
    }
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_Escape || this->fullscreen)
        QDialog::keyPressEvent(event);
}

void Dialog::showEvent(QShowEvent *event)
{
    // set to null position
    this->move(QPoint());
    QWidget::showEvent(event);

    if (this->fullscreen) {
        if (QWidget *parent = this->parentWidget()) {
            int margin = std::ceil(48 * this->arbiter.layout().scale) * 2;
            this->setFixedWidth(std::min(this->width(), parent->width() - margin));
            this->setFixedHeight(std::min(this->height(), parent->height() - margin));
        }
    }

    this->set_position();
}

bool Dialog::eventFilter(QObject *object, QEvent *event)
{
    // restart timer on any event
    if (this->timer->isActive())
        this->timer->start(this->timer->interval());

    return QWidget::eventFilter(object, event);
}

SnackBar::SnackBar(Arbiter &arbiter)
    : Dialog(arbiter, false, this->get_ref())
{
    this->setFixedHeight(64 * this->arbiter.layout().scale);
}

void SnackBar::resizeEvent(QResizeEvent* event)
{
    // its possible the ref didnt exist when the parent was originally set
    if (!this->parentWidget()) {
        auto flags = this->windowFlags();
        this->setParent(this->get_ref());
        this->setWindowFlags(flags);
    }

    if (QWidget *parent = this->parentWidget())
        this->setFixedWidth(parent->width() * (2 / 3.0));

    Dialog::resizeEvent(event);
}

void SnackBar::mousePressEvent(QMouseEvent *event)
{
    this->close();
}

QWidget *SnackBar::get_ref()
{
    for (QWidget *widget : qApp->allWidgets()) {
        if (widget->objectName() == "MsgRef")
            return widget;
    }
    return nullptr;
}