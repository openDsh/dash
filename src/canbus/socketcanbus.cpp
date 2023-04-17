#include <QTcpSocket>
#include <QStringList>
#include <QString>
#include "canbus/socketcanbus.hpp"
#include <QAbstractSocket>

SocketCANBus::SocketCANBus(QObject *parent, QString canInterface)
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

        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
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
    static SocketCANBus bus(Config::get_instance()->get_vehicle_interface());
    return &bus;
}

void SocketCANBus::readyRead()
{

    while (this->socket.canReadLine())
    {

        DASH_LOG(info) << "[SocketCANBus] Comincio a leggere i CANBUS";

        QString linea = QString(this->socket.readLine());
        DASH_LOG(info) << linea.toStdString();
        QStringList part = linea.split(" ");
        QStringList fram = part[1].split("-");

        QStringList hexx;
        int index = 0;

        for (int i = 0; i < fram[1].length(); i++)
        {
            if (i % 2 == 0)
            {
                hexx[index] = fram[1].mid(i - 2, 2);
                index++;
            }
        }

        // MS-CAN

        if (part.at(0) == "RX1")
        {

            // Esempio RX1 0206-008401

            if (fram.at(0) == "0206")
            {
                if (hexx.at(0) == "00")
                {
                    bool ok;

                    uint temprem = hexx[2].toUInt(&ok, 16);

                    if (ok == true && temprem > 64)
                    {
                        if (hexx.at(1) == "81")
                            DASH_LOG(info) << "PREMUTO pulsante in alto a sinistra\r\n";
                        if (hexx.at(1) == "82")
                            DASH_LOG(info) << "PREMUTO Pulsante giù a sinistra\r\n";
                        if (hexx.at(1) == "84")
                            DASH_LOG(info) << "PREMUTO Pulsante manopola sinistra\r\n";
                        if (hexx.at(1) == "91")
                            DASH_LOG(info) << "PREMUTO Pulsante destro in alto (successivo)\r\n";
                        if (hexx.at(1) == "92")
                            DASH_LOG(info) << "PREMUTO Pulsante in basso a destra\r\n";
                    }
                    else
                    {
                        if (hexx.at(1) == "81")
                            DASH_LOG(info) << "pulsante in alto a sinistra\r\n";
                        if (hexx.at(1) == "82")
                            DASH_LOG(info) << "Pulsante giù a sinistra\r\n";
                        if (hexx.at(1) == "84")
                            DASH_LOG(info) << "Pulsante manopola sinistra\r\n";
                        if (hexx.at(1) == "91")
                            DASH_LOG(info) << "Pulsante destro in alto (successivo)\r\n";
                        if (hexx.at(1) == "92")
                            DASH_LOG(info) << "Pulsante in basso a destra\r\n";
                    }
                }
                if (hexx.at(0) == "08")
                {
                    if (hexx.at(1) == "83")
                    {
                        if (hexx.at(2).at(1) == "F")
                        {
                            DASH_LOG(info) << "Manopola sinistra SU\r\n";
                        }
                        else
                        {
                            DASH_LOG(info) << "Manopola sinistra GIU\r\n";
                        }
                    }

                    if (hexx.at(1) == "93")
                    {
                        if (hexx.at(2).at(1) == "F")
                        {
                            DASH_LOG(info) << "Manopola destra (Volume) GIU\r\n";
                        }
                        else
                        {
                            DASH_LOG(info) << "Manopola destra (Volume) SU\r\n";
                        }
                    }
                }
            }

            // Esempio RX1 0450-460706FF

            if (fram.at(0) == "0450")
            {
                bool okk;

                uint luminosita_nuova = hexx[3].toUInt(&okk, 16);
                int valore_ws = (int)(luminosita_nuova / 25.5);

                if (valore_ws != lumws)
                {
                    /*
                    char query[73]; // 71

                    sprintf(query, "/home/gioele/RPi-USB-Brightness/64/lite/Raspi_USB_Backlight_nogui -b %d", valore_ws);

                    if (system(query) == 0)
                    {
                        printf("%s\r\n", query);
                    }
                    else
                    {
                        printf("==== ERRORE LUMINOSITA ====\r\n");
                    }*/

                    DASH_LOG(info) << "NUOVA LUMINOSITA:" << QString::number(valore_ws).toStdString() << "\r\n";

                    lumws = valore_ws;
                }
            }
        }

        // HS-CAN

        if (part.at(0) == "RX2")
        {
        }
    }
}