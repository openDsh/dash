#ifndef TABS_HPP_
#define TABS_HPP_

#include <QPoint>
#include <QtWidgets>
#include <iostream>
#include <regex>
#include <thread>

#include <QtDBus/QtDBus>

#include <BluezQt/Device>

#include <vector>

#include "app/bluetooth.hpp"
#include "app/config.hpp"
#include "app/theme.hpp"
#include "app/window.hpp"
#include "obd/obd.hpp"

class QTabWidget;
class MainWindow;

class SettingsTab : public QWidget {
    Q_OBJECT

   public:
    explicit SettingsTab(QWidget *parent = 0);

   private slots:
    void bluetooth_device_added(BluezQt::DevicePtr);
    void bluetooth_device_changed(BluezQt::DevicePtr);
    void bluetooth_device_removed(BluezQt::DevicePtr);
    void media_player_changed(QString, BluezQt::MediaPlayerPtr);

   signals:
    void brightness_updated(int);
    void si_units_changed(bool);

   private:
    MainWindow *app;

    Config *config;
    Theme *theme;
    Bluetooth *bluetooth;
    QSlider *brightness_control;
    bool dark_mode = true;

    QMap<BluezQt::DevicePtr, QPushButton *> bluetooth_device_buttons;
    QVBoxLayout *bluetooth_devices;
    QLabel *media_player;
};

#endif
