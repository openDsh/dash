#include <QString>
#include <iostream>
#include <stdlib.h>
#include <QByteArray>
#include <boost/log/trivial.hpp>

#include "app/theme.hpp"
#include "canbus/socketcanbus.hpp"
#include "canbus/vehicleinterface.hpp"

#define G37_LOG(severity) BOOST_LOG_TRIVIAL(severity) << "[G37VehiclePlugin] "

class InfinitiG37 : public QObject, public VehicleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehicleInterface_iid)
    Q_INTERFACES(VehicleInterface)

    public:
        bool init(ICANBus* canbus) override;

    private:
        static void monitorHeadlightStatus(QByteArray payload);
        static void updateClimateDisplay(QByteArray payload);

        static Theme* theme;
};
