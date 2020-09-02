#include <QString>
#include <iostream>
#include <stdlib.h>
#include <QByteArray>
#include <boost/log/trivial.hpp>

#include "app/theme.hpp"
#include "canbus/socketcanbus.hpp"
#include "canbus/vehicleinterface.hpp"
#include "plugins/plugin.hpp"
#include "app/tabs/climate.hpp"

#define G37_LOG(severity) BOOST_LOG_TRIVIAL(severity) << "[G37VehiclePlugin] "

class InfinitiG37 : public QObject, public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid)
    Q_INTERFACES(Plugin)

    public:
        bool init(ICANBus* canbus) override;
        QList<QWidget *> tabs() override;

    private:
        static bool duelClimate;

        static void monitorHeadlightStatus(QByteArray payload);
        static void updateClimateDisplay(QByteArray payload);
        static void updateTemperatureDisplay(QByteArray payload);


        static Theme* theme;

        static Climate *climate;
};
