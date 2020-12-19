#include <math.h>
#include <QPainter>
#include <QBitmap>
#include <QDebug>

#include "app/widgets/climate_state.hpp"
#include "OpenautoLog.hpp"

ClimateState::ClimateState(QWidget *parent) : QFrame(parent), climate_ref(QIcon(":/icons/climate_ref.svg").pixmap(512, 512)), defrost(QIcon(":/icons/defrost.svg").pixmap(512, 512)), body(QIcon(":/icons/chevron_right.svg").pixmap(512, 512)), feet(QIcon(":/icons/expand_more.svg").pixmap(512, 512))
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->scale = Config::get_instance()->get_scale();
}

QSize ClimateState::sizeHint() const
{
    int size = 52 * this->scale;
    return QSize(size, size);
}

void ClimateState::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int ref_size = 52 * this->scale;
    int state_size = ref_size / 3;

    int x_offset = std::max(0, (this->width() - ref_size) / 2);
    int y_offset = std::max(0, (this->height() - ref_size) / 2);
    {
        QBitmap mask(this->climate_ref.createMaskFromColor(Qt::transparent));
        this->climate_ref.fill(this->color);
        this->climate_ref.setMask(mask);
    }
    painter.drawPixmap(x_offset, y_offset, this->climate_ref.scaled(ref_size, ref_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    if (this->defrost_state) {
        QBitmap mask(this->defrost.createMaskFromColor(Qt::transparent));
        this->defrost.fill(this->color);
        this->defrost.setMask(mask);
        painter.drawPixmap(x_offset, y_offset, this->defrost.scaled(state_size, state_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    if (this->body_state) {
        QBitmap mask(this->body.createMaskFromColor(Qt::transparent));
        this->body.fill(this->color);
        this->body.setMask(mask);
        painter.drawPixmap(state_size + x_offset, y_offset, this->body.scaled(state_size, state_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    if (this->feet_state) {
        QBitmap mask(this->feet.createMaskFromColor(Qt::transparent));
        this->feet.fill(this->color);
        this->feet.setMask(mask);
        painter.drawPixmap(x_offset, state_size + y_offset, this->feet.scaled(state_size, state_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}

void ClimateState::toggle_defrost(bool enabled)
{
    this->defrost_state = enabled;
    this->repaint();
}

void ClimateState::toggle_body(bool enabled)
{
    this->body_state = enabled;
    this->repaint();
}

void ClimateState::toggle_feet(bool enabled)
{
    this->feet_state = enabled;
    this->repaint();
}
