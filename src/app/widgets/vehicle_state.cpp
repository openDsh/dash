#include <math.h>
#include <QColor>
#include <QPainter>
#include <QBitmap>
#include <QIcon>
#include <QDebug>

#include "app/arbiter.hpp"

#include "app/widgets/vehicle_state.hpp"

VehicleState::VehicleState(Arbiter &arbiter, QWidget *parent)
    : QFrame(parent)
    , vehicle_ref(":/graphics/vehicle/car.svg")
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);

    this->scale = arbiter.layout().scale;
}

QSize VehicleState::sizeHint() const
{
    return QSize(this->height(), this->height());
}

void VehicleState::set_base_color(QColor color)
{
    QStringList filter;

    if (this->l_headlight != Light::OFF)
        filter.append("l_headlight");
    if (this->r_headlight != Light::OFF)
        filter.append("r_headlight");
    if (this->l_taillight != Light::OFF)
        filter.append("l_taillight");
    if (this->r_taillight != Light::OFF)
        filter.append("r_taillight");
    if (this->br_pressure)
        filter.append({"br_pressure_value", "br_pressure_unit", "br_tire"});
    if (this->bl_pressure)
        filter.append({"bl_pressure_value", "bl_pressure_unit", "bl_tire"});
    if (this->fr_pressure)
        filter.append({"fr_pressure_value", "fr_pressure_unit", "fr_tire"});
    if (this->fl_pressure)
        filter.append({"fl_pressure_value", "fl_pressure_unit", "fl_tire"});

    this->base_color = color;
    this->vehicle_ref.recolor(this->base_color, filter);
}

void VehicleState::set_headlight_color(QColor color)
{
    this->headlight_color = color;

    if (this->l_headlight == Light::ON)
        this->vehicle_ref.recolor("l_headlight", this->headlight_color);
    if (this->r_headlight == Light::ON)
        this->vehicle_ref.recolor("r_headlight", this->headlight_color);
}

void VehicleState::set_taillight_color(QColor color)
{
    this->taillight_color = color;

    if (this->l_taillight == Light::ON)
        this->vehicle_ref.recolor("l_taillight", this->taillight_color);
    if (this->r_taillight == Light::ON)
        this->vehicle_ref.recolor("r_taillight", this->taillight_color);
}

void VehicleState::set_indicator_color(QColor color)
{
    this->indicator_color = color;

    if (this->l_headlight == Light::INDICATOR)
        this->vehicle_ref.recolor("l_headlight", this->indicator_color);
    if (this->r_headlight == Light::INDICATOR)
        this->vehicle_ref.recolor("r_headlight", this->indicator_color);
    if (this->l_taillight == Light::INDICATOR)
        this->vehicle_ref.recolor("l_taillight", this->indicator_color);
    if (this->r_taillight == Light::INDICATOR)
        this->vehicle_ref.recolor("r_taillight", this->indicator_color);

}

void VehicleState::set_warning_color(QColor color)
{
    this->warning_color = color;

    if (this->br_pressure) {
        this->vehicle_ref.recolor("br_pressure_value", this->warning_color);
        this->vehicle_ref.recolor("br_pressure_unit", this->warning_color);
        this->vehicle_ref.recolor("br_tire", this->warning_color);
    }
    if (this->bl_pressure) {
        this->vehicle_ref.recolor("bl_pressure_value", this->warning_color);
        this->vehicle_ref.recolor("bl_pressure_unit", this->warning_color);
        this->vehicle_ref.recolor("bl_tire", this->warning_color);
    }
    if (this->fr_pressure) {
        this->vehicle_ref.recolor("fr_pressure_value", this->warning_color);
        this->vehicle_ref.recolor("fr_pressure_unit", this->warning_color);
        this->vehicle_ref.recolor("fr_tire", this->warning_color);
    }
    if (this->fl_pressure) {
        this->vehicle_ref.recolor("fl_pressure_value", this->warning_color);
        this->vehicle_ref.recolor("fl_pressure_unit", this->warning_color);
        this->vehicle_ref.recolor("fl_tire", this->warning_color);
    }
}

void VehicleState::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    const QFontMetrics fontMetrics(this->font());

    int size = std::min(this->height(), this->height());

    QPixmap graphic;
    graphic.loadFromData(this->vehicle_ref.bytes(), "SVG");
    painter.drawPixmap(0, 0, graphic.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    auto pen = painter.pen();
    pen.setColor(this->base_color);
    painter.setPen(pen);
}

void VehicleState::rotate(int16_t degree)
{
    bool changed = false;

    changed |= this->vehicle_ref.rotate("root", degree);
    changed |= this->vehicle_ref.rotate("fl_pressure", 360 - degree);
    changed |= this->vehicle_ref.rotate("bl_pressure", 360 - degree);
    changed |= this->vehicle_ref.rotate("fr_pressure", 360 - degree);
    changed |= this->vehicle_ref.rotate("br_pressure", 360 - degree);

    if (changed)
        this->update();
}

