#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QString>

#include "app/arbiter.hpp"

#include "app/widgets/climate.hpp"

ClimateSnackBar::ClimateSnackBar(Arbiter &arbiter)
    : SnackBar(arbiter)
{
    this->setFocusPolicy(Qt::NoFocus);
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    this->set_body(this->body());
}

QWidget *ClimateSnackBar::body()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(6, 0, 6, 0);
    layout->setSpacing(0);

    this->driver_temp = new QLabel(widget);
    this->driver_temp->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(this->driver_temp, 1);

    layout->addLayout(this->state());

    this->passenger_temp = new QLabel(widget);
    this->passenger_temp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(this->passenger_temp, 1);

    return widget;
}

QLayout *ClimateSnackBar::state()
{
    auto layout = new QHBoxLayout();

    layout->addStretch();
    this->airflow = new ClimateState(this->arbiter);
    layout->addWidget(this->airflow, 1);

    this->fan_speed = new StepMeter(this->arbiter);
    layout->addWidget(this->fan_speed, 3);
    layout->addStretch();

    return layout;
}

void ClimateSnackBar::set_driver_temp(int temp)
{
    this->driver_temp->setText(QString("%1°").arg(temp));
    this->open(3000);
}

void ClimateSnackBar::set_passenger_temp(int temp)
{
    this->passenger_temp->setText(QString("%1°").arg(temp));
    this->open(3000);
}

void ClimateSnackBar::set_airflow(uint8_t airflow)
{
    this->airflow->toggle_defrost(airflow & Airflow::DEFROST);
    this->airflow->toggle_body(airflow & Airflow::BODY);
    this->airflow->toggle_feet(airflow & Airflow::FEET);
    this->open(3000);
}

void ClimateSnackBar::set_max_fan_speed(int max_speed)
{
    this->fan_speed->set_steps(max_speed);
}

void ClimateSnackBar::set_fan_speed(int speed)
{
    this->fan_speed->set_bars(speed);
    this->open(3000);
}

Climate::Climate(Arbiter &arbiter, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("Climate");

    this->snack_bar = new ClimateSnackBar(arbiter);
}

void Climate::max_fan_speed(int max_fan_speed)
{
    this->max_fan_speed_ = max_fan_speed;
    this->snack_bar->set_max_fan_speed(this->max_fan_speed_);
}

void Climate::fan_speed(int fan_speed)
{
    this->fan_speed_ = fan_speed;
    this->snack_bar->set_fan_speed(this->fan_speed_);
}

void Climate::airflow(uint8_t airflow)
{
    this->airflow_ = airflow;
    this->snack_bar->set_airflow(this->airflow_);
}

void Climate::driver_temp(int driver_temp)
{
    this->driver_temp_ = driver_temp;
    this->snack_bar->set_driver_temp(this->driver_temp_);
}

void Climate::passenger_temp(int passenger_temp)
{
    this->passenger_temp_ = passenger_temp;
    this->snack_bar->set_passenger_temp(this->passenger_temp_);
}
