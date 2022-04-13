#include <QApplication>
#include <QDir>

#include "app/config.hpp"

Config::Config()
    : QObject(qApp),
      openauto_config(std::make_shared<openauto::configuration::Configuration>()),
      openauto_button_codes(openauto_config->getButtonCodes()),
      settings()
{
    this->radio_station = this->settings.value("Pages/Media/Radio/station", 98.0).toDouble();
    this->radio_muted = this->settings.value("Pages/Media/Radio/muted", true).toBool();
    this->radio_plugin = this->settings.value("Pages/Media/Radio/plugin", "unloader").toString();
    this->media_home = this->settings.value("Pages/Media/Local/home", QDir().absolutePath()).toString();
    this->si_units = this->settings.value("Pages/Vehicle/si_units", false).toBool();
    // 0 - SocketCAN
    // 1 - Elm 327 USB
    // 2 - Elm 327 Bluetooth
    this->vehicle_can_bus = (ICANBus::VehicleBusType)(this->settings.value("Pages/Vehicle/can_bus", ICANBus::VehicleBusType::SocketCAN).toInt());
    this->vehicle_interface = this->settings.value("Pages/Vehicle/interface", "disabled").toString();
    this->vehicle_plugin = this->settings.value("Pages/Vehicle/plugin", "unloader").toString();
    this->cam_network_url = this->settings.value("Pages/Camera/stream_url", QString()).toString();
    this->cam_local_device = this->settings.value("Pages/Camera/local_device", QString()).toString();
    this->cam_is_network = this->settings.value("Pages/Camera/is_network", false).toBool();
    this->cam_local_format_override = this->settings.value("Pages/Camera/local_format_override", QVideoFrame::Format_Invalid).value<QVideoFrame::PixelFormat>();
    this->cam_autoconnect = this->settings.value("Pages/Camera/automatically_reconnect", false).toBool();
    this->cam_autoconnect_time_secs = this->settings.value("Pages/Camera/auto_reconnect_time_secs", 6).toInt();
    this->cam_overlay = this->settings.value("Pages/Camera/Overlay/enabled", false).toBool();
    this->cam_overlay_width = this->settings.value("Pages/Camera/Overlay/width", 100).toInt();
    this->cam_overlay_height = this->settings.value("Pages/Camera/Overlay/height", 100).toInt();
    this->show_aa_connected = this->settings.value("Pages/OpenAuto/show_aa_connected", 100).toBool();
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
