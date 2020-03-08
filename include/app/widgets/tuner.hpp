#ifndef TUNER_HPP_
#define TUNER_HPP_

#include <QSlider>

#include <app/config.hpp>

class Tuner : public QSlider {
    Q_OBJECT
    Q_PROPERTY(QColor color READ get_color WRITE set_color)

   public:
    Tuner(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    QColor get_color() { return this->color; }
    void set_color(QColor color) { this->color = color; }

   protected:
    void paintEvent(QPaintEvent *event) override;

   signals:
    void station_updated(int);

   private:
    Config *config;
    QColor color;
};

#endif
