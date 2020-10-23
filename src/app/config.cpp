#include <QApplication>
#include <QDir>
#include <QTimer>
#include <QProcess>

#include "app/config.hpp"
#include "plugins/brightness_plugin.hpp"

const QDir Config::BRIGHTNESS_PLUGIN_DIR(Config::plugin_dir("brightness"));

Config::Config()
    : QObject(qApp),
      openauto_config(std::make_shared<openauto::configuration::Configuration>()),
      openauto_button_codes(openauto_config->getButtonCodes()),
      ia_config(QSettings::IniFormat, QSettings::UserScope, "dash")
{
    this->load_brightness_plugins();
    this->brightness_active_plugin = new QPluginLoader(this);

    this->volume = this->ia_config.value("volume", 50).toInt();
    this->dark_mode = this->ia_config.value("dark_mode", false).toBool();
    this->brightness = this->ia_config.value("brightness", 255).toInt();
    this->si_units = this->ia_config.value("si_units", false).toBool();
    this->color = this->ia_config.value("color", "azure").toString();
    this->bluetooth_device = this->ia_config.value("Bluetooth/device", QString()).toString();
    this->radio_station = this->ia_config.value("Radio/station", 98.0).toDouble();
    this->radio_muted = this->ia_config.value("Radio/muted", true).toBool();
    this->media_home = this->ia_config.value("media_home", QDir().absolutePath()).toString();
    this->wireless_active = this->ia_config.value("Wireless/active", false).toBool();
    this->wireless_address = this->ia_config.value("Wireless/address", "0.0.0.0").toString();
    this->mouse_active = this->ia_config.value("mouse_active", true).toBool();
    this->launcher_home = this->ia_config.value("Launcher/home", QDir().absolutePath()).toString();
    this->launcher_auto_launch = this->ia_config.value("Launcher/auto_launch", false).toBool();
    this->launcher_app = this->ia_config.value("Launcher/app", QString()).toString();
    this->quick_view = this->ia_config.value("quick_view", "volume").toString();
    this->brightness_module = this->ia_config.value("brightness_module", "mocked").toString();
    this->brightness_plugin = this->ia_config.value("brightness_plugin", "libmocked").toString();
    this->controls_bar = this->ia_config.value("controls_bar", false).toBool();
    this->scale = this->ia_config.value("scale", 1.0).toDouble();
    this->cam_name = this->ia_config.value("Camera/name").toString();
    this->cam_network_url = this->ia_config.value("Camera/stream_url").toString();
    this->cam_local_device = this->ia_config.value("Camera/local_device").toString();
    this->cam_is_network = this->ia_config.value("Camera/is_network").toBool();
    this->cam_local_format_override = this->ia_config.value("Camera/local_format_override").value<QVideoFrame::PixelFormat>();
    this->cam_autoconnect = this->ia_config.value("Camera/automatically_reconnect").toBool();
    this->cam_autoconnect_time_secs = this->ia_config.value("Camera/auto_reconnect_time_secs", 6).toInt();
    this->ia_config.beginGroup("Pages");
    for (auto key : this->ia_config.childKeys())
        this->pages[key] = this->ia_config.value(key, true).toBool();
    this->ia_config.endGroup();
    this->ia_config.beginGroup("Shortcuts");
    for (auto key : this->ia_config.childKeys())
        this->shortcuts[key] = this->ia_config.value(key, QString()).toString();
    this->ia_config.endGroup();

    this->update_system_volume();

    if (this->brightness_active_plugin->isLoaded())
        this->brightness_active_plugin->unload();
    this->brightness_active_plugin->setFileName(this->brightness_plugins[this->brightness_plugin].absoluteFilePath());
}

