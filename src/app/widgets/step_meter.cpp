#include <QColor>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QFrame>
#include <QPainter>

#include "app/widgets/step_meter.hpp"

StepMeter::StepMeter(QWidget *parent) : QFrame(parent)
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->scale = Config::get_instance()->get_scale();
}

QSize StepMeter::sizeHint() const
{
    int width = ((12 * this->scale) * this->steps) + ((4 * this->scale) * (this->steps + 1));
    int height = (8 * this->scale) * this->steps;
    return QSize(width, height);
}

void StepMeter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    int bar_height = 8 * this->scale;
    int bar_width = 12 * this->scale;
    int spacing = 4 * this->scale;

    int x_offset = std::max(0, (this->width() - (this->steps * (bar_width + spacing))) / 2);
    int y_offset = std::max(0, (this->height() - (this->steps * bar_height)) / 2);
    for (int i = this->steps; i > 0; i--) {
        QPolygon bar(QRect(x_offset + ((i - 1) * (bar_width + spacing)), y_offset + (this->steps - i) * bar_height, bar_width, i * bar_height));
        QPoint top_left = bar.point(0);
        top_left.ry() += bar_height - (2 * this->scale);
        bar.setPoint(0, top_left);

        QPainterPath path;
        path.addPolygon(bar);
        painter.fillPath(path, (this->bars >= i) ? QBrush(this->bar_color) : QBrush(this->base_color));
    }
}

void StepMeter::set_steps(int steps)
{
    this->steps = steps;
    this->repaint();
}

void StepMeter::set_bars(int bars)
{
    this->bars = std::min(std::max(0, bars), this->steps);
    this->repaint();
}
