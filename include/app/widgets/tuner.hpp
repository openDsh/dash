#pragma once

#include <QColor>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QSlider>
#include <QSize>

class Arbiter;

class Tuner : public QSlider {
    Q_OBJECT
    Q_PROPERTY(QColor color READ get_color WRITE set_color)
    Q_PROPERTY(QColor accent READ get_accent WRITE set_accent)

   public:
    Tuner(Arbiter &arbiter);
    QSize sizeHint() const override;

    QColor get_color() { return this->color; }
    void set_color(QColor color) { this->color = color; }
    QColor get_accent() { return this->accent; }
    void set_accent(QColor accent) { this->accent = accent; }

   protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

   private:
    QColor color;
    QColor accent;
    QPoint mouse_pos;
    double scale;
};
