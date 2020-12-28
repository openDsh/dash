#pragma once

#include <QList>
#include <QWidget>

#include "plugins/plugin.hpp"

class VehiclePlugin : public Plugin {
   public:
    VehiclePlugin() { this->settings.beginGroup("Vehicle"); }
    virtual ~VehiclePlugin() = default;
    virtual bool init() = 0;
};

#define VehiclePlugin_iid "openDsh.plugins.VehiclePlugin"

Q_DECLARE_INTERFACE(VehiclePlugin, VehiclePlugin_iid)
