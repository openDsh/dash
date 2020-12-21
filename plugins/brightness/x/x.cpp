#include <QGuiApplication>
#include <QProcess>
#include <QScreen>

#include "x.hpp"

X::X()
{
    this->screen = QGuiApplication::primaryScreen();
}

void X::set(int brightness)
{
    if (this->screen != nullptr) {
        QProcess process(this);
        process.start(QString("xrandr --output %1 --brightness %2").arg(this->screen->name()).arg(brightness / 255.0));
        process.waitForFinished();
    }
}
