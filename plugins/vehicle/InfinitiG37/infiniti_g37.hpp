#include <QString>
#include <iostream>
#include <stdlib.h>
#include <QByteArray>
#include <boost/log/trivial.hpp>

#include "app/theme.hpp"

#include "plugins/vehicle_plugin.hpp"
#include "app/widgets/climate.hpp"

#define G37_LOG(severity) BOOST_LOG_TRIVIAL(severity) << "[G37VehiclePlugin] "

class InfinitiG37 : public QObject, VehiclePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehiclePlugin_iid)
    Q_INTERFACES(VehiclePlugin)

    public:
        bool init(ICANBus* canbus) override;

    private:
        QList<QWidget *> widgets() override;

        static bool duelClimate;

        static void monitorHeadlightStatus(QByteArray payload);
        static void updateClimateDisplay(QByteArray payload);
        static void updateTemperatureDisplay(QByteArray payload);


        static Theme* theme;

        static Climate *climate;
};
