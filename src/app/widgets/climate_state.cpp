#include <math.h>
#include <QPainter>
#include <QBitmap>
#include <QDebug>

#include "app/widgets/climate_state.hpp"

ClimateState::ClimateState(QWidget *parent) : QFrame(parent), climate_ref(QIcon(":/icons/climate_ref.svg").pixmap(512, 512)), defrost(QIcon(":/icons/defrost.svg").pixmap(512, 512)), body(QIcon(":/icons/chevron_right.svg").pixmap(512, 512)), feet(QIcon(":/icons/expand_more.svg").pixmap(512, 512))
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->config = Config::get_instance();
}

QSize ClimateState::sizeHint() const
{
    return QSize(42, 42) * this->config->get_scale();
}

void ClimateState::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    double scale = this->config->get_scale();

    int x_offset = std::max(0, (this->width() - (int)(42 * scale)) / 2);
    int y_offset = std::max(0, (this->height() - (int)(42 * scale)) / 2);
    {
        QBitmap mask(this->climate_ref.createMaskFromColor(Qt::transparent));
        this->climate_ref.fill(this->color);
        this->climate_ref.setMask(mask);
    }
    painter.drawPixmap(x_offset, y_offset, this->climate_ref.scaled(42 * scale, 42 * scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    if (this->defrost_state) {
        QBitmap mask(this->defrost.createMaskFromColor(Qt::transparent));
        this->defrost.fill(this->color);
        this->defrost.setMask(mask);
        painter.drawPixmap(x_offset, y_offset, this->defrost.scaled(14 * scale, 14 * scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    if (this->body_state) {
        QBitmap mask(this->body.createMaskFromColor(Qt::transparent));
        this->body.fill(this->color);
        this->body.setMask(mask);
        painter.drawPixmap((14 * scale) + x_offset, y_offset, this->body.scaled(14 * scale, 14 * scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    if (this->feet_state) {
        QBitmap mask(this->feet.createMaskFromColor(Qt::transparent));
        this->feet.fill(this->color);
        this->feet.setMask(mask);
        painter.drawPixmap(x_offset, (14 * scale) + y_offset, this->feet.scaled(14 * scale, 14 * scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}