void Config::save()
{
    if (this->volume != this->ia_config.value("volume", 50).toInt())
        this->ia_config.setValue("volume", this->volume);
    if (this->dark_mode != this->ia_config.value("dark_mode", false).toBool())
        this->ia_config.setValue("dark_mode", this->dark_mode);
    if (this->brightness != this->ia_config.value("brightness", 255).toInt())
        this->ia_config.setValue("brightness", this->brightness);
    if (this->si_units != this->ia_config.value("si_units", false).toBool())
        this->ia_config.setValue("si_units", this->si_units);
    if (this->color != this->ia_config.value("color", "azure").toString())
        this->ia_config.setValue("color", this->color);
    if (this->bluetooth_device != this->ia_config.value("Bluetooth/device", QString()).toString())
        this->ia_config.setValue("Bluetooth/device", this->bluetooth_device);
    if (this->radio_station != this->ia_config.value("Radio/station", 98.0).toDouble())
        this->ia_config.setValue("Radio/station", this->radio_station);
    if (this->radio_muted != this->ia_config.value("Radio/muted", true).toBool())
        this->ia_config.setValue("Radio/muted", this->radio_muted);
    if (this->media_home != this->ia_config.value("media_home", QDir().absolutePath()).toString())
        this->ia_config.setValue("media_home", this->media_home);
    if (this->wireless_active != this->ia_config.value("Wireless/active", false).toBool())
        this->ia_config.setValue("Wireless/active", this->wireless_active);
    if (this->wireless_address != this->ia_config.value("Wireless/address", "0.0.0.0").toString())
        this->ia_config.setValue("Wireless/address", this->wireless_address);
    if (this->launcher_home != this->ia_config.value("Launcher/home", QDir().absolutePath()).toString())
        this->ia_config.setValue("Launcher/home", this->launcher_home);
    if (this->launcher_auto_launch != this->ia_config.value("Launcher/auto_launch", false).toBool())
        this->ia_config.setValue("Launcher/auto_launch", this->launcher_auto_launch);
    if (this->launcher_app != this->ia_config.value("Launcher/app", QString()).toString())
        this->ia_config.setValue("Launcher/app", this->launcher_app);
    if (this->mouse_active != this->ia_config.value("mouse_active", true).toBool())
        this->ia_config.setValue("mouse_active", this->mouse_active);
    if (this->quick_view != this->ia_config.value("quick_view", "volume").toString())
        this->ia_config.setValue("quick_view", this->quick_view);
    if (this->brightness_module != this->ia_config.value("brightness_module", "mocked").toString())
        this->ia_config.setValue("brightness_module", this->brightness_module);
    if (this->brightness_plugin != this->ia_config.value("brightness_plugin", "libmocked").toString())
        this->ia_config.setValue("brightness_plugin", this->brightness_plugin);
    if (this->controls_bar != this->ia_config.value("controls_bar", false).toBool())
        this->ia_config.setValue("controls_bar", this->controls_bar);
    if (this->scale != this->ia_config.value("scale", 1.0).toDouble())
        this->ia_config.setValue("scale", this->scale);
    if (this->cam_name != this->ia_config.value("Camera/name").toString())
        this->ia_config.setValue("Camera/name", this->cam_name);
    if (this->cam_network_url != this->ia_config.value("Camera/stream_url").toString())
        this->ia_config.setValue("Camera/stream_url", this->cam_network_url);
    if (this->cam_local_device != this->ia_config.value("Camera/local_device").toString())
        this->ia_config.setValue("Camera/local_device", this->cam_local_device);
    if (this->cam_is_network != this->ia_config.value("Camera/is_network").toBool())
        this->ia_config.setValue("Camera/is_network", this->cam_is_network);
    if (this->cam_local_format_override != this->ia_config.value("Camera/local_format_override").value<QVideoFrame::PixelFormat>())
        this->ia_config.setValue("Camera/local_format_override", this->cam_local_format_override);
    if (this->cam_autoconnect != this->ia_config.value("Camera/automatically_reconnect").toBool())
        this->ia_config.setValue("Camera/automatically_reconnect", this->cam_autoconnect);
    if (this->cam_autoconnect_time_secs != this->ia_config.value("Camera/auto_reconnect_time_secs").toInt())
        this->ia_config.setValue("Camera/auto_reconnect_time_secs", this->cam_autoconnect_time_secs);
    for (auto id : this->pages.keys()) {
        QString config_key = QString("Pages/%1").arg(id);
        bool page_enabled = this->pages[id];
        if (page_enabled != this->ia_config.value(config_key, true).toBool())
            this->ia_config.setValue(config_key, page_enabled);
    }
    for (auto id : this->shortcuts.keys()) {
        QString config_key = QString("Shortcuts/%1").arg(id);
        QString shortcut = this->shortcuts[id];
        if (shortcut != this->ia_config.value(config_key, QString()).toString())
            this->ia_config.setValue(config_key, shortcut);
    }

    this->ia_config.sync();
}

Config *Config::get_instance()
{
    static Config config;
    return &config;
}

void Config::load_brightness_plugins()
{
    for (const QFileInfo &plugin : this->BRIGHTNESS_PLUGIN_DIR.entryInfoList(QDir::Files)) {
        if (QLibrary::isLibrary(plugin.absoluteFilePath()))
            this->brightness_plugins[plugin.baseName()] = plugin;
    }
}

void Config::update_system_volume()
{
    static QString command("amixer set Master %1%% --quiet");

    QProcess *lProc = new QProcess();
    lProc->start(command.arg(this->volume));
    lProc->waitForFinished();
}

void Config::set_volume(int volume)
{
    this->volume = volume;
    this->update_system_volume();
    emit volume_changed(this->volume);
}

void Config::set_brightness(int brightness)
{
    this->brightness = brightness;

    if (BrightnessPlugin *plugin = qobject_cast<BrightnessPlugin *>(this->brightness_active_plugin->instance()))
        plugin->set(this->brightness);

    emit brightness_changed(this->brightness);
}
