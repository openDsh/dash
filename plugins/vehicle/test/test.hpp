#pragma once

#include <QObject>
#include "plugins/vehicle_plugin.hpp"
#include "app/widgets/climate.hpp"
#include "app/widgets/vehicle.hpp"
#include "canbus/socketcanbus.hpp"

class Test : public QObject, VehiclePlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehiclePlugin_iid FILE "test.json")
    Q_INTERFACES(VehiclePlugin)

   public:
    Test() {};
    ~Test();
    QList<QWidget *> widgets() override;
    bool init(SocketCANBus*) override;

   private:
    Climate *climate;
    Vehicle *vehicle;
};
