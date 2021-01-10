#include <QApplication>
#include <QDir>
#include <QJsonDocument>
#include <QTimer>
#include <QProcess>
#include <QDebug>

#include "app/config.hpp"
#include "plugins/brightness_plugin.hpp"

Config::Server::Server(QObject *parent) : QWebSocketServer("dash", QWebSocketServer::NonSecureMode, parent)
{
    if (this->listen(QHostAddress::Any, this->PORT)) {
        connect(this, &QWebSocketServer::newConnection, [this]() {
            if (auto client = this->nextPendingConnection())
                this->add_client(client);
        });
    }
}

Config::Server::~Server()
{
    this->close();
    for (auto client : this->clients)
        delete client;
}

void Config::Server::add_client(QWebSocket *client)
{
    this->clients.append(client);

    connect(client, &QWebSocket::textMessageReceived, [this, client](QString msg) {
        this->handle_request(client, msg);
    });
}

void Config::Server::handle_request(QWebSocket *client, QString request)
{
    auto json = QJsonDocument::fromJson(request.toUtf8());
    qDebug() << json;
    // client->sendTextMessage(message);
}

Config::Config()
    : QObject(qApp),
      openauto_config(std::make_shared<openauto::configuration::Configuration>()),
      openauto_button_codes(openauto_config->getButtonCodes()),
      settings()
{
    this->load_brightness_plugins();
    this->brightness_active_plugin = new QPluginLoader(this);

    this->volume = this->settings.value("volume", 50).toInt();
    this->dark_mode = this->settings.value("dark_mode", false).toBool();
    this->brightness = this->settings.value("brightness", 255).toInt();
    this->si_units = this->settings.value("si_units", false).toBool();
    this->color_light = this->settings.value("color_light", "#000000").toString();
    this->color_dark = this->settings.value("color_dark", "#ffffff").toString();
    this->bluetooth_device = this->settings.value("Bluetooth/device", QString()).toString();
    this->radio_station = this->settings.value("Radio/station", 98.0).toDouble();
    this->radio_muted = this->settings.value("Radio/muted", true).toBool();
    this->media_home = this->settings.value("media_home", QDir().absolutePath()).toString();
    this->wireless_active = this->settings.value("Wireless/active", false).toBool();
    this->wireless_address = this->settings.value("Wireless/address", "0.0.0.0").toString();
    this->mouse_active = this->settings.value("mouse_active", true).toBool();
    this->quick_view = this->settings.value("quick_view", "none").toString();
    this->brightness_plugin = this->settings.value("brightness_plugin", "mocked").toString();
    this->controls_bar = this->settings.value("controls_bar", false).toBool();
    this->scale = this->settings.value("scale", 1.0).toDouble();
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
    this->settings.beginGroup("Pages");
    for (auto key : this->settings.childKeys())
        this->pages[key] = this->settings.value(key, true).toBool();
    this->settings.endGroup();
    this->settings.beginGroup("Shortcuts");
    for (auto key : this->settings.childKeys())
        this->shortcuts[key] = this->settings.value(key, QString()).toString();
    this->settings.endGroup();
    this->settings.beginGroup("Launcher");
    for (auto key : this->settings.childKeys())
        this->launcher_plugins.append(this->settings.value(key, QString()).toString());
    this->settings.endGroup();

    this->update_system_volume();

    if (this->brightness_active_plugin->isLoaded())
        this->brightness_active_plugin->unload();
    this->brightness_active_plugin->setFileName(this->brightness_plugins[this->brightness_plugin].absoluteFilePath());

    auto socket = new Server(this);
}

Config::~Config()
{
    this->save();
}

void Config::save()
{
    if (this->volume != this->settings.value("volume", 50).toInt())
        this->settings.setValue("volume", this->volume);
    if (this->dark_mode != this->settings.value("dark_mode", false).toBool())
        this->settings.setValue("dark_mode", this->dark_mode);
    if (this->brightness != this->settings.value("brightness", 255).toInt())
        this->settings.setValue("brightness", this->brightness);
    if (this->si_units != this->settings.value("si_units", false).toBool())
        this->settings.setValue("si_units", this->si_units);
    if (this->color_light != this->settings.value("color_light", "#000000").toString())
        this->settings.setValue("color_light", this->color_light);
    if (this->color_dark != this->settings.value("color_dark", "#ffffff").toString())
        this->settings.setValue("color_dark", this->color_dark);
    if (this->bluetooth_device != this->settings.value("Bluetooth/device", QString()).toString())
        this->settings.setValue("Bluetooth/device", this->bluetooth_device);
    if (this->radio_station != this->settings.value("Radio/station", 98.0).toDouble())
        this->settings.setValue("Radio/station", this->radio_station);
    if (this->radio_muted != this->settings.value("Radio/muted", true).toBool())
        this->settings.setValue("Radio/muted", this->radio_muted);
    if (this->media_home != this->settings.value("media_home", QDir().absolutePath()).toString())
        this->settings.setValue("media_home", this->media_home);
    if (this->wireless_active != this->settings.value("Wireless/active", false).toBool())
        this->settings.setValue("Wireless/active", this->wireless_active);
    if (this->wireless_address != this->settings.value("Wireless/address", "0.0.0.0").toString())
        this->settings.setValue("Wireless/address", this->wireless_address);
    if (this->mouse_active != this->settings.value("mouse_active", true).toBool())
        this->settings.setValue("mouse_active", this->mouse_active);
    if (this->quick_view != this->settings.value("quick_view", "volume").toString())
        this->settings.setValue("quick_view", this->quick_view);
    if (this->brightness_plugin != this->settings.value("brightness_plugin", "mocked").toString())
        this->settings.setValue("brightness_plugin", this->brightness_plugin);
    if (this->controls_bar != this->settings.value("controls_bar", false).toBool())
        this->settings.setValue("controls_bar", this->controls_bar);
    if (this->scale != this->settings.value("scale", 1.0).toDouble())
        this->settings.setValue("scale", this->scale);
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
    for (auto id : this->pages.keys()) {
        QString key = QString("Pages/%1").arg(id);
        bool page_enabled = this->pages[id];
        if (page_enabled != this->settings.value(key, true).toBool())
            this->settings.setValue(key, page_enabled);
    }
    for (auto id : this->shortcuts.keys()) {
        QString key = QString("Shortcuts/%1").arg(id);
        QString shortcut = this->shortcuts[id];
        if (shortcut != this->settings.value(key, QString()).toString())
            this->settings.setValue(key, shortcut);
    }
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

void Config::load_brightness_plugins()
{
    for (const QFileInfo &plugin : Config::plugin_dir("brightness").entryInfoList(QDir::Files)) {
        if (QLibrary::isLibrary(plugin.absoluteFilePath()))
            this->brightness_plugins[Config::fmt_plugin(plugin.baseName())] = plugin;
    }
}

void Config::update_system_volume()
{
    static QString command("amixer set Master %1% --quiet");

    QProcess *lProc = new QProcess();
    lProc->start(command.arg(this->volume));
    lProc->waitForFinished();
}

void Config::set_volume(int volume)
{
    this->volume = std::max(0, std::min(volume, 100));
    this->update_system_volume();
    emit volume_changed(this->volume);
}

void Config::set_brightness(int brightness)
{
    this->brightness = std::max(76, std::min(brightness, 255));

    if (BrightnessPlugin *plugin = qobject_cast<BrightnessPlugin *>(this->brightness_active_plugin->instance()))
        plugin->set(this->brightness);

    emit brightness_changed(this->brightness);
}
