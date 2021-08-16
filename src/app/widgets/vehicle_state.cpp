#include <math.h>
#include <QColor>
#include <QPainter>
#include <QBitmap>
#include <QIcon>
#include <QDebug>

#include "app/arbiter.hpp"

#include "app/widgets/vehicle_state.hpp"

const QColor VehicleState::PRESSURE_WARN_COLOR = QColor("orange");

VehicleState::VehicleState(Arbiter &arbiter, QWidget *parent)
    : QFrame(parent)
    , vehicle_ref(":/graphics/vehicle/car.svg")
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);

    this->scale = arbiter.layout().scale;

    this->toggle_pressure(true);
    this->set_pressure_threshold(35);
}

QSize VehicleState::sizeHint() const
{
    return QSize(this->height(), this->height());
}

void VehicleState::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    const QFontMetrics fontMetrics(this->font());

    this->set_base_color();

    int size = std::min(this->height(), this->height());

    QPixmap graphic;
    graphic.loadFromData(this->vehicle_ref.bytes(), "SVG");
    painter.drawPixmap(0, 0, graphic.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    auto pen = painter.pen();
    pen.setColor(this->color);
    painter.setPen(pen);
}

void VehicleState::toggle_fdriver_door(bool opened)
{
    if (opened)
        this->vehicle_ref.rotate("fd_door", 30);
    else
        this->vehicle_ref.rotate("fd_door", 0);
    this->toggle_fdriver_window(opened);
    this->repaint();
}

void VehicleState::toggle_fdriver_window(bool opened)
{
    this->vehicle_ref.toggle("fd_window", !opened);
    this->repaint();
}

void VehicleState::toggle_rdriver_door(bool opened)
{
    if (opened)
        this->vehicle_ref.rotate("rd_door", 30);
    else
        this->vehicle_ref.rotate("rd_door", 0);
    this->toggle_rdriver_window(opened);
    this->repaint();
}

void VehicleState::toggle_rdriver_window(bool opened)
{
    this->vehicle_ref.toggle("rd_window", !opened);
    this->repaint();
}

void VehicleState::toggle_fpass_door(bool opened)
{
    if (opened)
        this->vehicle_ref.rotate("fp_door", 330);
    else
        this->vehicle_ref.rotate("fp_door", 0);
    this->toggle_fpass_window(opened);
    this->repaint();
}

void VehicleState::toggle_fpass_window(bool opened)
{
    this->vehicle_ref.toggle("fp_window", !opened);
    this->repaint();
}

void VehicleState::toggle_rpass_door(bool opened)
{
    if (opened)
        this->vehicle_ref.rotate("rp_door", 330);
    else
        this->vehicle_ref.rotate("rp_door", 0);
    this->toggle_rpass_window(opened);
    this->repaint();
}

void VehicleState::toggle_rpass_window(bool opened)
{
    this->vehicle_ref.toggle("rp_window", !opened);
    this->repaint();
}

void VehicleState::toggle_driver_headlight(bool on)
{
    this->driver_headlight = on;
    this->vehicle_ref.recolor("d_headlight", this->driver_headlight ? Qt::yellow : this->color);
    this->repaint();
}

void VehicleState::toggle_pass_headlight(bool on)
{
    this->pass_headlight = on;
    this->vehicle_ref.recolor("p_headlight", this->pass_headlight ? Qt::yellow : this->color);
    this->repaint();
}

void VehicleState::toggle_headlights(bool on)
{
    this->toggle_driver_headlight(on);
    this->toggle_pass_headlight(on);
    this->repaint();
}

void VehicleState::toggle_driver_taillight(bool on)
{
    this->driver_taillight = on;
    this->vehicle_ref.recolor("d_taillight", this->driver_taillight ? Qt::red : this->color);
    this->repaint();
}

void VehicleState::toggle_pass_taillight(bool on)
{
    this->pass_taillight = on;
    this->vehicle_ref.recolor("p_taillight", this->pass_taillight ? Qt::red : this->color);
    this->repaint();
}

void VehicleState::toggle_taillights(bool on)
{
    this->toggle_driver_taillight(on);
    this->toggle_pass_taillight(on);
    this->repaint();
}

void VehicleState::toggle_pressure(bool enabled)
{
    this->vehicle_ref.toggle("fd_pressure_value", enabled);
    this->vehicle_ref.toggle("fd_pressure_unit", enabled);
    this->vehicle_ref.toggle("rd_pressure_value", enabled);
    this->vehicle_ref.toggle("rd_pressure_unit", enabled);
    this->vehicle_ref.toggle("fp_pressure_value", enabled);
    this->vehicle_ref.toggle("fp_pressure_unit", enabled);
    this->vehicle_ref.toggle("rp_pressure_value", enabled);
    this->vehicle_ref.toggle("rp_pressure_unit", enabled);
}

