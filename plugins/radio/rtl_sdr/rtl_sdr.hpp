#pragma once

#include <QObject>
#include <QProcess>
#include <QTcpSocket>

#include "plugins/radio_plugin.hpp"

class RtlSdr : public QObject, RadioPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID RadioPlugin_iid FILE "rtl_sdr.json")
    Q_INTERFACES(RadioPlugin)

   public:
    RtlSdr();
    bool play() override;
    bool stop() override;
    bool freq(int hz) override;

   private:
    QProcess server;
    QProcess player;
    QTcpSocket socket;

    void kill();
};
