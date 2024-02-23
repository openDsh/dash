#include <QGuiApplication>
#include <QProcess>
#include <QScreen>
#include <QString>

#include "x.hpp"

X::X()
{
    this->screen = QGuiApplication::primaryScreen();
}

bool X::supported()
{
    return true;
}

uint8_t X::priority()
{
    return 2;
}

void X::set(int brightness)
{
    if (this->screen != nullptr) {
        QProcess process(this);
        process.start(QString("/home/gioele/lum.sh %1").arg(brightness));
        process.waitForFinished();
    }
}
