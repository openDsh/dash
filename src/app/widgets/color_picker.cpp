#include <QColor>
#include <QHBoxLayout>
#include <QMap>
#include <QString>

#include "app/config.hpp"
#include "app/widgets/color_picker.hpp"

ColorPicker::ColorPicker(QSize block_size, QFont font, QWidget *parent) : QWidget(parent), hint_palette()
{
    this->font = font;

    this->r_slider = new QSlider(Qt::Orientation::Horizontal);
    this->g_slider = new QSlider(Qt::Orientation::Horizontal);
    this->b_slider = new QSlider(Qt::Orientation::Horizontal);

    QHBoxLayout *layout = new QHBoxLayout(this);

    this->button = new QPushButton(this);
    this->button->setFlat(true);
    this->button->setFont(this->font);
    this->button->setText(this->color().name());
    this->button->setIconSize(block_size);
    this->button->setProperty("base_icon_size", QVariant::fromValue(button->iconSize()));
    this->set_icon();
    layout->addWidget(this->button);

    Dialog *dialog = new Dialog(true, this->window());
    dialog->set_body(this->dialog_body(), true);

    QPushButton *save_button = new QPushButton("save");
    connect(save_button, &QPushButton::clicked, [this]() {
        QColor color = this->color();
        this->button->setText(color.name());
        this->set_icon();

        emit color_changed(color);
    });
    dialog->set_button(save_button);

    connect(this->button, &QPushButton::clicked, [dialog]() { dialog->open(); });
}

void ColorPicker::scale(double scale)
{
    this->color_hint->setFixedHeight(8 * scale);
    this->color_hint->setPalette(this->hint_palette);
}

void ColorPicker::update(QColor color)
{
    this->r_slider->setValue(color.red());
    this->g_slider->setValue(color.green());
    this->b_slider->setValue(color.blue());
    this->button->setText(color.name());

    this->set_icon();
}

QWidget *ColorPicker::dialog_body()
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
