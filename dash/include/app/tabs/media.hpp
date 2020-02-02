#ifndef MEDIA_HPP_
#define MEDIA_HPP_

#include <QtWidgets>

#include <app/bluetooth.hpp>
#include <app/config.hpp>
#include <app/theme.hpp>
#include <app/tuner.hpp>

class MediaTab : public QWidget {
    Q_OBJECT

   public:
    MediaTab(QWidget *parent = nullptr);

   private:
};

class BluetoothPlayerTab : public QWidget {
    Q_OBJECT

   public:
    BluetoothPlayerTab(QWidget *parent = nullptr);

   private:
    QWidget *track_widget();
    QWidget *controls_widget();

    Bluetooth *bluetooth;
    Theme *theme;
};

class RadioPlayerTab : public QWidget {
    Q_OBJECT

   public:
    RadioPlayerTab(QWidget *parent = nullptr);

   private:
    QWidget *tuner_widget();
    QWidget *controls_widget();

    Config *config;
    Theme *theme;
    Tuner *tuner;
};

#endif
