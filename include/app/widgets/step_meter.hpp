#pragma once

#include <QWidget>
#include <QFrame>

#include "app/config.hpp"

class StepMeter : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QColor base_color READ get_base_color WRITE set_base_color)
    Q_PROPERTY(QColor bar_color READ get_bar_color WRITE set_bar_color)

   public:
    StepMeter(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    void set_steps(int steps);
    void set_bars(int bars);
    QColor get_base_color() { return this->base_color; };
    QColor get_bar_color() { return this->bar_color; };
    void set_base_color(QColor base_color) { this->base_color = base_color; }
    void set_bar_color(QColor bar_color) { this->bar_color = bar_color; }

   protected:
    void paintEvent(QPaintEvent *event) override;

   private:
    void update_base_colors();
    void update_bar_colors();

    int steps = 0;
    int bars = 0;

    Config *config;

    QColor base_color = Qt::transparent;
    QColor bar_color = Qt::transparent;
};
