#pragma once

#include <QtWidgets>

#include "app/widgets/dialog.hpp"
#include "app/widgets/step_meter.hpp"
#include "app/widgets/climate_state.hpp"

class ClimateSnackBar : public SnackBar {
    Q_OBJECT

   public:
    ClimateSnackBar();

    void set_driver_temp(int temp);
    void set_passenger_temp(int temp);
    void set_airflow(uint8_t location);
    void set_max_fan_speed(int max_speed);
    void set_fan_speed(int speed);

   private:
    QLabel *driver_temp;
    QLabel *passenger_temp;
    ClimateState *airflow;
    StepMeter *fan_speed;

    QWidget *body_widget();
    QWidget *state_widget();
};

class Climate : public QWidget {
    Q_OBJECT

   public:
    Climate(QWidget *parent = nullptr);

    void max_fan_speed(int max_fan_speed);
    int fan_speed() const { return this->fan_speed_; }
    void fan_speed(int fan_speed);
    uint8_t airflow() const { return this->airflow_; }
    void airflow(uint8_t airflow);
    int driver_temp() const { return this->driver_temp_; }
    void driver_temp(int driver_temp);
    int passenger_temp() const { return this->passenger_temp_; }
    void passenger_temp(int passenger_temp);

   private:
    int max_fan_speed_ = 0;
    int fan_speed_ = 0;
    uint8_t airflow_ = 0;
    int driver_temp_ = 0;
    int passenger_temp_ = 0;

    ClimateSnackBar *snack_bar;
};
