#include <math.h>
#include <QDebug>
#include <QPainter>

#include <app/tuner.hpp>

Tuner::Tuner(int default_position, QWidget *parent) : QSlider(Qt::Orientation::Horizontal, parent)
{
    setRange(880, 1080);
    setTickInterval(1);
    setSliderPosition(default_position);

    connect(this, &QSlider::valueChanged, [this](int value) { emit station_updated(this->sliderPosition()); });
}

QSize Tuner::sizeHint() const
{
    auto size = QSlider::sizeHint();

    return QSize(size.width(), size.height() + 96);
}

void Tuner::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    QFontMetrics fontMetrics = QFontMetrics(this->font());

    double fontHeight = fontMetrics.height();

    double count = maximum() - minimum();
    double tick_dist = (this->width() - 40) / count;
    for (int i = 0; i <= count; i++) {
        double tickX = (i * tick_dist) + 20;
        double tickY = this->height();

        int tickNum = i + minimum();
        if (tickNum % 10 == 0) {
            p.setPen(QPen(this->color, 2));
            QLineF line(tickX, 32, tickX, this->height() - 32);
            p.drawLine(line);
            tickNum /= 10;
            double fontWidth = fontMetrics.width(QString::number(tickNum));
            double font_x = tickX - (fontWidth / 2.0);
            p.drawText(QPointF(font_x, tickY), QString::number(tickNum));
        }
        else if (tickNum % 5 == 0) {
            p.setPen(QPen(this->color, 2));
            QLineF line(tickX, 40, tickX, this->height() - 40);
            p.drawLine(line);
        }
        else {
            p.setPen(QPen(this->color, 1));
            QLineF line(tickX, 48, tickX, this->height() - 48);
            p.drawLine(line);
        }
    }
    QSlider::paintEvent(event);
}
