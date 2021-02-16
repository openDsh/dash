#include <QApplication>
#include <QDir>
#include <QTimer>
#include <QProcess>
#include <QDebug>

#include "app/config.hpp"
#include "plugins/brightness_plugin.hpp"

Config::Config()
    : QObject(qApp),
      openauto_config(std::make_shared<openauto::configuration::Configuration>()),
      openauto_button_codes(openauto_config->getButtonCodes()),
      settings()
{
    this->radio_station = this->settings.value("Pages/Media/Radio/station", 98.0).toDouble();
    this->radio_muted = this->settings.value("Pages/Media/Radio/muted", true).toBool();
    this->media_home = this->settings.value("Pages/Media/Local/home", QDir().absolutePath()).toString();
    this->si_units = this->settings.value("Pages/Vehicle/si_units", false).toBool();
    this->vehicle_plugin = this->settings.value("Pages/Vehicle/plugin", QString()).toString();
    this->vehicle_can_bus = this->settings.value("Pages/Vehicle/can_bus", false).toBool();
    this->vehicle_interface = this->settings.value("Pages/Vehicle/interface", QString()).toString();
    this->cam_network_url = this->settings.value("Pages/Camera/stream_url").toString();
    this->cam_local_device = this->settings.value("Pages/Camera/local_device").toString();
    this->cam_is_network = this->settings.value("Pages/Camera/is_network").toBool();
    this->cam_local_format_override = this->settings.value("Pages/Camera/local_format_override").value<QVideoFrame::PixelFormat>();
    this->cam_autoconnect = this->settings.value("Pages/Camera/automatically_reconnect").toBool();
    this->cam_autoconnect_time_secs = this->settings.value("Pages/Camera/auto_reconnect_time_secs", 6).toInt();
    this->settings.beginGroup("Pages/Launcher");
    for (auto key : this->settings.childKeys())
        this->launcher_plugins.append(this->settings.value(key, QString()).toString());
    this->settings.endGroup();
}

Config *Config::get_instance()
{
    static Config config;
    return &config;
}
