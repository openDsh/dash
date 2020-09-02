#include <QDebug>
#include <QTimer>

#include "test.hpp"

Test::Test()
{
    this->climate = new Climate();
    this->climate->set_max_fan_speed(4);
    this->climate->setProperty("tab_title", "Climate");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        this->climate->set_fan_speed(rand() % 4);
        this->climate->set_airflow(rand());
        this->climate->set_driver_temp((rand() % 20) + 60);
        this->climate->set_passenger_temp((rand() % 20) + 60);
    });

    timer->start(5000);
}

QList<QWidget *> Test::tabs()
{
    QList<QWidget *> tabs;
    tabs.append(this->climate);
    return tabs;
}
