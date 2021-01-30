#pragma once

#include <QObject>
#include "plugins/brightness_plugin.hpp"

class X : public QObject, BrightnessPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID BrightnessPlugin_iid FILE "x.json")
    Q_INTERFACES(BrightnessPlugin)

   public:
    X();
    bool is_supported() override;
    uint8_t get_priority() override;
    void set(int brightness) override;

   private:
    QScreen *screen;
};
