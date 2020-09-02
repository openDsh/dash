#include <QFile>
#include <QGuiApplication>
#include <QProcess>
#include <QWindow>
#include <QSlider>
#include <QHBoxLayout>

#include "app/modules/brightness.hpp"
#include "app/config.hpp"
#include "app/theme.hpp"

QWidget *BrightnessModule::control_widget(bool buttons, QWidget *parent)
{
    Config *config = Config::get_instance();
    Theme *theme = Theme::get_instance();

    QWidget *widget = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(76, 255);
    slider->setValue(config->get_brightness());
    connect(slider, &QSlider::valueChanged,
            [config](int position) { config->set_brightness(position); });
    connect(config, &Config::brightness_changed, [slider](int brightness) { slider->setValue(brightness); });

    if (buttons) {
        QPushButton *dim_button = new QPushButton(widget);
        dim_button->setFlat(true);
        dim_button->setIconSize(Theme::icon_26);
        dim_button->setIcon(theme->make_button_icon("brightness_low", dim_button));
        connect(dim_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 18); });

        QPushButton *brighten_button = new QPushButton(widget);
        brighten_button->setFlat(true);
        brighten_button->setIconSize(Theme::icon_26);
        brighten_button->setIcon(theme->make_button_icon("brightness_high", brighten_button));
        connect(brighten_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 18); });

        layout->addWidget(dim_button);
        layout->addWidget(brighten_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

MockedBrightnessModule::MockedBrightnessModule(QMainWindow *window) : BrightnessModule() { this->window = window; }

void MockedBrightnessModule::set_brightness(int brightness) { this->window->setWindowOpacity(brightness / 255.0); }

RpiBrightnessModule::RpiBrightnessModule() : BrightnessModule(), brightness_attribute(this->PATH)
{
    this->brightness_attribute.open(QIODevice::WriteOnly);
}

RpiBrightnessModule::~RpiBrightnessModule()
{
    if (this->brightness_attribute.isOpen()) this->brightness_attribute.close();
}

void RpiBrightnessModule::set_brightness(int brightness)
{
    if (this->brightness_attribute.isOpen()) {
        this->brightness_attribute.reset();
        this->brightness_attribute.write(std::to_string(brightness).c_str());
        this->brightness_attribute.flush();
    }
}

XBrightnessModule::XBrightnessModule() : BrightnessModule() { this->screen = QGuiApplication::primaryScreen(); }

void XBrightnessModule::set_brightness(int brightness)
{
    QProcess process(this);
    process.start(QString("xrandr --output %1 --brightness %2").arg(this->screen->name()).arg(brightness / 255.0));
    process.waitForFinished();
}
