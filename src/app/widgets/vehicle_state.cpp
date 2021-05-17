#include <math.h>
#include <QPainter>
#include <QBitmap>
#include <QIcon>
#include <QDebug>

#include "app/arbiter.hpp"

#include "app/widgets/vehicle_state.hpp"

VehicleState::VehicleState(Arbiter &arbiter, QWidget *parent)
    : QFrame(parent)
    , vehicle_ref("vehicle")
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);

    this->scale = arbiter.layout().scale;

    QFile file(":/graphics/vehicle/car_interface_v2beta.svg");
    if (file.open(QIODevice::ReadOnly))
        this->vehicle_ref.setContent(&file);
}

QSize VehicleState::sizeHint() const
{
    return QSize(this->height(), this->height());
}

void VehicleState::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPixmap graphic;
    graphic.loadFromData(this->vehicle_ref.toByteArray(), "SVG");
    // QBitmap mask(graphic.createMaskFromColor(Qt::transparent));
    // graphic.fill(this->color);
    // graphic.setMask(mask);
    painter.drawPixmap(0, 0, graphic.scaled(384, 384, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void VehicleState::mousePressEvent(QMouseEvent *event)
{
    this->toggle_fdriver_door(!this->fdriver_door);

    QWidget::mousePressEvent(event);
}

void VehicleState::toggle_fdriver_door(bool opened)
{
    this->fdriver_door = opened;
    auto elem = this->vehicle_ref.elementsByTagName("path").item(18).toElement();
    if (this->fdriver_door)
        elem.setAttribute("transform", "rotate(30 316.16 340.02)");
    else
        elem.setAttribute("transform", "rotate(0 316.16 340.02)");
    this->vehicle_ref.replaceChild(elem, this->vehicle_ref.elementsByTagName("path").item(18).toElement());
    this->repaint();
}
