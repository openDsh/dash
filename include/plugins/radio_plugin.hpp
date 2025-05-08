#pragma once

#include "plugins/plugin.hpp"

class RadioPlugin : public Plugin {
   public:
    RadioPlugin() { this->settings.beginGroup("Radio"); }
    virtual ~RadioPlugin() = default;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void freq(int hz) = 0;
};

#define RadioPlugin_iid "openDsh.plugins.RadioPlugin"

Q_DECLARE_INTERFACE(RadioPlugin, RadioPlugin_iid)
