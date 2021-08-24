#pragma once

#include <QObject>
#include <QMainWindow>

#include "plugins/brightness_plugin.hpp"

class Mocked : public BrightnessPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID BrightnessPlugin_iid FILE "mocked.json")
    Q_INTERFACES(BrightnessPlugin)

   public:
    Mocked();
    bool supported() override;
    uint8_t priority() override;
    void set(int brightness) override;

   private:
    QMainWindow *window;
};
