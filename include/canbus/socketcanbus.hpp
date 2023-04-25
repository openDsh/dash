#pragma once

#include <QCanBus>
#include <string>
#include <QVector>
#include <QByteArray>
#include <QVariant>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QObject>
#include <functional>
#include "DashLog.hpp"
#include "app/config.hpp"
#include "app/arbiter.hpp"
#include "plugins/vehicle_plugin.hpp"

class SocketCANBus : public QObject
{
    Q_OBJECT
public:
    SocketCANBus(QObject *parent = 0, QString canInterface = "can0");
    ~SocketCANBus();
    static SocketCANBus *get_instance();
    bool writeFrame(QString frame);
    virtual bool init(SocketCANBus*);


private:
    bool socketCANAvailable = false;
    QTcpSocket socket;
    QCanBusDevice *bus;
    int lumws = 10;

signals:

private slots:
    void readFrame();
};
