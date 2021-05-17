#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QString>

#include "app/arbiter.hpp"

#include "app/widgets/vehicle.hpp"

Vehicle::Vehicle(Arbiter &arbiter, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("Vehicle");
}

void Vehicle::max_fan_speed(int max_fan_speed)
{
    this->max_fan_speed_ = max_fan_speed;
}
