#include "app/progress.hpp"

#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QStyle>
#include <QStyleOption>

ProgressIndicator::ProgressIndicator(QWidget* parent) : QWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);

    this->theme = Theme::get_instance();

    QParallelAnimationGroup* group = new QParallelAnimationGroup;
    group->setLoopCount(-1);

    QPropertyAnimation* animation;

    dash_length=0;

    animation = new QPropertyAnimation(this, "dash_length");
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setStartValue(0.1);
    animation->setKeyValueAt(0.15, 1);
    animation->setKeyValueAt(0.5, 18);
    animation->setKeyValueAt(0.6, 18);
    animation->setEndValue(18);
    animation->setDuration(1500);

    group->addAnimation(animation);

    animation = new QPropertyAnimation(this, "dash_offset");
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->setStartValue(0);
    animation->setKeyValueAt(0.15, 0);
    animation->setKeyValueAt(0.7, -7);
    animation->setKeyValueAt(0.75, -7);
    animation->setEndValue(-25);
    animation->setDuration(1500);

    group->addAnimation(animation);

    animation = new QPropertyAnimation(this, "angle");
    animation->setStartValue(-90);
    animation->setEndValue(270);
    animation->setDuration(1500);

    group->addAnimation(animation);

    group->start();
}

void ProgressIndicator::startAnimation()
{
    if (this->timer_id == -1) this->timer_id = startTimer(0);
    this->enabled = true;
}

void ProgressIndicator::stopAnimation()
{
    if (this->timer_id != -1) killTimer(this->timer_id);
    this->enabled = false;

    this->timer_id = -1;

    update();
}

void ProgressIndicator::timerEvent(QTimerEvent*) { update(); }

void ProgressIndicator::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!enabled) return;

    painter.translate(width() / 2, height() / 2);
    painter.rotate(this->angle);

    QPen pen;
    pen.setWidth(3);
    pen.setColor(palette().color(QPalette::Base));

    QVector<qreal> pattern;
    pattern << this->dash_length * 48 / 36 << 30 * 48 / 36;

    pen.setDashOffset(this->dash_offset * 48 / 36);
    pen.setDashPattern(pattern);

    painter.setPen(pen);

    painter.drawEllipse(QPoint(0, 0), 48 / 3, 48 / 3);
}
