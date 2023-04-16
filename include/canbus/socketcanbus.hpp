#pragma once

#include <QCanBus>
#include <string>
#include <QVector>
#include <QByteArray>
#include <QVariant>
#include <QTcpSocket>
//#include <QAbstractSocket>
#include "DashLog.hpp"
#include "canbus/ICANBus.hpp"
#include "app/config.hpp"

class SocketCANBus : public ICANBus
{
    Q_OBJECT
public:
    SocketCANBus(QString canInterface = "can0");
    ~SocketCANBus();
    static SocketCANBus *get_instance();
    bool writeFrame(QString frame) override;

private:
    bool socketCANAvailable = false;
    QTcpSocket socket;
    QCanBusDevice *bus;
    std::map<int, std::vector<std::function<void(QByteArray)>>> callbacks;

signals:

public slots:
    void readyRead();
};
