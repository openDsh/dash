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

    void set_max_fan_speed(int max_speed);
    void set_fan_speed(int speed);
    void set_airflow(uint8_t location);
    void set_driver_temp(int temp);
    void set_passenger_temp(int temp);

   private:
    int max_fan_speed = 0;
    int fan_speed = 0;
    uint8_t airflow = 0;
    int driver_temp = 0;
    int passenger_temp = 0;

    ClimateSnackBar *snack_bar;
};
