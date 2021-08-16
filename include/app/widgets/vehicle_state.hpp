#pragma once

#include <QWidget>
#include <QDomDocument>

#include "app/config.hpp"
#include "app/graphics/dynamic_svg.hpp"

class Arbiter;

class VehicleState : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QColor color READ get_color WRITE set_color)

   public:
    VehicleState(Arbiter &arbiter, QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QColor get_color() { return this->color; };
    void set_color(QColor color) { this->color = color; }

    void toggle_fdriver_door(bool opened);
    void toggle_fdriver_window(bool opened);

    void toggle_rdriver_door(bool opened);
    void toggle_rdriver_window(bool opened);

    void toggle_fpass_door(bool opened);
    void toggle_fpass_window(bool opened);

    void toggle_rpass_door(bool opened);
    void toggle_rpass_window(bool opened);

    void toggle_driver_headlight(bool on);
    void toggle_pass_headlight(bool on);
    void toggle_headlights(bool on);

    void toggle_driver_taillight(bool on);
    void toggle_pass_taillight(bool on);
    void toggle_taillights(bool on);

    void toggle_pressure(bool enabled);
    void set_pressure_unit(QString unit);
    void set_rpass_pressure(uint8_t value);
    void set_rdriver_pressure(uint8_t value);
    void set_fpass_pressure(uint8_t value);
    void set_fdriver_pressure(uint8_t value);

    void set_fl_sensor(uint8_t level);
    void set_fml_sensor(uint8_t level);
    void set_fmr_sensor(uint8_t level);
    void set_fr_sensor(uint8_t level);

    void set_rl_sensor(uint8_t level);
    void set_rml_sensor(uint8_t level);
    void set_rmr_sensor(uint8_t level);
    void set_rr_sensor(uint8_t level);

    inline void set_pressure_threshold(uint8_t value) { this->pressure_threshold = value; }

   protected:
    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event)
    {
        // idk why this works but oh 🐳
        this->updateGeometry();
    }

   private:
    static const QColor PRESSURE_WARN_COLOR;

    QColor color = Qt::transparent;

    bool driver_headlight = false;
    bool pass_headlight = false;
    bool driver_taillight = false;
    bool pass_taillight = false;

    uint8_t pressure_threshold = 0;
    bool rp_pressure = false;
    bool rd_pressure = false;
    bool fp_pressure = false;
    bool fd_pressure = false;

    double scale;

    DynamicSVG vehicle_ref;

    void set_base_color();
    void set_sensor(QString sensor, uint8_t level);
};
