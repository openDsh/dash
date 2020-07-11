#include <QColor>
#include <QHBoxLayout>
#include <QMap>
#include <QString>

#include "app/config.hpp"
#include "app/widgets/color_label.hpp"

ColorLabel::ColorLabel(QSize block_size, QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->block_size = block_size;

    QString color = Config::get_instance()->get_color();

    QHBoxLayout *layout = new QHBoxLayout(this);

    this->icon = new QLabel(this);
    QPixmap block(block_size);
    block.fill(this->theme->get_color(color));
    this->icon->setPixmap(block);

    this->name = new QLabel(this);
    this->name->setText(color);

    connect(this->theme, &Theme::color_updated, [this]() { this->update(this->text()); });

    layout->addStretch();
    layout->addWidget(this->icon);
    layout->addWidget(this->name);
    layout->addStretch();
}

void ColorLabel::scale(double scale)
{
    QSize size(this->block_size);
    size.rwidth() *= scale;
    size.rheight() *= scale;
    this->icon->setPixmap(this->icon->pixmap()->scaled(size));
}

void ColorLabel::update(QString color)
{
    this->name->setText(color);

    QPixmap block(this->icon->pixmap()->size());
    block.fill(this->theme->get_color(color));
    this->icon->setPixmap(block);
}
