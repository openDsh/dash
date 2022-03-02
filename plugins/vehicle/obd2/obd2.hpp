#include <QString>
#include <iostream>
#include <stdlib.h>
#include <QByteArray>
#include <boost/log/trivial.hpp>

#include "plugins/vehicle_plugin.hpp"
#include "obd/command.hpp"
#include "obd/conversions.hpp"

#include "app/arbiter.hpp"
#include "openauto/Service/InputService.hpp"

#define OBD2_LOG(severity) BOOST_LOG_TRIVIAL(severity) << "[OBD2Plugin] "

class Obd2 : public QObject, VehiclePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehiclePlugin_iid)
    Q_INTERFACES(VehiclePlugin)

    public:
        bool init(ICANBus* canbus) override;

    private:
        QList<QWidget *> widgets() override;
        double maf_rate;
        void readObd2(Command cmd, QByteArray payload);
};
