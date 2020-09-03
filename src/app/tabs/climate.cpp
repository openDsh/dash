#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QString>

#include "app/theme.hpp"
#include "app/tabs/climate.hpp"

ClimateSnackBar::ClimateSnackBar() : SnackBar()
{
    this->setFocusPolicy(Qt::NoFocus);
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    this->set_body(this->body_widget());
}

QWidget *ClimateSnackBar::body_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    this->driver_temp = new QLabel(widget);
    this->driver_temp->setFont(Theme::font_14);
    this->driver_temp->setAlignment(Qt::AlignCenter);
    layout->addWidget(this->driver_temp, 4);

    layout->addStretch(6);
    layout->addWidget(this->state_widget());
    layout->addStretch(6);

    this->passenger_temp = new QLabel(widget);
    this->passenger_temp->setFont(Theme::font_14);
    this->passenger_temp->setAlignment(Qt::AlignCenter);
    layout->addWidget(this->passenger_temp, 4);

    return widget;
}

QWidget *ClimateSnackBar::state_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->airflow = new ClimateState(widget);
    layout->addWidget(this->airflow);

    this->fan_speed = new StepMeter(widget);
    layout->addWidget(this->fan_speed);

    return widget;
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

void ClimateSnackBar::set_airflow(uint8_t location)
{
    this->airflow->toggle_defrost(location & 0b1);
    this->airflow->toggle_body(location & 0b010);
    this->airflow->toggle_feet(location & 0b100);
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

Climate::Climate(QWidget *parent) : QWidget(parent)
{
    this->snack_bar = new ClimateSnackBar();
}

void Climate::set_max_fan_speed(int max_speed)
{
    this->max_fan_speed = max_speed;
    this->snack_bar->set_max_fan_speed(max_speed);
}

void Climate::set_fan_speed(int speed)
{
    this->fan_speed = speed;
    this->snack_bar->set_fan_speed(speed);
}

void Climate::set_airflow(uint8_t location)
{
    this->airflow = location;
    this->snack_bar->set_airflow(location);
}

void Climate::set_driver_temp(int temp)
{
    this->driver_temp = temp;
    this->snack_bar->set_driver_temp(temp);
}

void Climate::set_passenger_temp(int temp)
{
    this->passenger_temp = temp;
    this->snack_bar->set_passenger_temp(temp);
}
