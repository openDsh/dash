#pragma once

#include "plugins/plugin.hpp"

class RadioTunerPlugin : public Plugin {
   public:
    RadioTunerPlugin() { this->settings.beginGroup("RadioTuner"); }
    virtual ~RadioTunerPlugin() = default;
    virtual bool supported() = 0;
    virtual void freq(uint32_t hz) = 0;
    virtual int16_t power_level() = 0;
};

#define RadioTunerPlugin_iid "openDsh.plugins.RadioTunerPlugin"

Q_DECLARE_INTERFACE(RadioTunerPlugin, RadioTunerPlugin_iid)
