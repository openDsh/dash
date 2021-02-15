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
    this->si_units = this->settings.value("si_units", false).toBool();
    this->radio_station = this->settings.value("Radio/station", 98.0).toDouble();
    this->radio_muted = this->settings.value("Radio/muted", true).toBool();
    this->media_home = this->settings.value("media_home", QDir().absolutePath()).toString();
    this->cam_name = this->settings.value("Camera/name").toString();
    this->cam_network_url = this->settings.value("Camera/stream_url").toString();
    this->cam_local_device = this->settings.value("Camera/local_device").toString();
    this->cam_is_network = this->settings.value("Camera/is_network").toBool();
    this->cam_local_format_override = this->settings.value("Camera/local_format_override").value<QVideoFrame::PixelFormat>();
    this->cam_autoconnect = this->settings.value("Camera/automatically_reconnect").toBool();
    this->cam_autoconnect_time_secs = this->settings.value("Camera/auto_reconnect_time_secs", 6).toInt();
    this->vehicle_plugin = this->settings.value("Vehicle/plugin", QString()).toString();
    this->vehicle_can_bus = this->settings.value("Vehicle/can_bus", false).toBool();
    this->vehicle_interface = this->settings.value("Vehicle/interface", QString()).toString();
    this->settings.beginGroup("Launcher");
    for (auto key : this->settings.childKeys())
        this->launcher_plugins.append(this->settings.value(key, QString()).toString());
    this->settings.endGroup();
}

Config::~Config()
{
    this->save();
}

void Config::save()
{
    if (this->si_units != this->settings.value("si_units", false).toBool())
        this->settings.setValue("si_units", this->si_units);
    if (this->radio_station != this->settings.value("Radio/station", 98.0).toDouble())
        this->settings.setValue("Radio/station", this->radio_station);
    if (this->radio_muted != this->settings.value("Radio/muted", true).toBool())
        this->settings.setValue("Radio/muted", this->radio_muted);
    if (this->media_home != this->settings.value("media_home", QDir().absolutePath()).toString())
        this->settings.setValue("media_home", this->media_home);
    if (this->cam_name != this->settings.value("Camera/name").toString())
        this->settings.setValue("Camera/name", this->cam_name);
    if (this->cam_network_url != this->settings.value("Camera/stream_url").toString())
        this->settings.setValue("Camera/stream_url", this->cam_network_url);
    if (this->cam_local_device != this->settings.value("Camera/local_device").toString())
        this->settings.setValue("Camera/local_device", this->cam_local_device);
    if (this->cam_is_network != this->settings.value("Camera/is_network").toBool())
        this->settings.setValue("Camera/is_network", this->cam_is_network);
    if (this->cam_local_format_override != this->settings.value("Camera/local_format_override").value<QVideoFrame::PixelFormat>())
        this->settings.setValue("Camera/local_format_override", this->cam_local_format_override);
    if (this->cam_autoconnect != this->settings.value("Camera/automatically_reconnect").toBool())
        this->settings.setValue("Camera/automatically_reconnect", this->cam_autoconnect);
    if (this->cam_autoconnect_time_secs != this->settings.value("Camera/auto_reconnect_time_secs").toInt())
        this->settings.setValue("Camera/auto_reconnect_time_secs", this->cam_autoconnect_time_secs);
    if (this->vehicle_plugin != this->settings.value("Vehicle/plugin").toString())
        this->settings.setValue("Vehicle/plugin", this->vehicle_plugin);
    if (this->vehicle_can_bus != this->settings.value("Vehicle/can_bus").toBool())
        this->settings.setValue("Vehicle/can_bus", this->vehicle_can_bus);
    if (this->vehicle_interface != this->settings.value("Vehicle/interface").toString())
        this->settings.setValue("Vehicle/interface", this->vehicle_interface);
    this->settings.remove("Launcher");
    for (int i = 0; i < this->launcher_plugins.size(); i++)
        this->settings.setValue(QString("Launcher/%1").arg(i), this->launcher_plugins[i]);

    this->settings.sync();
}

Config *Config::get_instance()
{
    static Config config;
    return &config;
}
