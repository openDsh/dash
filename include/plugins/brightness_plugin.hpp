#pragma once

#include <QObject>
#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>

#include "plugins/plugin.hpp"

class BrightnessPlugin : public Plugin {
   public:
    BrightnessPlugin() { this->settings.beginGroup("Brightness"); }
    virtual ~BrightnessPlugin() = default;
    virtual bool is_supported() = 0;
    virtual uint8_t get_priority() = 0;
    virtual void set(int brightness) = 0;
};

#define BrightnessPlugin_iid "openDsh.plugins.BrightnessPlugin"

Q_DECLARE_INTERFACE(BrightnessPlugin, BrightnessPlugin_iid)
