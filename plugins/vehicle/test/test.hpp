#pragma once

#include <QObject>
#include <QTcpSocket>
#include "plugins/vehicle_plugin.hpp"
#include "app/widgets/climate.hpp"
#include "app/widgets/vehicle.hpp"
#include "canbus/socketcanbus.hpp"
#include "app/arbiter.hpp"

class Test : public QObject, VehiclePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehiclePlugin_iid FILE "test.json")
    Q_INTERFACES(VehiclePlugin)

public:
    Test(){};
    ~Test();
    QList<QWidget *> widgets() override;
    bool init(SocketCANBus *bus) override;

    inline void socketcan(SocketCANBus *bus)
    {
        if (!this->bus)
            this->bus = bus;
    }

private:
    Climate *climate;
    Vehicle *vehicle;
    int lumws = 0;
    int ttCool = 0;
    SocketCANBus *bus = nullptr;

signals:

private slots:
    void readFrame();
};
