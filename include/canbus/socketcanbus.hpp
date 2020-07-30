#pragma once

#include <QCanBus> 
#include <string>
#include <QVector>
#include <QByteArray>
#include <QVariant>

#include "DashLog.hpp"

class SocketCANBus : public QObject
{
    Q_OBJECT
    public:
        SocketCANBus(QString canInterface = "can0");
        ~SocketCANBus();
        static SocketCANBus *get_instance();
        void registerFrameHandler(int id, std::function<void(QByteArray)> callback);
        bool writeFrame(QCanBusFrame frame);

    private:
        bool socketCANAvailable = false;
        QCanBusDevice *bus;
        std::map<int, std::vector<std::function<void(QByteArray)>>> callbacks;
        QList<QCanBusDevice::Filter> filterList;

    private slots:
        void framesAvailable();
};

