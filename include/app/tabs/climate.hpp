#pragma once

#include <QtWidgets>

class Climate : public QWidget {
    Q_OBJECT

   public:
    Climate(QWidget *parent = nullptr) : QWidget(parent) {}

    inline void set_max_speed(int max_speed) { this->max_speed = max_speed; }
    inline void set_speed(int speed) { this->speed = speed; }
    inline void set_driver_temp(int driver_temp) { this->driver_temp = driver_temp; }
    inline void set_passenger_temp(int passenger_temp) { this->passenger_temp = passenger_temp; }

    friend QWidget *popup(Climate &climate);

   private:
    int max_speed = 0;
    int speed = 0;
    int driver_temp = 0;
    int passenger_temp = 0;
};
