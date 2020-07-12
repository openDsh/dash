#include <math.h>
#include <QPainter>

#include "app/widgets/tuner.hpp"

Tuner::Tuner(QWidget *parent) : QSlider(Qt::Orientation::Horizontal, parent)
{
    this->config = Config::get_instance();

    setTracking(false);
    setRange(880, 1080);
    setTickInterval(1);
    setValue(this->config->get_radio_station());

    connect(this, &QSlider::valueChanged, [this](int value) {
        this->config->set_radio_station(value);
        emit station_updated(value);
    });
}

QSize Tuner::sizeHint() const
{
    auto size = QSlider::sizeHint();

    return QSize(size.width(), size.height() + 96);
}

void Tuner::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QFontMetrics fontMetrics = QFontMetrics(this->font());

    double count = maximum() - minimum();
    double tick_dist = (this->width() - 40) / count;
    for (int i = 0; i <= count; i++) {
        double x = (i * tick_dist) + 20;
        double y = this->height();

        int tick_num = i + minimum();
        if (tick_num % 10 == 0) {
            painter.setPen(QPen(this->color, 2));
            painter.drawLine(QLineF(x, 32, x, this->height() - 32));

            tick_num /= 10;
            double font_width = fontMetrics.width(QString::number(tick_num));
            painter.drawText(QPointF(x - (font_width / 2.0), y), QString::number(tick_num));
        }
        else if (tick_num % 5 == 0) {
            painter.setPen(QPen(this->color, 2));
            painter.drawLine(QLineF(x, 40, x, this->height() - 40));
        }
        else {
            painter.setPen(QPen(this->color, 1));
            painter.drawLine(QLineF(x, 48, x, this->height() - 48));
        }
    }

    QSlider::paintEvent(event);
}
