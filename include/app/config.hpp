#pragma once

#include "openauto/Configuration/Configuration.hpp"

#include <QFrame>
#include <QKeySequence>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QWidget>
#include <QVideoFrame>

#include "app/modules/brightness.hpp"

class Config : public QObject {
    Q_OBJECT

   public:
    Config();

    void save();

    inline int get_volume() { return this->volume; }
    inline void set_volume(int volume)
    {
        this->volume = volume;
        emit volume_changed(this->volume);
    }

    inline bool get_dark_mode() { return this->dark_mode; }
    inline void set_dark_mode(bool dark_mode) { this->dark_mode = dark_mode; }

    inline int get_brightness() { return this->brightness; }
    inline void set_brightness(int brightness)
    {
        this->brightness = brightness;
        emit brightness_changed(this->brightness);
    }

    inline bool get_si_units() { return this->si_units; }
    inline void set_si_units(bool si_units)
    {
        this->si_units = si_units;
        emit si_units_changed(this->si_units);
    }

    inline QString get_color() { return this->color; }
    inline void set_color(QString color) { this->color = color; }

    inline QString get_bluetooth_device() { return this->bluetooth_device; }
    inline void set_bluetooth_device(QString bluetooth_device) { this->bluetooth_device = bluetooth_device; }

    inline double get_radio_station() { return this->radio_station; }
    inline void set_radio_station(double radio_station) { this->radio_station = radio_station; }

    inline bool get_radio_muted() { return this->radio_muted; }
    inline void set_radio_muted(bool radio_muted) { this->radio_muted = radio_muted; }

    inline QString get_media_home() { return this->media_home; }
    inline void set_media_home(QString media_home) { this->media_home = media_home; }

    inline bool get_wireless_active() { return this->wireless_active; }
    inline void set_wireless_active(bool wireless_active) { this->wireless_active = wireless_active; }

    inline QString get_wireless_address() { return this->wireless_address; }
    inline void set_wireless_address(QString wireless_address) { this->wireless_address = wireless_address; }

    inline QString get_launcher_home() { return this->launcher_home; }
    inline void set_launcher_home(QString launcher_home) { this->launcher_home = launcher_home; }

    inline bool get_launcher_auto_launch() { return this->launcher_auto_launch; }
    inline void set_launcher_auto_launch(bool launcher_auto_launch) { this->launcher_auto_launch = launcher_auto_launch; }

    inline QString get_launcher_app() { return this->launcher_app; }
    inline void set_launcher_app(QString launcher_app) { this->launcher_app = launcher_app; }

    inline bool get_mouse_active() { return this->mouse_active; }
    inline void set_mouse_active(bool mouse_active) { this->mouse_active = mouse_active; }

    inline void set_shortcut(QString id, QString shortcut) { this->shortcuts[id] = shortcut; }
    inline QString get_shortcut(QString id) { return this->shortcuts[id]; }

    inline QString get_quick_view() { return this->quick_view; }
    inline void set_quick_view(QString quick_view)
    {
        this->quick_view = quick_view;
        emit quick_view_changed(this->quick_view);
    }
    inline QMap<QString, QWidget *> get_quick_views() { return this->quick_views; }
    inline QWidget *get_quick_view(QString name) { return this->quick_views[name]; }
    inline void add_quick_view(QString name, QWidget *view) { this->quick_views[name] = view; }

    inline QString get_brightness_module() { return this->brightness_module; }
    inline void set_brightness_module(QString brightness_module) { this->brightness_module = brightness_module; }
    inline QMap<QString, BrightnessModule *> get_brightness_modules() { return this->brightness_modules; }
    inline BrightnessModule *get_brightness_module(QString name) { return this->brightness_modules[name]; }
    inline void add_brightness_module(QString name, BrightnessModule *module)
    {
        this->brightness_modules[name] = module;
    }

    inline bool get_controls_bar() { return this->controls_bar; }
    inline void set_controls_bar(bool controls_bar)
    {
        this->controls_bar = controls_bar;
        emit controls_bar_changed(this->controls_bar);
    }

    inline double get_scale() { return this->scale; }
    inline void set_scale(double scale)
    {
        this->scale = scale;
        emit scale_changed(this->scale);
    }

    inline bool get_page(QWidget *page) { return this->pages.value(page->objectName(), true); }
    inline void set_page(QWidget *page, bool enabled)
    {
        this->pages[page->objectName()] = enabled;
        emit page_changed(page, enabled);
    }

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

    static Config *get_instance();

   private:
    QMap<QString, QWidget *> quick_views;
    QMap<QString, BrightnessModule *> brightness_modules;

    QSettings ia_config;
    int volume;
    bool dark_mode;
    int brightness;
    bool si_units;
    QString color;
    QString bluetooth_device;
    double radio_station;
    bool radio_muted;
    QString media_home;
    bool wireless_active;
    QString wireless_address;
    QString launcher_home;
    bool launcher_auto_launch;
    QString launcher_app;
    bool mouse_active;
    QMap<QString, QString> shortcuts;
    QString quick_view;
    QString brightness_module;
    bool controls_bar;
    double scale;
    QString cam_network_url;
    QString cam_local_device;
    QString cam_name;
    bool cam_is_network;
    QVideoFrame::PixelFormat cam_local_format_override;
    QMap<QString, bool> pages;

   signals:
    void volume_changed(int volume);
    void brightness_changed(unsigned int brightness);
    void si_units_changed(bool si_units);
    void quick_view_changed(QString quick_view);
    void controls_bar_changed(bool controls_bar);
    void scale_changed(double scale);
    void page_changed(QWidget *page, bool enabled);
    void save_status(bool status);
};

