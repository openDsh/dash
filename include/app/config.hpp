#pragma once

#include "openauto/Configuration/Configuration.hpp"

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVideoFrame>
#include <QWidget>

/*
    NOTE: to only be used for page-specific config

    this is temporary, will soon be migrated to new cool ways
*/

class Config : public QObject {
    Q_OBJECT

   public:
    std::shared_ptr<openauto::configuration::Configuration> openauto_config;
    openauto::configuration::Configuration::ButtonCodes openauto_button_codes;

    Config();

    inline double get_radio_station() { return this->radio_station; }
    inline void set_radio_station(double radio_station)
    {
        this->radio_station = radio_station;
        this->settings.setValue("Pages/Media/Radio/station", this->radio_station);
    }

    inline bool get_radio_muted() { return this->radio_muted; }
    inline void set_radio_muted(bool radio_muted)
    {
        this->radio_muted = radio_muted;
        this->settings.setValue("Pages/Media/Radio/muted", this->radio_muted);
    }

    inline QString get_radio_plugin() { return this->radio_plugin; }
    inline void set_radio_plugin(QString radio_plugin)
    {
        this->radio_plugin = radio_plugin;
        if (this->radio_plugin == "unloader")
            this->settings.remove("Pages/Media/Radio/plugin");
        else
            this->settings.setValue("Pages/Media/Radio/plugin", this->radio_plugin);
    }

    inline QString get_media_home() { return this->media_home; }
    inline void set_media_home(QString media_home)
    {
        this->media_home = media_home;
        this->settings.setValue("Pages/Media/Local/home", this->media_home);
    }

    inline bool get_si_units() { return this->si_units; }
    inline void set_si_units(bool si_units)
    {
        this->si_units = si_units;
        this->settings.setValue("Pages/Vehicle/si_units", this->si_units);
        emit si_units_changed(this->si_units);
    }

    inline bool get_vehicle_can_bus() { return this->vehicle_can_bus; }
    inline void set_vehicle_can_bus(bool vehicle_can_bus)
    {
        this->vehicle_can_bus = vehicle_can_bus;
        this->settings.setValue("Pages/Vehicle/can_bus", this->vehicle_can_bus);
        emit vehicle_can_bus_changed(this->vehicle_can_bus);
    }

    inline QString get_vehicle_interface() { return this->vehicle_interface; }
    inline void set_vehicle_interface(QString vehicle_interface)
    {
        this->vehicle_interface = vehicle_interface;
        if (this->vehicle_interface == "disabled")
            this->settings.remove("Pages/Vehicle/interface");
        else
            this->settings.setValue("Pages/Vehicle/interface", this->vehicle_interface);
        emit vehicle_interface_changed(this->vehicle_interface);
    }

    inline QString get_vehicle_plugin() { return this->vehicle_plugin; }
    inline void set_vehicle_plugin(QString vehicle_plugin)
    {
        this->vehicle_plugin = vehicle_plugin;
        if (this->vehicle_plugin == "unloader")
            this->settings.remove("Pages/Vehicle/plugin");
        else
            this->settings.setValue("Pages/Vehicle/plugin", this->vehicle_plugin);
    }

    inline QString get_cam_network_url() { return this->cam_network_url; }
    inline void set_cam_network_url(QString network_url)
    {
        this->cam_network_url = network_url;
        this->settings.setValue("Pages/Camera/stream_url", this->cam_network_url);
    }

    inline const QString& get_cam_local_device() { return this->cam_local_device; }
    inline void set_cam_local_device(QString local_device)
    {
        this->cam_local_device = local_device;
        this->settings.setValue("Pages/Camera/local_device", this->cam_local_device);
    }

    inline bool get_cam_is_network() { return this->cam_is_network; }
    inline void set_cam_is_network(bool is_network)
    {
        this->cam_is_network = is_network;
        this->settings.setValue("Pages/Camera/is_network", this->cam_is_network);
    }

