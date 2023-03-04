#include <QApplication>

#include "mocked.hpp"

Mocked::Mocked()
{
    for (QWidget *widget : qApp->allWidgets()) {
        if (QMainWindow* window = qobject_cast<QMainWindow *>(widget)) {
            this->window = window;
            break;
        }
    }
}

bool Mocked::supported()
{
    return true;
}

uint8_t Mocked::priority()
{
    return 1;
}

void Mocked::set(int brightness)
{
    for (auto widget : QApplication::topLevelWidgets())
        widget->setWindowOpacity(brightness / 255.0);
}
