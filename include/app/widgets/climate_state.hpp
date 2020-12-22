#pragma once

#include <QWidget>
#include <QIcon>

#include "app/config.hpp"

class ClimateState : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QColor color READ get_color WRITE set_color)

   public:
    ClimateState(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QColor get_color() { return this->color; };
    void set_color(QColor color) { this->color = color; }
    void toggle_defrost(bool enabled);
    void toggle_body(bool enabled);
    void toggle_feet(bool enabled);

   protected:
    void paintEvent(QPaintEvent *event) override;

   private:
    QColor color = Qt::transparent;

    bool defrost_state = false;
    bool body_state = false;
    bool feet_state = false;

    double scale;

    QPixmap climate_ref;
    QPixmap defrost;
    QPixmap body;
    QPixmap feet;
};
