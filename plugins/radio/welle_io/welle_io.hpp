#pragma once

#include <QObject>
#include <QProcess>
#include <QTcpSocket>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "plugins/radio_plugin.hpp"

class WelleIo : public QObject, RadioPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID RadioPlugin_iid FILE "welle_io.json")
    Q_INTERFACES(RadioPlugin)

   public:
    WelleIo();
    void play() override;
    void stop() override;
    void freq(int hz) override;
    QWidget* get_widget() override;

   private:
    QProcess server;
    QProcess player;
    QTcpSocket socket;
};
