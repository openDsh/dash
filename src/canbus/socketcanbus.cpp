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
        
        QStringList hexx;
        int index = 0;
        
        for (int i = 0; i < fram[1].length(); i++) {
          if(i%2==0){
            hexx[index] = 
          }
          
        }
        
        QStringList hexx = fram[1].split(QRegularExpression("[\\s|,]"), QString::SkipEmptyParts);
        
        if(part.at(0)=="RX1"){
          if(fram.at(0)=="0206"){
            
          }
        }
        if(part.at(0)=="RX2"){
          
        }
    }
    
    
    }

    while (this->socket.canReadLine())
    {
        QString data = QString(this->socket.readLine());
        DASH_LOG(info) << data.toStdString();
        
        QStringList list = data.split(" ");

        if(list[0].compare())
    }
}

SocketCANBus::~SocketCANBus()
{
    DASH_LOG(info) << "[SocketCANBus] Disconnessione da Carberry";

    this->socket.close();
}

bool SocketCANBus::writeFrame(QString frame)
{
    if(this->socket.write(frame.toUtf8())<0)
        return false;
    return true;
}

SocketCANBus *SocketCANBus::get_instance()
{
    static SocketCANBus bus(Config::get_instance()->get_vehicle_interface());
    return &bus;
}
