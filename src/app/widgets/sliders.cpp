#include <QHBoxLayout>
#include <QObject>
#include <QPushButton>
#include <QSlider>

#include "app/config.hpp"
#include "app/theme.hpp"

#include "app/widgets/sliders.hpp"

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
    QObject::connect(slider, &QSlider::valueChanged, [config](int position) { config->set_brightness(position); });
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

QWidget *volume_slider(bool skip_buttons, QWidget *parent)
{
    Theme *theme = Theme::get_instance();
    Config *config = Config::get_instance();

    QWidget *widget = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(0, 100);
    slider->setValue(config->get_volume());
    QObject::connect(slider, &QSlider::valueChanged, [config](int position) { config->set_volume(position); });
    QObject::connect(config, &Config::volume_changed, [slider](int volume) { slider->setValue(volume); });

    if (!skip_buttons) {
        QPushButton *lower_button = new QPushButton(widget);
        lower_button->setFlat(true);
        lower_button->setIconSize(Theme::icon_26);
        lower_button->setIcon(theme->make_button_icon("volume_down", lower_button));
        QObject::connect(lower_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 10); });

        QPushButton *raise_button = new QPushButton(widget);
        raise_button->setFlat(true);
        raise_button->setIconSize(Theme::icon_26);
        raise_button->setIcon(theme->make_button_icon("volume_up", raise_button));
        QObject::connect(raise_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 10); });

        layout->addWidget(lower_button);
        layout->addWidget(raise_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}