    inline QVideoFrame::PixelFormat get_cam_local_format_override() { return this->cam_local_format_override; }
    inline void set_cam_local_format_override(QVideoFrame::PixelFormat local_format)
    {
        this->cam_local_format_override = local_format;
        this->settings.setValue("Pages/Camera/local_format_override", this->cam_local_format_override);
    }

    inline bool get_cam_autoconnect() { return this->cam_autoconnect; }
    inline void set_cam_autoconnect(bool enabled)
    {
        this->cam_autoconnect = enabled;
        this->settings.setValue("Pages/Camera/automatically_reconnect", this->cam_autoconnect);
    }

    inline int get_cam_autoconnect_time_secs() { return this->cam_autoconnect_time_secs; }
    inline void set_cam_autoconnect_time_secs(int seconds)
    {
        this->cam_autoconnect_time_secs = seconds;
        this->settings.setValue("Pages/Camera/auto_reconnect_time_secs", this->cam_autoconnect_time_secs);
    }

    inline bool get_cam_overlay() { return this->cam_overlay; }
    inline void set_cam_overlay(bool enabled)
    {
        this->cam_overlay = enabled;
        this->settings.setValue("Pages/Camera/Overlay/enabled", this->cam_overlay);
    }

    inline int get_cam_overlay_width() { return this->cam_overlay_width; }
    inline void set_cam_overlay_width(int value)
    {
        this->cam_overlay_width = value;
        this->settings.setValue("Pages/Camera/Overlay/width", this->cam_overlay_width);
    }

    inline int get_cam_overlay_height() { return this->cam_overlay_height; }
    inline void set_cam_overlay_height(int value)
    {
        this->cam_overlay_height = value;
        this->settings.setValue("Pages/Camera/Overlay/height", this->cam_overlay_height);
    }

    inline bool get_show_aa_connected() { return this->show_aa_connected; }
    inline void set_show_aa_connected(bool enabled)
    {
        this->show_aa_connected = enabled;
        this->settings.setValue("Pages/OpenAuto/show_aa_connected", this->show_aa_connected);
    }

    inline bool get_force_aa_fullscreen() { return this->force_aa_fullscreen; }
    inline void set_force_aa_fullscreen(bool enabled)
    {
        this->force_aa_fullscreen = enabled;
        this->settings.setValue("Pages/OpenAuto/force_aa_fullscreen", this->force_aa_fullscreen);
    }    

    inline const QStringList &get_launcher_plugins() { return this->launcher_plugins; }
    inline void set_launcher_plugin(QString plugin, bool remove = false)
    {
        if (remove)
            this->launcher_plugins.removeOne(plugin);
        else
            this->launcher_plugins.append(plugin);

        // need to sync idxs
        this->settings.remove("Pages/Launcher");
        for (int i = 0; i < this->launcher_plugins.size(); i++)
            this->settings.setValue(QString("Pages/Launcher/%1").arg(i), this->launcher_plugins[i]);
    }

    static Config *get_instance();

   private:
    QSettings settings;
    double radio_station;
    bool radio_muted;
    QString radio_plugin;
    QString media_home;
    bool si_units;
    bool vehicle_can_bus;
    QString vehicle_interface;
    QString vehicle_plugin;
    QString cam_network_url;
    QString cam_local_device;
    bool cam_is_network;
    QVideoFrame::PixelFormat cam_local_format_override;
    bool cam_autoconnect;
    int cam_autoconnect_time_secs;
    bool cam_overlay;
    int cam_overlay_width;
    int cam_overlay_height;
    bool show_aa_connected;
    bool force_aa_fullscreen;
    QStringList launcher_plugins;

   signals:
    void si_units_changed(bool si_units);
    void cam_autoconnect_changed(bool enabled);
    void cam_overlay_changed(bool enabled);
    void vehicle_can_bus_changed(bool state);
    void vehicle_interface_changed(QString interface);
};
