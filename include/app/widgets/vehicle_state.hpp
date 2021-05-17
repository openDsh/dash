#pragma once

#include <QWidget>
#include <QDomDocument>

#include "app/config.hpp"

class Arbiter;

class VehicleState : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QColor color READ get_color WRITE set_color)

   public:
    VehicleState(Arbiter &arbiter, QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QColor get_color() { return this->color; };
    void set_color(QColor color) { this->color = color; }

    void toggle_fdriver_door(bool open);

   protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event)
    {
        // idk why this works but oh 🐳
        this->updateGeometry();
    }

   private:
    QColor color = Qt::transparent;

    bool fdriver_door = false;

    double scale;

    QDomDocument vehicle_ref;
};
