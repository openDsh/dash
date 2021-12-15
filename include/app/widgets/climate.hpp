#pragma once

#include <QtWidgets>

#include "app/widgets/dialog.hpp"
#include "app/widgets/step_meter.hpp"
#include "app/widgets/climate_state.hpp"

class Arbiter;

enum Airflow : uint8_t
{
    OFF = 0,
    DEFROST = (1 << 0),
    BODY = (1 << 1),
    FEET = (1 << 2)
};

class ClimateSnackBar : public SnackBar {
    Q_OBJECT

   public:
    ClimateSnackBar(Arbiter &arbiter);

    void set_left_temp(int temp);
    void set_right_temp(int temp);
    void set_airflow(uint8_t airflow);
    void set_max_fan_speed(int max_speed);
    void set_fan_speed(int speed);

   private:
    QLabel *left_temp;
    QLabel *right_temp;
    ClimateState *airflow;
    StepMeter *fan_speed;

    QWidget *body();
    QLayout *state();
};

class Climate : public QWidget {
    Q_OBJECT

   public:
    Climate(Arbiter &arbiter, QWidget *parent = nullptr);

    void max_fan_speed(int max_fan_speed);
    int fan_speed() const { return this->fan_speed_; }
    void fan_speed(int fan_speed);
    uint8_t airflow() const { return this->airflow_; }
    void airflow(uint8_t airflow);
    int left_temp() const { return this->left_temp_; }
    void left_temp(int left_temp);
    int right_temp() const { return this->right_temp_; }
    void right_temp(int right_temp);

   private:
    int max_fan_speed_ = 0;
    int fan_speed_ = 0;
    uint8_t airflow_ = Airflow::OFF;
    int left_temp_ = 0;
    int right_temp_ = 0;

    ClimateSnackBar *snack_bar;
};
