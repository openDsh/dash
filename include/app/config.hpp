#pragma once

#include "openauto/Configuration/Configuration.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFrame>
#include <QKeySequence>
#include <QPluginLoader>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QWidget>
#include <QVideoFrame>

class Config : public QObject {
    Q_OBJECT

   public:
    Config();
    ~Config();

    void save();

    inline bool get_si_units() { return this->si_units; }
    inline void set_si_units(bool si_units)
    {
        this->si_units = si_units;
        emit si_units_changed(this->si_units);
    }

    inline double get_radio_station() { return this->radio_station; }
    inline void set_radio_station(double radio_station) { this->radio_station = radio_station; }

    inline bool get_radio_muted() { return this->radio_muted; }
    inline void set_radio_muted(bool radio_muted) { this->radio_muted = radio_muted; }

    inline QString get_media_home() { return this->media_home; }
    inline void set_media_home(QString media_home) { this->media_home = media_home; }

    inline QString get_cam_network_url() { return this->cam_network_url; }
    inline void set_cam_network_url(QString network_url) { this->cam_network_url = network_url; }

    inline const QString& get_cam_local_device() { return this->cam_local_device; }
    inline void set_cam_local_device(QString local_device) { this->cam_local_device = local_device; }

    inline bool get_cam_is_network() { return this->cam_is_network; }
    inline void set_cam_is_network(bool is_network) { this->cam_is_network = is_network; }

    std::shared_ptr<openauto::configuration::Configuration> openauto_config;
    openauto::configuration::Configuration::ButtonCodes openauto_button_codes;

    inline QVideoFrame::PixelFormat get_cam_local_format_override() { return this->cam_local_format_override; }
    inline void set_cam_local_format_override(QVideoFrame::PixelFormat local_format) { this->cam_local_format_override = local_format; }

    inline bool get_cam_autoconnect() { return this->cam_autoconnect; }
    inline void set_cam_autoconnect(bool enabled) { this->cam_autoconnect = enabled; }

    inline int get_cam_autoconnect_time_secs() { return this->cam_autoconnect_time_secs; }
    inline void set_cam_autoconnect_time_secs(int seconds) { this->cam_autoconnect_time_secs = seconds; }

    inline bool get_vehicle_can_bus() { return this->vehicle_can_bus; }
    inline void set_vehicle_can_bus(bool vehicle_can_bus)
    {
        this->vehicle_can_bus = vehicle_can_bus;
        emit vehicle_can_bus_changed(this->vehicle_can_bus);
    }

    inline QString get_vehicle_plugin() { return this->vehicle_plugin; }
    inline void set_vehicle_plugin(QString vehicle_plugin) { this->vehicle_plugin = vehicle_plugin; }

    inline QString get_vehicle_interface() { return this->vehicle_interface; }
    inline void set_vehicle_interface(QString vehicle_interface)
    {
        this->vehicle_interface = vehicle_interface;
        emit vehicle_interface_changed(this->vehicle_interface);
    }

    inline const QStringList &get_launcher_plugins() { return this->launcher_plugins; }
    inline void set_launcher_plugin(QString plugin, bool remove = false)
    {
        if (remove)
            this->launcher_plugins.removeOne(plugin);
        else
            this->launcher_plugins.append(plugin);
    }

    static Config *get_instance();

   private:
    QSettings settings;
    bool si_units;
    double radio_station;
    bool radio_muted;
    QString media_home;
    QString cam_network_url;
    QString cam_local_device;
    QString cam_name;
    bool cam_is_network;
    QVideoFrame::PixelFormat cam_local_format_override;
    bool cam_autoconnect;
    int cam_autoconnect_time_secs;
    QString vehicle_plugin;
    bool vehicle_can_bus;
    QString vehicle_interface;
    QStringList launcher_plugins;

   signals:
    void si_units_changed(bool si_units);
    void cam_autoconnect_changed(bool enabled);
    void vehicle_can_bus_changed(bool state);
    void vehicle_interface_changed(QString interface);
};
