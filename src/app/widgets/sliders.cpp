#include <QHBoxLayout>
#include <QObject>
#include <QPushButton>
#include <QSlider>

#include "app/config.hpp"
#include "app/theme.hpp"

QWidget *brightness_slider(bool buttons, QWidget *parent)
{
    Theme *theme = Theme::get_instance();
    Config *config = Config::get_instance();

    QWidget *widget = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(76, 255);
    slider->setValue(config->get_brightness());
    QObject::connect(slider, &QSlider::valueChanged,
            [config](int position) { config->set_brightness(position); });
    QObject::connect(config, &Config::brightness_changed, [slider](int brightness) { slider->setValue(brightness); });

    if (buttons) {
        QPushButton *dim_button = new QPushButton(widget);
        dim_button->setFlat(true);
        dim_button->setIconSize(Theme::icon_26);
        dim_button->setIcon(theme->make_button_icon("brightness_low", dim_button));
        QObject::connect(dim_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 18); });

        QPushButton *brighten_button = new QPushButton(widget);
        brighten_button->setFlat(true);
        brighten_button->setIconSize(Theme::icon_26);
        brighten_button->setIcon(theme->make_button_icon("brightness_high", brighten_button));
        QObject::connect(brighten_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 18); });

        layout->addWidget(dim_button);
        layout->addWidget(brighten_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}
