#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QStyle>
#include <QStyleOption>

#include "app/widgets/progress.hpp"

ProgressIndicator::ProgressIndicator(QWidget* parent) : QFrame(parent)
{
    this->pen_width = BASE_PEN_WIDTH;
    this->ellipse_point = BASE_ELLIPSE_POINT;

    this->setFocusPolicy(Qt::NoFocus);

    this->pen_width = BASE_PEN_WIDTH;

    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);
    group->setLoopCount(-1);

    this->dash_length=0;
    QPropertyAnimation *length_animation = new QPropertyAnimation(this, "dash_length");
    length_animation->setEasingCurve(QEasingCurve::InOutQuad);
    length_animation->setStartValue(0.1);
    length_animation->setKeyValueAt(0.15, 4);
    length_animation->setKeyValueAt(0.5, 18);
    length_animation->setKeyValueAt(0.6, 18);
    length_animation->setEndValue(18);
    length_animation->setDuration(1500);
    group->addAnimation(length_animation);

    QPropertyAnimation *offset_animation = new QPropertyAnimation(this, "dash_offset");
    offset_animation->setEasingCurve(QEasingCurve::InOutSine);
    offset_animation->setStartValue(0);
    offset_animation->setKeyValueAt(0.15, 0);
    offset_animation->setKeyValueAt(0.7, -7);
    offset_animation->setKeyValueAt(0.75, -7);
    offset_animation->setEndValue(-25);
    offset_animation->setDuration(1500);
    group->addAnimation(offset_animation);

    QPropertyAnimation *angle_animation = new QPropertyAnimation(this, "angle");
    angle_animation->setStartValue(-90);
    angle_animation->setEndValue(270);
    angle_animation->setDuration(1500);
    group->addAnimation(angle_animation);

    group->start();
}

QSize ProgressIndicator::sizeHint()
{
    int size = (this->ellipse_point * 2) + this->pen_width + 1;
    return QSize(size, size);
}

void ProgressIndicator::start_animation()
{
    if (this->timer_id == -1) this->timer_id = startTimer(0);
    this->enabled = true;
}

void ProgressIndicator::stop_animation()
{
    if (this->timer_id != -1) killTimer(this->timer_id);
    this->enabled = false;

    this->timer_id = -1;

    update();
}

void ProgressIndicator::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!enabled) return;

    painter.translate(this->width() / 2, this->height() / 2);
    painter.rotate(this->angle);

    QPen pen;
    pen.setWidth(this->pen_width);
    pen.setColor(palette().color(QPalette::Base));

    QVector<qreal> pattern;
    pattern << this->dash_length * 48 / 36 << 30 * 48 / 36;

    pen.setDashOffset(this->dash_offset * 48 / 36);
    pen.setDashPattern(pattern);

    painter.setPen(pen);
    painter.drawEllipse(QPoint(0, 0), this->ellipse_point, this->ellipse_point);
}