void VehicleState::toggle_fl_door(bool open)
{
    bool changed = false;

    changed |= this->vehicle_ref.rotate("fl_door", open ? 30 : 0);
    changed |= this->vehicle_ref.toggle("fl_window", !open);

    if (changed)
        this->update();
}

void VehicleState::toggle_fl_window(bool up)
{
    if (this->vehicle_ref.toggle("fl_window", up))
        this->update();
}

void VehicleState::toggle_bl_door(bool open)
{
    bool changed = false;

    changed |= this->vehicle_ref.rotate("bl_door", open ? 30 : 0);
    changed |= this->vehicle_ref.toggle("bl_window", !open);

    if (changed)
        this->update();
}

void VehicleState::toggle_bl_window(bool up)
{
    if (this->vehicle_ref.toggle("bl_window", up))
        this->update();
}

void VehicleState::toggle_fr_door(bool open)
{
    bool changed = false;

    changed |= this->vehicle_ref.rotate("fr_door", open ? 330 : 0);
    changed |= this->vehicle_ref.toggle("fr_window", !open);

    if (changed)
        this->update();
}

void VehicleState::toggle_fr_window(bool up)
{
    if (this->vehicle_ref.toggle("fr_window", up))
        this->update();
}

void VehicleState::toggle_br_door(bool open)
{
    bool changed = false;

    changed |= this->vehicle_ref.rotate("br_door", open ? 330 : 0);
    changed |= this->vehicle_ref.toggle("br_window", !open);

    if (changed)
        this->update();
}

void VehicleState::toggle_br_window(bool up)
{
    if (this->vehicle_ref.toggle("br_window", up))
        this->update();
}

void VehicleState::toggle_headlights(bool on)
{
    bool changed = false;

    changed |= this->toggle_l_headlight(on ? Light::ON : Light::OFF, this->headlight_color);
    changed |= this->toggle_r_headlight(on ? Light::ON : Light::OFF, this->headlight_color);

    if (changed)
        this->update();
}

void VehicleState::toggle_taillights(bool on)
{
    bool changed = false;

    changed |= this->toggle_l_taillight(on ? Light::ON : Light::OFF, this->taillight_color);
    changed |= this->toggle_r_taillight(on ? Light::ON : Light::OFF, this->taillight_color);

    if (changed)
        this->update();
}

void VehicleState::toggle_l_indicators(bool on)
{
    bool changed = false;

    changed |= this->toggle_l_headlight(on ? Light::INDICATOR : Light::OFF, this->indicator_color);
    changed |= this->toggle_l_taillight(on ? Light::INDICATOR : Light::OFF, this->indicator_color);

    if (changed)
        this->update();
}

void VehicleState::toggle_r_indicators(bool on)
{
    bool changed = false;

    changed |= this->toggle_r_headlight(on ? Light::INDICATOR : Light::OFF, this->indicator_color);
    changed |= this->toggle_r_taillight(on ? Light::INDICATOR : Light::OFF, this->indicator_color);

    if (changed)
        this->update();
}

void VehicleState::toggle_hazards(bool on)
{
    bool changed = false;

    changed |= this->toggle_l_headlight(on ? Light::INDICATOR : Light::OFF, this->indicator_color);
    changed |= this->toggle_l_taillight(on ? Light::INDICATOR : Light::OFF, this->indicator_color);
    changed |= this->toggle_r_headlight(on ? Light::INDICATOR : Light::OFF, this->indicator_color);
    changed |= this->toggle_r_taillight(on ? Light::INDICATOR : Light::OFF, this->indicator_color);

    if (changed)
        this->update();
}


void VehicleState::enable_pressure()
{
    bool changed = false;

    changed |= this->vehicle_ref.toggle("fl_pressure_value", true);
    changed |= this->vehicle_ref.toggle("fl_pressure_unit", true);
    changed |= this->vehicle_ref.toggle("bl_pressure_value", true);
    changed |= this->vehicle_ref.toggle("bl_pressure_unit", true);
    changed |= this->vehicle_ref.toggle("fr_pressure_value", true);
    changed |= this->vehicle_ref.toggle("fr_pressure_unit", true);
    changed |= this->vehicle_ref.toggle("br_pressure_value", true);
    changed |= this->vehicle_ref.toggle("br_pressure_unit", true);

    if (changed)
        this->update();
}

void VehicleState::set_pressure_unit(QString unit)
{
    bool changed = false;

    changed |= this->vehicle_ref.set_text("br_pressure_unit", unit);
    changed |= this->vehicle_ref.set_text("bl_pressure_unit", unit);
    changed |= this->vehicle_ref.set_text("fr_pressure_unit", unit);
    changed |= this->vehicle_ref.set_text("fl_pressure_unit", unit);

    if (changed)
        this->update();
}

