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

            QString aperturaHS = "CAN USER OPEN CH2 500K\r\n";
            this->socket.write(aperturaHS.toUtf8());

            QString mascheraMS = "CAN USER MASK CH1 0FFF\r\n";
            this->socket.write(mascheraMS.toUtf8());

            QString mascheraHS = "CAN USER MASK CH2 0FFF\r\n";
            this->socket.write(mascheraHS.toUtf8());

            QString filtroPulsantiVolante = "CAN USER FILTER CH1 0 0206\r\n";
            this->socket.write(filtroPulsantiVolante.toUtf8());

            QString filtroLuminosita = "CAN USER FILTER CH1 1 0450\r\n";
            this->socket.write(filtroLuminosita.toUtf8());

            QString filtroAutonomia = "CAN USER FILTER CH1 2 04EE\r\n";
            this->socket.write(filtroAutonomia.toUtf8());

            QString filtroRPMeVel = "CAN USER FILTER CH1 3 04E8\r\n";
            this->socket.write(filtroRPMeVel.toUtf8());

            QString filtroTemp = "CAN USER FILTER CH1 4 0682\r\n";
            this->socket.write(filtroTemp.toUtf8());

            QString filtroTempAntigelo = "CAN USER FILTER CH2 0 0510\r\n";
            this->socket.write(filtroTempAntigelo.toUtf8());

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

bool SocketCANBus::writeFrame(QString frame)
{
    if (this->socket.write(frame.toUtf8()) < 0)
        return false;
    return true;
}

SocketCANBus *SocketCANBus::get_instance()
{
    static SocketCANBus bus(NULL, Config::get_instance()->get_vehicle_interface());
    return &bus;
}