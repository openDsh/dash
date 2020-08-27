#pragma once

#include <QWidget>
#include <QFrame>

class StepMeter : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QColor notch_color READ get_notch_color WRITE set_notch_color)
    Q_PROPERTY(QColor step_color READ get_step_color WRITE set_step_color)

   public:
    StepMeter(int steps, int val, QWidget *parent = nullptr);

    QColor get_notch_color() { return this->notch_color; };
    QColor get_step_color() { return this->step_color; };

    void set_notch_color(QColor notch_color)
    {
        this->notch_color = notch_color;
        this->update_notch_colors();
    }
    void set_step_color(QColor step_color)
    {
        this->step_color = step_color;
        this->update_step_colors();
    }

   private:
    void update_notch_colors();
    void update_step_colors();
    QLayout *make_block(int split);

    int steps;
    int val;

    QColor notch_color = Qt::transparent;
    QColor step_color = Qt::transparent;
    QList<QWidget *> blocks;
};
