#include <cmath>

#include <QFontMetrics>
#include <QLineF>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QString>

#include "app/arbiter.hpp"

#include "app/widgets/tuner.hpp"

Tuner::Tuner(Arbiter &arbiter)
    : QSlider(Qt::Orientation::Horizontal)
    , mouse_pos()
    , scale(arbiter.layout().scale)
{
    this->setRange(880, 1080);
    this->setTickInterval(1);
}

QSize Tuner::sizeHint() const
{
    return QSize(this->width(), this->width() / 3);
}

void Tuner::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const QFontMetrics fontMetrics(this->font());

    const bool half_tick = (value() % 2) == 1;

    const int ticks = 12;
    const double spacing = this->width() / (double)(ticks + 2);

    const double needle_y = this->height() * 0.1;
    const double needle_x = this->width() / 2.0;

    const double min_tick_height = this->height() * 0.3;
    const int bottom = this->height() - fontMetrics.height();
    const double slice = (bottom - min_tick_height) / (double)ticks;

    double x = 0;
    if (half_tick)
        x -= (spacing / 2.0);
    for (int i = 0; i <= (ticks * 2); i += 2) {
        int station = (this->value() - ticks) + i;
        int slices = i - ticks;
        if (half_tick) {
            station -= 1;
            slices -= 1;
        }

        x += spacing;
        const double y = (slice * std::abs(slices)) + (this->height() * 0.1);

        int alpha_offset = (255 * (y / (double)this->height())) + 24;
        auto tick_color = this->color;
        if (station % 10 == 0) {
            if (station != this->value()) {
                QString num = QString::number(station / 10);
                auto pen = painter.pen();
                pen.setColor(this->color);
                painter.setPen(pen);
                painter.drawText(QPointF(x - (fontMetrics.width(num) / 2.0), this->height()), num);
                alpha_offset = 12;
            }
        }
        tick_color.setAlpha(255 - alpha_offset);
        painter.setPen(QPen(tick_color, 4 * this->scale, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(QLineF(x, y, x, bottom));
    }

    painter.setPen(QPen(this->accent, 6 * this->scale, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QLineF(needle_x, needle_y, needle_x, bottom));

    QSlider::paintEvent(event);
}

void Tuner::mousePressEvent(QMouseEvent *event)
{
    this->mouse_pos = event->pos();
}

void Tuner::mouseMoveEvent(QMouseEvent *event)
{
    int dist = this->mouse_pos.x() - event->pos().x();
    double ratio = (this->maximum() - this->minimum()) / (double)this->width();
    int trans = dist * ratio;
    if (trans != 0) {
        this->setSliderPosition(this->value() + trans);
        this->mouse_pos = event->pos();
    }
}