void VehicleState::set_pressure_unit(QString unit)
{
    this->vehicle_ref.update("rp_pressure_unit", unit);
    this->vehicle_ref.update("rd_pressure_unit", unit);
    this->vehicle_ref.update("fp_pressure_unit", unit);
    this->vehicle_ref.update("fd_pressure_unit", unit);
    this->repaint();
}

void VehicleState::set_rpass_pressure(uint8_t value)
{
    this->vehicle_ref.update("rp_pressure_value", QString::number(value));
    this->rp_pressure = value < this->pressure_threshold;
    this->vehicle_ref.recolor("rp_pressure_value", this->rp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->vehicle_ref.recolor("rp_pressure_unit", this->rp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->vehicle_ref.recolor("rp_tire", this->rp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->repaint();
}

void VehicleState::set_rdriver_pressure(uint8_t value)
{
    this->vehicle_ref.update("rd_pressure_value", QString::number(value));
    this->rd_pressure = value < this->pressure_threshold;
    this->vehicle_ref.recolor("rd_pressure_value", this->rd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->vehicle_ref.recolor("rd_pressure_unit", this->rd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->vehicle_ref.recolor("rd_tire", this->rd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->repaint();
}

void VehicleState::set_fpass_pressure(uint8_t value)
{
    this->vehicle_ref.update("fp_pressure_value", QString::number(value));
    this->fp_pressure = value < this->pressure_threshold;
    this->vehicle_ref.recolor("fp_pressure_value", this->fp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->vehicle_ref.recolor("fp_pressure_unit", this->fp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->vehicle_ref.recolor("fp_tire", this->fp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->repaint();
}

void VehicleState::set_fdriver_pressure(uint8_t value)
{
    this->vehicle_ref.update("fd_pressure_value", QString::number(value));
    this->fd_pressure = value < this->pressure_threshold;
    this->vehicle_ref.recolor("fd_pressure_value", this->fd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->vehicle_ref.recolor("fd_pressure_unit", this->fd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->vehicle_ref.recolor("fd_tire", this->fd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
    this->repaint();
}

void VehicleState::set_fl_sensor(uint8_t level)
{
    this->set_sensor("fl_sensor", level);
}

void VehicleState::set_fml_sensor(uint8_t level)
{
    this->set_sensor("fml_sensor", level);
}

void VehicleState::set_fmr_sensor(uint8_t level)
{
    this->set_sensor("fmr_sensor", level);
}

void VehicleState::set_fr_sensor(uint8_t level)
{
    this->set_sensor("fr_sensor", level);
}

void VehicleState::set_rl_sensor(uint8_t level)
{
    this->set_sensor("rl_sensor", level);
}

void VehicleState::set_rml_sensor(uint8_t level)
{
    this->set_sensor("rml_sensor", level);
}

void VehicleState::set_rmr_sensor(uint8_t level)
{
    this->set_sensor("rmr_sensor", level);
}

void VehicleState::set_rr_sensor(uint8_t level)
{
    this->set_sensor("rr_sensor", level);
}

void VehicleState::set_base_color()
{
    static QColor prev_color = Qt::transparent;

    if (this->color != prev_color)
    {
        this->vehicle_ref.recolor(this->color);

        this->vehicle_ref.recolor("d_headlight", this->driver_headlight ? Qt::yellow : this->color);
        this->vehicle_ref.recolor("p_headlight", this->pass_headlight ? Qt::yellow : this->color);
        this->vehicle_ref.recolor("d_taillight", this->driver_taillight ? Qt::red : this->color);
        this->vehicle_ref.recolor("p_taillight", this->pass_taillight ? Qt::red : this->color);

        this->vehicle_ref.recolor("rp_pressure_value", this->rp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("rp_pressure_unit", this->rp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("rp_tire", this->rp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("rd_pressure_value", this->rd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("rd_pressure_unit", this->rd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("rd_tire", this->rd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("fp_pressure_value", this->fp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("fp_pressure_unit", this->fp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("fp_tire", this->fp_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("fd_pressure_value", this->fd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("fd_pressure_unit", this->fd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);
        this->vehicle_ref.recolor("fd_tire", this->fd_pressure ? VehicleState::PRESSURE_WARN_COLOR : this->color);

        prev_color = this->color;
    }
}

void VehicleState::set_sensor(QString sensor, uint8_t level)
{
    for (int i = 1; i <= 4; i++)
        this->vehicle_ref.toggle(sensor + QString::number(i), i <= level);
    this->repaint();
}
