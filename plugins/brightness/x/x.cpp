#include <QGuiApplication>
#include <QProcess>
#include <QScreen>

#include "x.hpp"

X::X()
{
    this->screen = QGuiApplication::primaryScreen();
}

bool X::is_supported()
{
    if (this->screen != nullptr) {
        // Check that we can execute xrandr
        QProcess process(this);
        process.start(QString("xrandr --version"));
        process.waitForFinished();
        return process.exitCode() == 0;
    }
    return false;
}

uint8_t X::get_priority()
{
    return 2;
}

void X::set(int brightness)
{
    if (this->screen != nullptr) {
        QProcess process(this);
        process.start(QString("xrandr --output %1 --brightness %2").arg(this->screen->name()).arg(brightness / 255.0));
        process.waitForFinished();
    }
}
