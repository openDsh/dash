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
class DataTab;

class Gauge : public QWidget {
    Q_OBJECT

   public:
    explicit Gauge(QString unit, int pos, int font_v, int font_u, int refresh_rate,
                   std::vector<Command> commands, int precision,
                   std::function<double(std::vector<double>, bool)> result, bool si, QString alt_unit = QString(),
                   QWidget *parent = 0);

    void convert(bool);
    void update_value();
    inline void start() { this->timer->start(this->refresh_rate); }
    inline void stop() { this->timer->stop(); }

   private slots:
    void update_gauge();

   private:
    DataTab *tab;

    QString unit;
    QString alt_unit;

    bool si;

    QLabel *value_label;
    QLabel *unit_label;

    QTimer *timer;
    int refresh_rate;

    std::vector<Command> commands;
    std::function<double(std::vector<double>, bool)> result;
    int precision;
};

class DataTab : public QWidget {
    Q_OBJECT

   public:
    explicit DataTab(QWidget *parent = 0);

    void enable_updates();

    void disable_updates();

    void convert_gauges(bool);

   private slots:
    void toggle_updates(bool);

   private:
    OBD *obd;

    MainWindow *app;

    QFrame *obd_status;

    std::vector<Gauge *> gauges;
};

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
