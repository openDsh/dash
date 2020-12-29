#include <QDebug>
#include <QTimer>

#include "test.hpp"

Test::Test()
{
    this->climate = new Climate();
    this->climate->setObjectName("Climate");
    this->climate->max_fan_speed(4);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        switch(rand() % 50) {
            case 0:
                this->climate->airflow(Airflow::OFF);
                break;
            case 1:
                this->climate->airflow(Airflow::DEFROST);
                break;
            case 2:
                this->climate->airflow(Airflow::BODY);
                break;
            case 3:
                this->climate->airflow(Airflow::FEET);
                break;
            case 4:
                this->climate->airflow(Airflow::DEFROST | Airflow::BODY);
                break;
            case 5:
                this->climate->airflow(Airflow::DEFROST | Airflow::FEET);
                break;
            case 6:
                this->climate->airflow(Airflow::BODY | Airflow::FEET);
                break;
            case 7:
                this->climate->airflow(Airflow::DEFROST | Airflow::BODY | Airflow::FEET);
                break;
            case 8:
                this->climate->fan_speed((rand() % 4) + 1);
                break;
        }
    });
    timer->start(1000);

    QTimer *timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, [this]() {
        if (rand() % 10 == 1) {
            this->climate->driver_temp((rand() % 20) + 60);
            this->climate->passenger_temp((rand() % 20) + 60);
        }
    });
    timer2->start(1000);
}

Test::~Test()
{
    delete this->climate;
}

QList<QWidget *> Test::widgets()
{
    QList<QWidget *> tabs;
    tabs.append(this->climate);
    return tabs;
}

bool Test::init(ICANBus* canbus)
{
    return true;
}
