#pragma once
#include <QCanBus>
#include <functional>
#include <QObject>

class ICANBus : public QObject {
   Q_OBJECT
   public:
      virtual bool writeFrame(QCanBusFrame frame) = 0;
      virtual void registerFrameHandler(int id, std::function<void(QByteArray)> callback) = 0;
      enum VehicleBusType { SocketCAN};
      Q_ENUM(VehicleBusType)
};