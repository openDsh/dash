#include <math.h>

#include <QColor>
#include <QBitmap>
#include <QIcon>
#include <QPainter>

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
    return QSize(this->width(), this->height());
}

void VehicleState::set_base_color(QColor color)
{
    QStringList filter({"fl_indicator", "bl_indicator", "fr_indicator", "br_indicator"});

    if (this->headlights)
        filter.append({"l_headlight", "r_headlight"});
    if (this->taillights)
        filter.append({"l_taillight", "r_taillight"});
    if (this->br_pressure)
        filter.append({"br_pressure", "br_tire"});
    if (this->bl_pressure)
        filter.append({"bl_pressure", "bl_tire"});
    if (this->fr_pressure)
        filter.append({"fr_pressure", "fr_tire"});
    if (this->fl_pressure)
        filter.append({"fl_pressure", "fl_tire"});

    this->base_color = color;
    this->vehicle_ref.recolor(this->base_color, filter);
}

void VehicleState::set_headlight_color(QColor color)
{
    this->headlight_color = color;

    if (this->headlights) {
        this->vehicle_ref.recolor("l_headlight", this->headlight_color);
        this->vehicle_ref.recolor("r_headlight", this->headlight_color);
    }
}

void VehicleState::set_taillight_color(QColor color)
{
    this->taillight_color = color;

    if (this->taillights) {
        this->vehicle_ref.recolor("l_taillight", this->taillight_color);
        this->vehicle_ref.recolor("r_taillight", this->taillight_color);
    }
}

void VehicleState::set_indicator_color(QColor color)
{
    this->indicator_color = color;

    this->vehicle_ref.recolor("fl_indicator", this->indicator_color);
    this->vehicle_ref.recolor("bl_indicator", this->indicator_color);
    this->vehicle_ref.recolor("fr_indicator", this->indicator_color);
    this->vehicle_ref.recolor("br_indicator", this->indicator_color);
}

void VehicleState::set_warning_color(QColor color)
{
    this->warning_color = color;

    if (this->br_pressure) {
        this->vehicle_ref.recolor("br_pressure", this->warning_color);
        this->vehicle_ref.recolor("br_tire", this->warning_color);
    }
    if (this->bl_pressure) {
        this->vehicle_ref.recolor("bl_pressure", this->warning_color);
        this->vehicle_ref.recolor("bl_tire", this->warning_color);
    }
    if (this->fr_pressure) {
        this->vehicle_ref.recolor("fr_pressure", this->warning_color);
        this->vehicle_ref.recolor("fr_tire", this->warning_color);
    }
    if (this->fl_pressure) {
        this->vehicle_ref.recolor("fl_pressure", this->warning_color);
        this->vehicle_ref.recolor("fl_tire", this->warning_color);
    }
}

void VehicleState::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.translate(this->rect().center());
    painter.rotate(this->rotation);

    const int degree = this->rotation % 180;
    float radian;
    if (degree > 90)
        radian = (180 - degree) * 0.01745329251;
    else
        radian = degree * 0.01745329251;
    const float rotated_width = (this->vehicle_ref.width() + (this->vehicle_ref.height() * std::tan(radian))) * std::cos(radian);
    const float rotated_height = (this->vehicle_ref.height() + (this->vehicle_ref.width() * std::tan(radian))) * std::cos(radian);
    const float scale_factor = std::min(this->width() / rotated_width, this->height() / rotated_height);

    QPixmap graphic;
    graphic.loadFromData(this->vehicle_ref.bytes(), "SVG");
    auto scaled = graphic.scaled(this->vehicle_ref.width() * scale_factor, this->vehicle_ref.height() * scale_factor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(-scaled.width() / 2.0, -scaled.height() / 2.0, scaled);

    auto pen = painter.pen();
    pen.setColor(this->base_color);
    painter.setPen(pen);
}

