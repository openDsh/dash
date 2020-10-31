#include <QColor>
#include <QHBoxLayout>
#include <QMap>
#include <QString>

#include "app/config.hpp"
#include "app/widgets/color_label.hpp"

ColorLabel::ColorLabel(QSize block_size, QFont font, QWidget *parent) : QWidget(parent), hint_palette()
{
    this->font = font;
    this->block_size = block_size;

    this->r_slider = new QSlider(Qt::Orientation::Horizontal);
    this->g_slider = new QSlider(Qt::Orientation::Horizontal);
    this->b_slider = new QSlider(Qt::Orientation::Horizontal);

    QHBoxLayout *layout = new QHBoxLayout(this);

    this->name = new QLabel(this);
    this->name->setFont(this->font);
    this->name->setText(this->color().name());

    this->icon = new QLabel(this);
    this->set_pixmap(this->icon);

    layout->addStretch();
    layout->addWidget(this->icon);
    layout->addWidget(this->name);
    layout->addStretch();

    this->dialog = new Dialog(true, this->window());
    dialog->set_body(this->dialog_body(), true);

    QPushButton *save_button = new QPushButton("save");
    connect(save_button, &QPushButton::clicked, [this]() {
        QColor color = this->color();
        this->name->setText(color.name());
        QPixmap block(this->block_size);
        block.fill(color);
        this->icon->setPixmap(block);

        emit color_changed(color);
    });
    this->dialog->set_button(save_button);
}

void ColorLabel::scale(double scale)
{
    QSize size(this->block_size);
    size.rwidth() *= scale;
    size.rheight() *= scale;
    this->icon->setPixmap(this->icon->pixmap()->scaled(size));

    this->color_hint->setFixedHeight(8 * scale);
    this->color_hint->setPalette(this->hint_palette);
}

void ColorLabel::update(QColor color)
{
    this->r_slider->setValue(color.red());
    this->g_slider->setValue(color.green());
    this->b_slider->setValue(color.blue());
    this->name->setText(color.name());

    this->set_pixmap(this->icon);
}

QWidget *ColorLabel::dialog_body()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->color_hint = new QFrame(widget);
    this->color_hint->setFixedHeight(8);
    this->color_hint->setAutoFillBackground(true);

    layout->addLayout(this->component_slider(this->r_slider));
    layout->addLayout(this->component_slider(this->g_slider));
    layout->addLayout(this->component_slider(this->b_slider));
    layout->addSpacing(16);
    layout->addWidget(color_hint);

    return widget;
}
