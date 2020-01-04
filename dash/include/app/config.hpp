#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>

#include <QObject>
#include <QSettings>
#include <QString>

class Config : public QObject {
    Q_OBJECT

   public:
    Config(QObject *parent = nullptr);

    static Config *get_instance();
    std::shared_ptr<f1x::openauto::autoapp::configuration::Configuration> open_auto_config;

    int get_volume() { return this->volume; }

    void set_volume(int volume) { this->volume = volume; }

    bool get_dark_mode() { return this->dark_mode; }

    void set_dark_mode(bool dark_mode) { this->dark_mode = dark_mode; }

    int get_brightness() { return this->brightness; }

    void set_brightness(int brightness) { this->brightness = brightness; }

    bool get_si_units() { return this->si_units; }

    void set_si_units(bool si_units) { this->si_units = si_units; }

    QString get_color() { return this->color; }

    void set_color(QString color) { this->color = color; }

    QString get_bluetooth_device() { return this->bluetooth_device; }

    void set_bluetooth_device(QString bluetooth_device) { this->bluetooth_device = bluetooth_device; }

    double get_radio_station() { return this->radio_station; }

    void set_radio_station(double radio_station) { this->radio_station = radio_station; }

    bool get_radio_muted() { return this->radio_muted; }

    void set_radio_muted(bool radio_muted) { this->radio_muted = radio_muted; }

    void save();

   signals:
    void open_auto_updated();
    void si_units_changed();

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
};

#endif