void VehicleState::rotate(int16_t degree)
{
    bool changed = false;

    this->rotation = degree;
    changed |= this->vehicle_ref.rotate("fl_pressure", 360 - this->rotation);
    changed |= this->vehicle_ref.rotate("bl_pressure", 360 - this->rotation);
    changed |= this->vehicle_ref.rotate("fr_pressure", 360 - this->rotation);
    changed |= this->vehicle_ref.rotate("br_pressure", 360 - this->rotation);

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

    this->headlights = on;
    changed |= this->vehicle_ref.recolor("l_headlight", this->headlights ? this->headlight_color : this->base_color);
    changed |= this->vehicle_ref.recolor("r_headlight", this->headlights ? this->headlight_color : this->base_color);

    if (changed)
        this->update();
}

void VehicleState::toggle_taillights(bool on)
{
    bool changed = false;

    this->taillights = on;
    changed |= this->vehicle_ref.recolor("l_taillight", this->taillights ? this->taillight_color : this->base_color);
    changed |= this->vehicle_ref.recolor("r_taillight", this->taillights ? this->taillight_color : this->base_color);

    if (changed)
        this->update();
}

void VehicleState::toggle_l_indicators(bool on)
{
    bool changed = false;

    changed |= this->vehicle_ref.toggle("fl_indicator", on);
    changed |= this->vehicle_ref.toggle("bl_indicator", on);

    if (changed)
        this->update();
}

void VehicleState::toggle_r_indicators(bool on)
{
    bool changed = false;

    changed |= this->vehicle_ref.toggle("fr_indicator", on);
    changed |= this->vehicle_ref.toggle("br_indicator", on);

    if (changed)
        this->update();
}

void VehicleState::toggle_hazards(bool on)
{
    bool changed = false;

    changed |= this->vehicle_ref.toggle("fl_indicator", on);
    changed |= this->vehicle_ref.toggle("bl_indicator", on);
    changed |= this->vehicle_ref.toggle("fr_indicator", on);
    changed |= this->vehicle_ref.toggle("br_indicator", on);

    if (changed)
        this->update();
}


void VehicleState::enable_pressure()
{
    bool changed = false;

    changed |= this->vehicle_ref.toggle("fl_pressure", true);
    changed |= this->vehicle_ref.toggle("bl_pressure", true);
    changed |= this->vehicle_ref.toggle("fr_pressure", true);
    changed |= this->vehicle_ref.toggle("br_pressure", true);

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
    changed |= this->vehicle_ref.recolor("br_pressure", this->br_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("br_tire", this->br_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.set_text("br_pressure_value", QString::number(value));

    if (changed)
        this->update();
}

void VehicleState::set_bl_pressure(uint8_t value)
{
    bool changed = false;

    this->bl_pressure = value < this->pressure_threshold;
    changed |= this->vehicle_ref.recolor("bl_pressure", this->bl_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("bl_tire", this->bl_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.set_text("bl_pressure_value", QString::number(value));

    if (changed)
        this->update();
}

void VehicleState::set_fr_pressure(uint8_t value)
{
    bool changed = false;

    this->fr_pressure = value < this->pressure_threshold;
    changed |= this->vehicle_ref.recolor("fr_pressure", this->fr_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.recolor("fr_tire", this->fr_pressure ? this->warning_color : this->base_color);
    changed |= this->vehicle_ref.set_text("fr_pressure_value", QString::number(value));

    if (changed)
        this->update();
}

void VehicleState::set_fl_pressure(uint8_t value)
{
    bool changed = false;

    this->fl_pressure = value < this->pressure_threshold;
    changed |= this->vehicle_ref.recolor("fl_pressure", this->fl_pressure ? this->warning_color : this->base_color);
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
    bool changed = false;

    changed |= this->set_sensor("fl_sensor", 0);
    changed |= this->set_sensor("fml_sensor", 0);
    changed |= this->set_sensor("fmr_sensor", 0);
    changed |= this->set_sensor("fr_sensor", 0);
    changed |= this->set_sensor("bl_sensor", 0);
    changed |= this->set_sensor("bml_sensor", 0);
    changed |= this->set_sensor("bmr_sensor", 0);
    changed |= this->set_sensor("br_sensor", 0);

    if (changed)
        this->update();

    this->sensors_enabled = false;
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

bool VehicleState::set_sensor(QString sensor, uint8_t level)
{
    if (!this->sensors_enabled)
        return false;

    bool changed = false;

    for (int i = 1; i <= 4; i++)
        changed |= this->vehicle_ref.toggle(sensor + QString::number(i), i <= level);

    return changed;
}
