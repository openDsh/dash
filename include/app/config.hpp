#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>

#include <QObject>
#include <QSettings>
#include <QString>

class Config : public QObject {
    Q_OBJECT

   public:
    Config();

    void save();

    inline int get_volume() { return this->volume; }
    inline void set_volume(int volume) { this->volume = volume; }

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

    std::shared_ptr<f1x::openauto::autoapp::configuration::Configuration> openauto_config;

    static Config *get_instance();

   private:
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

   signals:
    void brightness_changed(unsigned int brightness);
    void si_units_changed(bool si_units);
};

#endif
