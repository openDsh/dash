#include <QHBoxLayout>

#include "app/arbiter.hpp"

#include "app/widgets/vehicle.hpp"

Vehicle::Vehicle(Arbiter &arbiter, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("Vehicle");

    this->vehicle = new VehicleState(arbiter);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this->vehicle, 0, Qt::AlignHCenter);

}

void Vehicle::rotate(int16_t degree)
{
    this->vehicle->rotate(degree);
}

void Vehicle::door(Position position, bool open)
{
    if (position & Position::FRONT) {
        if (position & Position::LEFT)
            this->vehicle->toggle_fl_door(open);
        if (position & Position::RIGHT)
            this->vehicle->toggle_fr_door(open);
    }
    if (position & Position::BACK) {
        if (position & Position::LEFT)
            this->vehicle->toggle_bl_door(open);
        if (position & Position::RIGHT)
            this->vehicle->toggle_br_door(open);
    }
}

void Vehicle::window(Position position, bool up)
{
    if (position & Position::FRONT) {
        if (position & Position::LEFT)
            this->vehicle->toggle_fl_window(up);
        if (position & Position::RIGHT)
            this->vehicle->toggle_fr_window(up);
    }
    if (position & Position::BACK) {
        if (position & Position::LEFT)
            this->vehicle->toggle_bl_window(up);
        if (position & Position::RIGHT)
            this->vehicle->toggle_br_window(up);
    }
}

void Vehicle::headlights(bool on)
{
    this->vehicle->toggle_headlights(on);
}

void Vehicle::taillights(bool on)
{
    this->vehicle->toggle_taillights(on);
}

void Vehicle::indicators(Position position, bool on)
{
    if (position & Position::LEFT)
        this->vehicle->toggle_l_indicators(on);
    if (position & Position::RIGHT)
        this->vehicle->toggle_r_indicators(on);
}

void Vehicle::hazards(bool on)
{
    this->vehicle->toggle_hazards(on);
}

void Vehicle::pressure_init(QString unit, uint8_t threshold)
{
    this->vehicle->enable_pressure();
    this->vehicle->set_pressure_unit(unit);
    this->vehicle->set_pressure_threshold(threshold);
}

void Vehicle::pressure(Position position, uint8_t value)
{
    if (position & Position::FRONT) {
        if (position & Position::LEFT)
            this->vehicle->set_fl_pressure(value);
        if (position & Position::RIGHT)
            this->vehicle->set_fr_pressure(value);
    }
    if (position & Position::BACK) {
        if (position & Position::LEFT)
            this->vehicle->set_bl_pressure(value);
        if (position & Position::RIGHT)
            this->vehicle->set_br_pressure(value);
    }
}

void Vehicle::disable_sensors()
{
    this->vehicle->disable_sensors();
}

void Vehicle::sensor(Position position, uint8_t level)
{
    if (position & Position::FRONT) {
        if (position & Position::LEFT)
            this->vehicle->set_fl_sensor(level);
        if (position & Position::MIDDLE_LEFT)
            this->vehicle->set_fml_sensor(level);
        if (position & Position::RIGHT)
            this->vehicle->set_fr_sensor(level);
        if (position & Position::MIDDLE_RIGHT)
            this->vehicle->set_fmr_sensor(level);
    }
    if (position & Position::BACK) {
        if (position & Position::LEFT)
            this->vehicle->set_bl_sensor(level);
        if (position & Position::MIDDLE_LEFT)
            this->vehicle->set_bml_sensor(level);
        if (position & Position::RIGHT)
            this->vehicle->set_br_sensor(level);
        if (position & Position::MIDDLE_RIGHT)
            this->vehicle->set_bmr_sensor(level);
    }
}

void Vehicle::wheel_steer(int16_t degree)
{
    this->vehicle->set_wheel_steer(degree);
}
