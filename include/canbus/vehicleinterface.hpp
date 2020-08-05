#pragma once
#include "ICANBus.hpp"

class VehicleInterface
{
public:
    virtual ~VehicleInterface() = default;
    virtual bool init(ICANBus* canbus) = 0;
};

#define VehicleInterface_iid "org.dash.canbus.VehicleInterface"

Q_DECLARE_INTERFACE(VehicleInterface, VehicleInterface_iid)