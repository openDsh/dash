#pragma once
#include <QCanBus>
#include <functional>
#include <QObject>

class ICANBus : public QObject {
   Q_OBJECT
   public:
      enum VehicleBusType { SocketCAN};
      Q_ENUM(VehicleBusType)
};