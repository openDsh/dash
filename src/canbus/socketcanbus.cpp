#include <QTcpSocket>
#include <QStringList>
#include <QString>
#include "canbus/socketcanbus.hpp"
#include <QAbstractSocket>

SocketCANBus::SocketCANBus(QObject *parent, QString canInterface) : QObject(parent), socket(this)
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

            QString aperturaMS = "CAN USER OPEN CH1 95K2\r\n";
            this->socket.write(aperturaMS.toUtf8());

            //QString aperturaHS = "CAN USER OPEN CH2 500K\r\n";
            //this->socket.write(aperturaHS.toUtf8());

            QString aperturaLS = "CAN USER OPEN CH2 33K3 GMLAN\r\n";
            this->socket.write(aperturaLS.toUtf8());

            QString mascheraMS = "CAN USER MASK CH1 0FFF\r\n";
            this->socket.write(mascheraMS.toUtf8());

            QString filtroCH1_1 = "CAN USER FILTER CH1 1 0450\r\n";
            this->socket.write(filtroCH1_1.toUtf8());

            QString filtroCH1_2 = "CAN USER FILTER CH1 2 04EE\r\n";
            this->socket.write(filtroCH1_2.toUtf8());

            QString filtroCH1_3 = "CAN USER FILTER CH1 3 04E8\r\n";
            this->socket.write(filtroCH1_3.toUtf8());

            QString filtroCH1_4 = "CAN USER FILTER CH1 4 0682\r\n";
            this->socket.write(filtroCH1_4.toUtf8());

            QString filtroCH1_0 = "CAN USER FILTER CH1 0 0206\r\n";
            this->socket.write(filtroCH1_0.toUtf8());

            QString filtroCH1_5 = "CAN USER FILTER CH1 5 06C8\r\n";
            this->socket.write(filtroCH1_5.toUtf8());

            QString filtroCH1_6 = "CAN USER FILTER CH1 6 04EC\r\n";
            this->socket.write(filtroCH1_6.toUtf8());

            QString filtroCH1_7 = "CAN USER FILTER CH1 7 0208\r\n";
            this->socket.write(filtroCH1_7.toUtf8());

            QString filtroCH1_8 = "CAN USER FILTER CH1 8 02B0\r\n";
            this->socket.write(filtroCH1_8.toUtf8());

            //QString filtroTempAntigeloRX2 = "CAN USER FILTER CH2 0 0510\r\n";
            //this->socket.write(filtroTempAntigeloRX2.toUtf8());

            QString mascheraHS = "CAN USER MASK CH2 0FFF\r\n";
            this->socket.write(mascheraHS.toUtf8());

            QString filtroCH2_0 = "CAN USER FILTER CH2 0 0110\r\n";
            this->socket.write(filtroCH2_0.toUtf8());
            
            QString filtroCH2_1 = "CAN USER FILTER CH2 1 0145\r\n";
            this->socket.write(filtroCH2_1.toUtf8());
            
            QString filtroCH2_2 = "CAN USER FILTER CH2 2 0350\r\n";
            this->socket.write(filtroCH2_2.toUtf8());
            
            QString filtroCH2_3 = "CAN USER FILTER CH2 3 0375\r\n";
            this->socket.write(filtroCH2_3.toUtf8());
            
            QString filtroCH2_4 = "CAN USER FILTER CH2 4 0500\r\n";
            this->socket.write(filtroCH2_4.toUtf8());
            
            QString filtroCH2_5 = "CAN USER FILTER CH2 5 0130\r\n";
            this->socket.write(filtroCH2_5.toUtf8());

            DASH_LOG(info) << "[SocketCANBus] Connesso a Carberry";
        }
        else
        {
            DASH_LOG(error) << "[SocketCANBus] Errore di connessione a Carberry";
        }
    }
}

SocketCANBus::~SocketCANBus()
{
    DASH_LOG(info) << "[SocketCANBus] Disconnessione da Carberry";

    this->socket.close();
}

bool SocketCANBus::writeFrame(int ch, QString frame)
{
    if (this->socket.write(QString("CAN USER TX CH%1 %2").arg(ch).arg(frame).toUtf8()) < 0)
        return false;
        
    return true;
}

SocketCANBus *SocketCANBus::get_instance()
{
    static SocketCANBus bus(NULL, Config::get_instance()->get_vehicle_interface());
    return &bus;
}