#include <QTcpSocket>
#include <QStringList>
#include <QString>
#include "canbus/socketcanbus.hpp"

SocketCANBus::SocketCANBus(QString canInterface)
{
    if (QCanBus::instance()->plugins().contains(QStringLiteral("socketcan")))
    {
        DASH_LOG(info) << "[SocketCANBus] 'socketcan' Available";
        socketCANAvailable = true;

        this->socket.connectToHost("127.0.0.1", 7070);
        this->socket.waitForConnected();

        if (this->socket.state() == QAbstractSocket::ConnectedState)
        {
            QString allineamento = "CAN USER ALIGN RIGHT\r\n";
            this->socket.write(allineamento.toUtf8());

            QString apertura = "CAN USER OPEN CH1 95K2\r\n";
            this->socket.write(apertura.toUtf8());
            
            QString maschera = "CAN USER MASK CH1 0FFF\r\n";
            this->socket.write(maschera.toUtf8());

            QString filtro10 = "CAN USER FILTER CH1 0 0206\r\n";
            this->socket.write(filtro10.toUtf8());

            QString filtro11 = "CAN USER FILTER CH1 1 0450\r\n";
            this->socket.write(filtro11.toUtf8());

            DASH_LOG(info) << "[SocketCANBus] Connesso a Carberry";
        }
        else
        {
            DASH_LOG(error) << "[SocketCANBus] Errore di connessione a Carberry";
        }
        
    while (this->socket.canReadLine())
    {
        QString linea = QString(this->socket.readLine());
        DASH_LOG(info) << linea.toStdString();
        QStringList part = linea.split(" ");
        QStringList fram = part[1].split("-");
        
        if(part.at(0)=="RX1"){
          if(fram.at(0)=="0206"){
            
          }
        }
        if(part.at(0)=="RX2"){
          
        }
    }
    
    
    }
}

SocketCANBus::~SocketCANBus()
{
    DASH_LOG(info) << "[SocketCANBus] Disconnessione da Carberry";

    this->socket.close();
}

bool SocketCANBus::writeFrame(QCanBusFrame frame)
{
    //if(this->socket.write("p")<0) //maggiore di frame.lenght
        //return false;
    return true;
}

SocketCANBus *SocketCANBus::get_instance()
{
    static SocketCANBus bus(Config::get_instance()->get_vehicle_interface());
    return &bus;
}

void SocketCANBus::registerFrameHandler(int id, std::function<void(QByteArray)> callback)
{

    QStringList list;
    while (this->socket.canReadLine())
    {
        QString data = QString(this->socket.readLine());
        DASH_LOG(info) << data.toStdString();
        list.append(data);
    }

    /*
        callbacks[id].push_back(callback);
        QCanBusDevice::Filter filter;
        filter.frameId = id;
        filter.frameIdMask = 0xFFF;
        filter.format = QCanBusDevice::Filter::MatchBaseAndExtendedFormat;
        filter.type = QCanBusFrame::InvalidFrame;
        filterList.append(filter);
        bus->setConfigurationParameter(QCanBusDevice::RawFilterKey, QVariant::fromValue(filterList));
    */
}
