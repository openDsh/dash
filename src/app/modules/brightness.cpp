#include <QFile>
#include <QGuiApplication>
#include <QProcess>
#include <QWindow>

#include "app/modules/brightness.hpp"

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
