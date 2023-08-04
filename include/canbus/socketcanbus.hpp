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

class SocketCANBus : public QObject
{
    Q_OBJECT
public:
    SocketCANBus(QObject *parent = 0, QString canInterface = "can0");
    ~SocketCANBus();
    static SocketCANBus *get_instance();
    QTcpSocket socket;

private:
    bool socketCANAvailable = false;
    QCanBusDevice *bus;
};