void VehicleState::set_br_pressure(uint8_t value)
{
    bool changed = false;

    this->br_pressure = value < this->pressure_threshold;
    changed |= this->vehicle_ref.recolor("br_pressure_value", this->br_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("br_pressure_unit", this->br_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("br_tire", this->br_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.set_text("br_pressure_value", QString::number(value));

    if (changed)
        this->update();
}

void VehicleState::set_bl_pressure(uint8_t value)
{
    bool changed = false;

    this->bl_pressure = value < this->pressure_threshold;
    changed |= this->vehicle_ref.recolor("bl_pressure_value", this->bl_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("bl_pressure_unit", this->bl_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("bl_tire", this->bl_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.set_text("bl_pressure_value", QString::number(value));

    if (changed)
        this->update();
}

void VehicleState::set_fr_pressure(uint8_t value)
{
    bool changed = false;

    this->fr_pressure = value < this->pressure_threshold;
    changed |= this->vehicle_ref.recolor("fr_pressure_value", this->fr_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("fr_pressure_unit", this->fr_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("fr_tire", this->fr_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.set_text("fr_pressure_value", QString::number(value));

    if (changed)
        this->update();
}

void VehicleState::set_fl_pressure(uint8_t value)
{
    bool changed = false;

    this->fl_pressure = value < this->pressure_threshold;
    changed |= this->vehicle_ref.recolor("fl_pressure_value", this->fl_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("fl_pressure_unit", this->fl_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("fl_tire", this->fl_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.set_text("fl_pressure_value", QString::number(value));

    if (changed)
        this->update();
}

void VehicleState::set_wheel_steer(int16_t degree)
{
    bool changed = false;

    changed |= this->vehicle_ref.rotate("fl_tire", degree);
    changed |= this->vehicle_ref.rotate("fr_tire", degree);

    if (changed)
        this->update();
}

void VehicleState::disable_sensors()
{
    this->sensors_enabled = false;

    bool changed = false;

    changed |= this->set_sensor("fl_sensor", 0);
    changed |= this->set_sensor("fml_sensor", 0);
    changed |= this->set_sensor("fmr_sensor", 0);
    changed |= this->set_sensor("fr_sensor", 0);
    changed |= this->set_sensor("bl_sensor", 0);
    changed |= this->set_sensor("bml_sensor", 0);
    changed |= this->set_sensor("bmr_sensor", 0);
    changed |= this->set_sensor("br_sensor", 0);
    changed |= this->vehicle_ref.scale("root", 1.25);

    if (changed)
        this->update();
}

void VehicleState::set_fl_sensor(uint8_t level)
{
    if (this->set_sensor("fl_sensor", level))
        this->update();
}

void VehicleState::set_fml_sensor(uint8_t level)
{
    if (this->set_sensor("fml_sensor", level))
        this->update();
}

void VehicleState::set_fmr_sensor(uint8_t level)
{
    if (this->set_sensor("fmr_sensor", level))
        this->update();
}

void VehicleState::set_fr_sensor(uint8_t level)
{
    if (this->set_sensor("fr_sensor", level))
        this->update();
}

void VehicleState::set_bl_sensor(uint8_t level)
{
    if (this->set_sensor("bl_sensor", level))
        this->update();
}

void VehicleState::set_bml_sensor(uint8_t level)
{
    if (this->set_sensor("bml_sensor", level))
        this->update();
}

void VehicleState::set_bmr_sensor(uint8_t level)
{
    if (this->set_sensor("bmr_sensor", level))
        this->update();
}

void VehicleState::set_br_sensor(uint8_t level)
{
    if (this->set_sensor("br_sensor", level))
        this->update();
}

bool VehicleState::toggle_l_headlight(Light state, QColor color)
{
    this->l_headlight = state;
    return this->vehicle_ref.recolor("l_headlight", (this->l_headlight != Light::OFF) ? color : this->base_color);
}

bool VehicleState::toggle_r_headlight(Light state, QColor color)
{
    this->r_headlight = state;
    return this->vehicle_ref.recolor("r_headlight", (this->r_headlight != Light::OFF) ? color : this->base_color);
}
bool VehicleState::toggle_l_taillight(Light state, QColor color)
{
    this->l_taillight = state;
    return this->vehicle_ref.recolor("l_taillight", (this->l_taillight != Light::OFF) ? color : this->base_color);
}

bool VehicleState::toggle_r_taillight(Light state, QColor color)
{
    this->r_taillight = state;
    return this->vehicle_ref.recolor("r_taillight", (this->r_taillight != Light::OFF) ? color : this->base_color);
}

bool VehicleState::set_sensor(QString sensor, uint8_t level)
{
    if (!this->sensors_enabled)
        return false;

    bool changed = false;

    for (int i = 1; i <= 4; i++)
        changed |= this->vehicle_ref.toggle(sensor + QString::number(i), i <= level);

    return changed;
}
