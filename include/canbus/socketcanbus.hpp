#pragma once

#include <QCanBus> 
#include <string>
#include <QVector>
#include <QByteArray>
#include <QVariant>
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
        void registerFrameHandler(int id, std::function<void(QByteArray)> callback) override;
        bool writeFrame(QCanBusFrame frame) override;

    private:
        bool socketCANAvailable = false;
        QCanBusDevice *bus;
        std::map<int, std::vector<std::function<void(QByteArray)>>> callbacks;
        QList<QCanBusDevice::Filter> filterList;

        QVector<QCanBusFrame> readAllFrames(int numFrames);


    private slots:
        void framesAvailable();
};

