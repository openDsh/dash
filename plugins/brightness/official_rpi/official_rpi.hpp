#pragma once

#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>

#include "plugins/brightness_plugin.hpp"

class OfficialRPi : public QObject, BrightnessPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID BrightnessPlugin_iid FILE "official_rpi.json")
    Q_INTERFACES(BrightnessPlugin)

   public:
    OfficialRPi();
    ~OfficialRPi();
    bool is_supported() override;
    uint8_t get_priority() override;
    void set(int brightness) override;

   private:
    const QString PATH = "/sys/class/backlight/rpi_backlight/brightness";

    QFile brightness_attribute;
};
