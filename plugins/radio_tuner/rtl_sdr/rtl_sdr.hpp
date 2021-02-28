#pragma once

#include <QObject>
#include <QProcess>
#include <QTcpSocket>

#include "plugins/radio_tuner_plugin.hpp"

class RtlSdr : public QObject, RadioTunerPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID RadioTunerPlugin_iid FILE "rtl_sdr.json")
    Q_INTERFACES(RadioTunerPlugin)

   public:
    RtlSdr();
    bool supported() override;
    void freq(uint32_t hz) override;
    int16_t power_level() override;

   private:
    QProcess server;
    QTcpSocket socket;
};
