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

void Mocked::set(int brightness)
{
    if (this->window != nullptr)
        this->window->setWindowOpacity(brightness / 255.0);
}
