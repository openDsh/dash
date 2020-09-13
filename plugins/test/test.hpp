#pragma once

#include <QObject>
#include "plugins/vehicle_plugin.hpp"
#include "app/tabs/climate.hpp"

class Test : public QObject, VehiclePlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehiclePlugin_iid FILE "test.json")
    Q_INTERFACES(VehiclePlugin)

   public:
    Test();
    ~Test();
    QList<QWidget *> widgets() override;
    bool init() override;

   private:
    Climate *climate;
};
