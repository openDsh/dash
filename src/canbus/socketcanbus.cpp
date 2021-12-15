#include "canbus/socketcanbus.hpp"

SocketCANBus::SocketCANBus(QString canInterface)
{
    if (QCanBus::instance()->plugins().contains(QStringLiteral("socketcan"))) {
        QString errorString;
        // const QList<QCanBusDeviceInfo> devices = ;
        // if (!errorString.isEmpty()) {
        //     DASH_LOG(error) << errorString;
        //     return;
        // }
        auto devices = QCanBus::instance()->availableDevices(
                QStringLiteral("socketcan"), &errorString);
        DASH_LOG(info) <<"[SocketCANBus] Found socket can devices (nct): "<<devices.length();
        for (auto device:QCanBus::instance()->availableDevices(
            QStringLiteral("socketcan"), &errorString)) {
            if (device.name() == "can0")
                DASH_LOG(info) <<"[SocketCANBus] Found can0 device: "<<device.name().toStdString();
            else
                DASH_LOG(info) <<"[SocketCANBus] Found some other device: ";
        }

        DASH_LOG(info) << "[SocketCANBus] 'socketcan' Available";
        socketCANAvailable = true;
        bus = QCanBus::instance()->createDevice(
            QStringLiteral("socketcan"), canInterface, &errorString);
        if (!bus) {
            DASH_LOG(error) <<"[SocketCANBus] Error creating CAN device, " << errorString.toStdString();
        } else {
            DASH_LOG(info) <<"[SocketCANBus] Connecting CAN interface "<<canInterface.toStdString();
            if (bus->connectDevice()) {
                connect(bus, &QCanBusDevice::framesReceived, this, &SocketCANBus::framesAvailable);
                DASH_LOG(info) <<"[SocketCANBus] Connected "<<canInterface.toStdString();
            } else {
                DASH_LOG(error) <<"[SocketCANBus] Error connecting CAN device";
                bus = NULL;
            }
        }
    }
}

SocketCANBus::~SocketCANBus()
{
    DASH_LOG(info) << "[SocketCANBus] Disconnecting and deleting bus";
    if(bus->state() == QCanBusDevice::ConnectedState){
        bus->disconnectDevice();
    }
}

bool SocketCANBus::writeFrame(QCanBusFrame frame)
{
    if(bus && bus->state() != QCanBusDevice::ConnectedState) return false;
    return bus->writeFrame(frame);
}

SocketCANBus *SocketCANBus::get_instance()
{
    static SocketCANBus bus(Config::get_instance()->get_vehicle_interface());
    return &bus;
}

// QSerialBus readAllFrames introduced in 5.12 - Pi OS is stuck on 5.11, so implement our own
QVector<QCanBusFrame> SocketCANBus::readAllFrames(int numFrames){
    QVector<QCanBusFrame> frames = QVector<QCanBusFrame>();
    for(int i = 0; i<numFrames; i++){
        frames.append(bus->readFrame());
    }
    return frames;
}

void SocketCANBus::framesAvailable()
{
    if(bus && bus->state() != QCanBusDevice::ConnectedState) return;
    int numFrames = bus->framesAvailable();
    if(numFrames>0){
        QVector<QCanBusFrame> frames =  readAllFrames(numFrames);
        for(int i = 0; i<frames.length(); i++){
            QCanBusFrame frame = frames.at(i);
            if (callbacks.find(frame.frameId()) != callbacks.end())
            {
                for(auto callback : callbacks[frame.frameId()]){
                    callback(frame.payload());
                }
            }
        }
    }

}


void SocketCANBus::registerFrameHandler(int id, std::function<void(QByteArray)> callback)
{
    callbacks[id].push_back(callback);
    QCanBusDevice::Filter filter;
    filter.frameId = id;
    filter.frameIdMask = 0xFFF;
    filter.format=QCanBusDevice::Filter::MatchBaseAndExtendedFormat;
    filter.type = QCanBusFrame::InvalidFrame;
    filterList.append(filter);
    bus->setConfigurationParameter(QCanBusDevice::RawFilterKey, QVariant::fromValue(filterList));
}
