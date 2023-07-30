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
    int tempAC = 0;
    int fanAC = 0;
    int ttCool = 0;
    int ttlvlCool = 0;
    int kmsalvati = 0;
    int rpmsalvati = 0;
    int velsalvata = 0;
    double tempsalvata = 0;
    double lastcons = 0;
    double ttlastcons = 0;
    int ttOil = 0;
    double ttVolt = 0;
    double ttBenz = 0;
    double codBenz [100] = {52.0, 52.0, 52.0, 52.0, 52.0, 51.2, 50.5, 49.8, 49.1, 48.4, 47.7, 47.0, 46.2, 45.2, 44.8, 44.1, 43.4, 42.7, 42.0, 41.3, 40.7, 40.1, 39.5, 38.8, 38.2, 37.6, 37.0, 36.3, 35.7, 35.1, 34.5, 33.8, 33.2, 32.6, 32.0, 31.3, 30.6, 30.0, 29.3, 28.6, 28.0, 27.3, 26.6, 26.0, 25.3, 24.6, 24.0, 23.3, 22.6, 22.0, 21.3, 20.7, 20.0, 19.4, 18.7, 18.1, 17.5, 16.8, 16.2, 15.5, 14.9, 14.2, 13.6, 13.0, 12.5, 12.1, 11.6, 11.2, 10.8, 10.3, 9.9, 9.5, 9.0, 8.6, 8.1, 7.7, 7.3, 6.8, 6.4, 6.0, 5.7, 5.4, 5.1, 4.8, 4.5, 4.2, 4.0, 3.7, 3.4, 3.1, 2.8, 2.5, 2.2, 2.0, 1.7, 1.4, 1.1, 0.8, 0.5, 0.2};
    bool premuto = false;
    SocketCANBus *bus = nullptr;

signals:

private slots:
    void readFrame();
};
