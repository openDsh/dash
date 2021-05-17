#pragma once

#include <QtWidgets>

#include "app/widgets/dialog.hpp"

class Vehicle : public QWidget {
    Q_OBJECT

   public:
    Vehicle(Arbiter &arbiter, QWidget *parent = nullptr);

    void max_fan_speed(int max_fan_speed);

   private:
    int max_fan_speed_ = 0;
};
