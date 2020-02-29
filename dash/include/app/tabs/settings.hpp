#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <QMap>
#include <QtWidgets>

#include <app/bluetooth.hpp>
#include <app/config.hpp>
#include <app/theme.hpp>

class SettingsTab : public QTabWidget {
    Q_OBJECT

   public:
    SettingsTab(QWidget *parent = nullptr);

   signals:
    void si_units_changed(bool);
    void brightness_updated(int);
};

class GeneralSettingsTab : public QWidget {
    Q_OBJECT

   public:
    GeneralSettingsTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *dark_mode_row_widget();
    QWidget *brightness_row_widget();
    QWidget *brightness_widget();
    QWidget *si_units_row_widget();
    QWidget *color_row_widget();
    QWidget *controls_widget();

    SettingsTab *settings_tab;
    Config *config;
    Theme *theme;
};

class BluetoothSettingsTab : public QWidget {
    Q_OBJECT

   public:
    BluetoothSettingsTab(QWidget *parent = nullptr);

   private:
    QWidget *controls_widget();
    QWidget *scanner_widget();
    QWidget *devices_widget();

    SettingsTab *settings_tab;
    Bluetooth *bluetooth;
    Config *config;
    Theme *theme;
    QMap<BluezQt::DevicePtr, QPushButton *> devices;
};

class OpenAutoSettingsTab : public QWidget {
    Q_OBJECT

   public:
    OpenAutoSettingsTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *rhd_row_widget();
    QWidget *frame_rate_row_widget();
    QWidget *resolution_row_widget();
    QWidget *dpi_row_widget();
    QWidget *dpi_widget();
    QWidget *rt_audio_row_widget();
    QWidget *audio_channels_row_widget();
    QWidget *bluetooth_row_widget();

    SettingsTab *settings_tab;
    Bluetooth *bluetooth;
    Config *config;
    Theme *theme;
};

#endif
