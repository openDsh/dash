#pragma once

#include <QCanBus>
#include <string>
#include <QVector>
#include <QByteArray>
#include <QVariant>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "DashLog.hpp"
#include "canbus/ICANBus.hpp"
#include "app/config.hpp"

class SocketCANBus : public QObject
{
    Q_OBJECT
public:
    SocketCANBus(QObject *parent = 0, QString canInterface = "can0");
    ~SocketCANBus();
    bool writeFrame(QString frame);

private:
    bool socketCANAvailable = false;
    QTcpSocket socket;
    QCanBusDevice *bus;
    int lumws = 10;

signals:

private slots:
    void readFrame();
};
