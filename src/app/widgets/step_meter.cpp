#include <QColor>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>

#include "app/arbiter.hpp"

#include "app/widgets/step_meter.hpp"

StepMeter::StepMeter(Arbiter &arbiter, QWidget *parent)
    : QFrame(parent)
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);

    this->scale = arbiter.layout().scale;
}

QSize StepMeter::sizeHint() const
{
    return QSize(this->height() * 2, this->height());
}

void StepMeter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    int spacing = 4 * this->scale;
    int bar_width = (this->width() - (spacing * (this->steps + 1))) / this->steps;

    for (int i = 0; i < this->steps; i++) {
        double bar_height = (i + 1) * (this->height() / (double)this->steps);
        QPolygonF bar(QRectF((i * (bar_width + spacing)) + spacing, this->height() - bar_height, bar_width, bar_height));

        QPainterPath path;
        path.addPolygon(bar);
        painter.fillPath(path, (this->bars > i) ? QBrush(this->bar_color) : QBrush(this->base_color));
    }
}

void StepMeter::set_steps(int steps)
{
    this->steps = steps;
    this->update();
}

void StepMeter::set_bars(int bars)
{
    this->bars = std::min(std::max(0, bars), this->steps);
    this->update();
}
