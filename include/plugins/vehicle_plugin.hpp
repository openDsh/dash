#pragma once

#include "plugins/plugin.hpp"
#include "canbus/socketcanbus.hpp"

class VehiclePlugin : public Plugin {
   public:
    VehiclePlugin() { this->settings.beginGroup("Vehicle"); }
    virtual ~VehiclePlugin() = default;
    virtual bool init(ICANBus* canbus) = 0;
};

#define VehiclePlugin_iid "openDsh.plugins.VehiclePlugin"

Q_DECLARE_INTERFACE(VehiclePlugin, VehiclePlugin_iid)
