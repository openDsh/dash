#pragma once

#include <QMainWindow>
#include <QObject>
#include <QScreen>
#include <QFile>

class BrightnessModule : public QObject {
    Q_OBJECT

   public:
    BrightnessModule() {}

    virtual void set_brightness(int brightness) = 0;

    static QWidget *control_widget(bool buttons, QWidget *parent = nullptr);
};
