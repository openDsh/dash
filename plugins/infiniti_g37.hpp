#include <QString>
#include <iostream>
#include <stdlib.h>
#include <QByteArray>
#include "app/theme.hpp"
#include "canbus/socketcanbus.hpp"
#include "canbus/vehicleinterface.hpp"

class InfinitiG37 : public QObject, public VehicleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehicleInterface_iid)
    Q_INTERFACES(VehicleInterface)

    public:
        bool init(SocketCANBus* canbus, Theme* theme) override;

    private:
        static void monitorHeadlightStatus(QByteArray payload);
        static void updateClimateDisplay(QByteArray payload);

        static Theme* theme;
};
