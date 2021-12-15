#pragma once

#include <QWidget>
#include <QDomDocument>

#include "app/config.hpp"
#include "app/graphics/dynamic_svg.hpp"

class Arbiter;

class VehicleState : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QColor base_color READ get_base_color WRITE set_base_color)
    Q_PROPERTY(QColor headlight_color READ get_headlight_color WRITE set_headlight_color)
    Q_PROPERTY(QColor taillight_color READ get_taillight_color WRITE set_taillight_color)
    Q_PROPERTY(QColor indicator_color READ get_indicator_color WRITE set_indicator_color)
    Q_PROPERTY(QColor warning_color READ get_warning_color WRITE set_warning_color)

   public:
    VehicleState(Arbiter &arbiter, QWidget *parent = nullptr);

    QSize sizeHint() const override;
    void set_base_color(QColor color);
    void set_headlight_color(QColor color);
    void set_taillight_color(QColor color);
    void set_indicator_color(QColor color);
    void set_warning_color(QColor color);

    void rotate(int16_t degree);

    void toggle_fl_door(bool open);
    void toggle_fl_window(bool up);

    void toggle_bl_door(bool open);
    void toggle_bl_window(bool up);

    void toggle_fr_door(bool open);
    void toggle_fr_window(bool up);

    void toggle_br_door(bool open);
    void toggle_br_window(bool up);

    void toggle_headlights(bool on);
    void toggle_taillights(bool on);
    void toggle_l_indicators(bool on);
    void toggle_r_indicators(bool on);
    void toggle_hazards(bool on);

    void enable_pressure();
    void set_pressure_unit(QString unit);
    void set_br_pressure(uint8_t value);
    void set_bl_pressure(uint8_t value);
    void set_fr_pressure(uint8_t value);
    void set_fl_pressure(uint8_t value);

    void set_wheel_steer(int16_t degree);

    void disable_sensors();
    void set_fl_sensor(uint8_t level);
    void set_fml_sensor(uint8_t level);
    void set_fmr_sensor(uint8_t level);
    void set_fr_sensor(uint8_t level);
    void set_bl_sensor(uint8_t level);
    void set_bml_sensor(uint8_t level);
    void set_bmr_sensor(uint8_t level);
    void set_br_sensor(uint8_t level);

    inline QColor get_base_color() { return this->base_color; };
    inline QColor get_headlight_color() { return this->headlight_color; };
    inline QColor get_taillight_color() { return this->taillight_color; };
    inline QColor get_indicator_color() { return this->indicator_color; };
    inline QColor get_warning_color() { return this->warning_color; };
    inline void set_pressure_threshold(uint8_t value) { this->pressure_threshold = value; }

   protected:
    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event)
    {
        // idk why this works but oh 🐳
        this->updateGeometry();
    }

   private:
    QColor base_color;
    QColor headlight_color;
    QColor taillight_color;
    QColor indicator_color;
    QColor warning_color;

    bool headlights = false;
    bool taillights = false;

    uint8_t pressure_threshold = 0;
    bool br_pressure = false;
    bool bl_pressure = false;
    bool fr_pressure = false;
    bool fl_pressure = false;

    bool sensors_enabled = true;

    double scale;
    int16_t rotation = 0;

    DynamicSVG vehicle_ref;

    bool set_sensor(QString sensor, uint8_t level);
};
