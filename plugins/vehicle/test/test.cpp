#include <QDebug>
#include <QTimer>

#include "test.hpp"

Test::~Test()
{
    if (this->climate)
        delete this->climate;
}

QList<QWidget *> Test::widgets()
{
    QList<QWidget *> tabs;
    if (this->vehicle)
        tabs.append(this->vehicle);
    return tabs;
}

bool Test::init(ICANBus*)
{
    if (this->arbiter) {
        this->vehicle = new VehicleState(*this->arbiter);
        this->vehicle->setObjectName("Vehicle");
        this->vehicle->toggle_pressure(true);
        this->vehicle->set_pressure_unit("psi");
        this->vehicle->set_pressure_threshold(35);

        this->climate = new Climate(*this->arbiter);
        this->climate->setObjectName("Climate");
        this->climate->max_fan_speed(4);

        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]{
            static bool toggle = false;

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
                case 9:
                    this->vehicle->set_fl_sensor(rand() % 5);
                    break;
                case 10:
                    this->vehicle->set_fml_sensor(rand() % 5);
                    break;
                case 11:
                    this->vehicle->set_fmr_sensor(rand() % 5);
                    break;
                case 12:
                    this->vehicle->set_fr_sensor(rand() % 5);
                    break;
                case 13:
                    this->vehicle->set_rl_sensor(rand() % 5);
                    break;
                case 14:
                    this->vehicle->set_rml_sensor(rand() % 5);
                    break;
                case 15:
                    this->vehicle->set_rmr_sensor(rand() % 5);
                    break;
                case 16:
                    this->vehicle->set_rr_sensor(rand() % 5);
                    break;
                case 17:
                    this->vehicle->toggle_fdriver_door(toggle);
                    break;
                case 18:
                    this->vehicle->toggle_rdriver_door(toggle);
                    break;
                case 19:
                    this->vehicle->toggle_fpass_door(toggle);
                    break;
                case 20:
                    this->vehicle->toggle_rpass_door(toggle);
                    break;
                case 21:
                    this->vehicle->toggle_headlights(toggle);
                    break;
                case 22:
                    this->vehicle->toggle_taillights(toggle);
                    break;
                case 23:
                    this->vehicle->set_rpass_pressure((rand() % 21) + 30);
                    break;
                case 24:
                    this->vehicle->set_rdriver_pressure((rand() % 21) + 30);
                    break;
                case 25:
                    this->vehicle->set_fpass_pressure((rand() % 21) + 30);
                    break;
                case 26:
                    this->vehicle->set_fdriver_pressure((rand() % 21) + 30);
                    break;
                default:
                    toggle = !toggle;
                    break;
            }
        });
        timer->start(1000);

        auto timer2 = new QTimer(this);
        connect(timer2, &QTimer::timeout, [this]{
            if (rand() % 10 == 1) {
                this->climate->driver_temp((rand() % 20) + 60);
                this->climate->passenger_temp((rand() % 20) + 60);
            }
        });
        timer2->start(1000);

        return true;
    }
    
    return false;
}
