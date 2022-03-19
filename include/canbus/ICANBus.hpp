#pragma once
#include <QCanBus>
#include <functional>
class ICANBus {
   public:
      virtual bool writeFrame(QCanBusFrame frame) = 0;
      virtual void registerFrameHandler(int id, std::function<void(QByteArray)> callback) = 0;
};