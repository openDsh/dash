#pragma once

#include <QList>
#include <QWidget>

#include "plugins/plugin.hpp"
#include "canbus/socketcanbus.hpp"

class VehiclePlugin : public Plugin {
   public:
    virtual ~VehiclePlugin() = default;
    virtual bool init(ICANBus* canbus) = 0;
};

#define VehiclePlugin_iid "org.dash.VehiclePlugin"

Q_DECLARE_INTERFACE(VehiclePlugin, VehiclePlugin_iid)
