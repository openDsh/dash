#pragma once

#include <string>
#include <QCanBusFrame>
#include <QBluetoothSocket>
#include <sys/socket.h>
#include <QVector>
#include <QByteArray>
#include <QVariant>
#include <QTimer>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <mutex>
#include <string>
#include <iomanip>



#include "DashLog.hpp"
#include "canbus/ICANBus.hpp"
#include "app/config.hpp"

class elm327 : public ICANBus
{
    Q_OBJECT
    public:
        elm327(QString canInterface = "/dev/pts/1", bool bluetooth=false);
        ~elm327();
        static elm327 *get_usb_instance();
        static elm327 *get_bt_instance();
        void registerFrameHandler(int id, std::function<void(QByteArray)> callback) override;
        bool writeFrame(QCanBusFrame frame) override;
        void connect(QString dev_path, speed_t baudrate);
        void initialize();
        inline bool is_connected() { return this->connected; }
        enum OBDType {USB, BT};
        inline OBDType get_adapter_type(){return this->adapterType;}
    private:
        bool connected; 
        OBDType adapterType;
        QBluetoothSocket* btSocket;
        int fd;
        std::mutex elm_mutex;
        std::map<int, std::vector<std::function<void(QByteArray)>>> callbacks;

        void connect(std::string dev_path, speed_t baudrate);
        void handleFrame(QCanBusFrame frame);
        std::string _read();
        QCanBusFrame receive();
        int _write(std::string str);
        bool is_failed_response(std::string str);
        inline std::string raw_query(std::string cmd) { return (this->_write(cmd) > 0) ? this->_read() : ""; }

    public slots:
        void btConnected();
        void socketChanged(QBluetoothSocket::SocketState state);
       
};

