#pragma once

#include <QString>
#include <QWidget>

#include "app/widgets/vehicle_state.hpp"

class Arbiter;

enum Position : uint8_t
{
    NONE = 0,
    FRONT = (1 << 0),
    BACK = (1 << 1),
    LEFT = (1 << 2),
    MIDDLE_LEFT = (1 << 3),
    RIGHT = (1 << 4),
    MIDDLE_RIGHT = (1 << 5),
    FRONT_LEFT = FRONT | LEFT,
    FRONT_RIGHT = FRONT | RIGHT,
    BACK_LEFT = BACK | LEFT,
    BACK_RIGHT = BACK | RIGHT,
    FRONT_MIDDLE_LEFT = FRONT | MIDDLE_LEFT,
    FRONT_MIDDLE_RIGHT = FRONT | MIDDLE_RIGHT,
    BACK_MIDDLE_LEFT = BACK | MIDDLE_LEFT,
    BACK_MIDDLE_RIGHT = BACK | MIDDLE_RIGHT
};

class Vehicle : public QWidget {
    Q_OBJECT

   public:
    Vehicle(Arbiter &arbiter, QWidget *parent = nullptr);

    void rotate(int16_t degree);
    void door(Position position, bool open);
    void window(Position position, bool up);
    void headlights(bool on);
    void taillights(bool on);
    void indicators(Position position, bool on);
    void hazards(bool on);
    void pressure_init(QString unit, uint8_t threshold);
    void pressure(Position position, uint8_t value);
    void disable_sensors();
    void sensor(Position position, uint8_t level);
    void wheel_steer(int16_t degree);

   private:
    VehicleState *vehicle;
};
