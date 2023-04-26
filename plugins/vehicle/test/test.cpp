#include <QDebug>
#include <QTimer>
#include <QString>
#include <QStringList>

#include "test.hpp"

Test::~Test()
{
    if (this->climate)
        delete this->climate;
    if (this->vehicle)
        delete this->vehicle;
}

QList<QWidget *> Test::widgets()
{
    QList<QWidget *> tabs;
    if (this->vehicle)
        tabs.append(this->vehicle);
    return tabs;
}

bool Test::init(SocketCANBus *bus)
{
    if (this->arbiter)
    {
        this->vehicle = new Vehicle(*this->arbiter);
        this->vehicle->pressure_init("psi", 35);
        this->vehicle->disable_sensors();
        this->vehicle->rotate(270);

        this->climate = new Climate(*this->arbiter);
        this->climate->max_fan_speed(4);

        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]
                {
            static bool toggle = false;

            switch(rand() % 50) {
                case 0:
                    this->climate->airflow(Airflow::OFF);
                    break;
                case 1:
                    this->climate->airflow(Airflow::DEFROST);
                    break;
                case 2:
                    this->climate->airflow(Airflow::BODY);
                    break;
                case 3:
                    this->climate->airflow(Airflow::FEET);
                    break;
                case 4:
                    this->climate->airflow(Airflow::DEFROST | Airflow::BODY);
                    break;
                case 5:
                    this->climate->airflow(Airflow::DEFROST | Airflow::FEET);
                    break;
                case 6:
                    this->climate->airflow(Airflow::BODY | Airflow::FEET);
                    break;
                case 7:
                    this->climate->airflow(Airflow::DEFROST | Airflow::BODY | Airflow::FEET);
                    break;
                case 8:
                    this->climate->fan_speed((rand() % 4) + 1);
                    break;
                case 9:
                    this->vehicle->sensor(Position::FRONT_LEFT, rand() % 5);
                    break;
                case 10:
                    this->vehicle->sensor(Position::FRONT_MIDDLE_LEFT, rand() % 5);
                    break;
                case 11:
                    this->vehicle->sensor(Position::FRONT_MIDDLE_RIGHT, rand() % 5);
                    break;
                case 12:
                    this->vehicle->sensor(Position::FRONT_RIGHT, rand() % 5);
                    break;
                case 13:
                    this->vehicle->sensor(Position::BACK_LEFT, rand() % 5);
                    break;
                case 14:
                    this->vehicle->sensor(Position::BACK_MIDDLE_LEFT, rand() % 5);
                    break;
                case 15:
                    this->vehicle->sensor(Position::BACK_MIDDLE_RIGHT, rand() % 5);
                    break;
                case 16:
                    this->vehicle->sensor(Position::BACK_RIGHT, rand() % 5);
                    break;
                case 17:
                    this->vehicle->door(Position::FRONT_LEFT, toggle);
                    break;
                case 18:
                    this->vehicle->door(Position::BACK_LEFT, toggle);
                    break;
                case 19:
                    this->vehicle->door(Position::FRONT_RIGHT, toggle);
                    break;
                case 20:
                    this->vehicle->door(Position::BACK_RIGHT, toggle);
                    break;
                case 21:
                    this->vehicle->headlights(toggle);
                    break;
                case 22:
                    this->vehicle->taillights(toggle);
                    break;
                case 23:
                    this->vehicle->pressure(Position::BACK_RIGHT, (rand() % 21) + 30);
                    break;
                case 24:
                    this->vehicle->pressure(Position::BACK_LEFT, (rand() % 21) + 30);
                    break;
                case 25:
                    this->vehicle->pressure(Position::FRONT_RIGHT, (rand() % 21) + 30);
                    break;
                case 26:
                    this->vehicle->pressure(Position::FRONT_LEFT, (rand() % 21) + 30);
                    break;
                case 27:
                    this->vehicle->wheel_steer((rand() % 10) * ((rand() % 2) ? 1 : -1));
                    break;
                case 28:
                    this->vehicle->indicators(Position::LEFT, toggle);
                    break;
                case 29:
                    this->vehicle->indicators(Position::RIGHT, toggle);
                    break;
                case 30:
                    this->vehicle->hazards(toggle);
                    break;
                default:
                    toggle = !toggle;
                    break;
            } });
        timer->start(1000);

        auto timer2 = new QTimer(this);
        connect(timer2, &QTimer::timeout, [this]
                {
            if (rand() % 10 == 1) {
                this->climate->left_temp((rand() % 20) + 60);
                this->climate->right_temp((rand() % 20) + 60);
            } });
        timer2->start(100000);

        socketcan(bus);
        QObject::connect(&bus->socket, &QTcpSocket::readyRead, this, &Test::readFrame);

        return true;
    }

    return false;
}

void Test::readFrame()
{

    while (bus->socket.canReadLine())
    {

        QString linea = QString(bus->socket.readLine());
        DASH_LOG(info) << "[PLUGIN] CAN-BUS = " << linea.toStdString() << "\n";

        if (linea.contains(" ") && linea.contains("-")) // checks to avoid a split crash
        {

            QString canbus = linea.split(" ")[0];
            QString id = linea.split(" ")[1].split("-")[0];
            QString data = linea.split(" ")[1].split("-")[1];

            QStringList dataHex;

            QString duo;

            for (int i = 0; i < data.length(); i++)
            {
                if (i % 2 == 0 && i != 0)
                {
                    dataHex.append(duo);
                    duo = "";
                }
                duo.append(data[i]);
            }

            // MS-CAN

            if (canbus == "RX1")
            {

                // Esempio RX1 0206-008401

                if (id == "0206")
                {
                    if (dataHex.at(0) == "00")
                    {
                        bool ok;

                        uint temprem = dataHex[2].toUInt(&ok, 16);

                        if (ok == true && temprem > 64)
                        {
                            if (dataHex.at(1) == "81")
                            {
                                DASH_LOG(info) << "PREMUTO pulsante in alto a sinistra\r\n";
                            }
                            if (dataHex.at(1) == "82")
                            {
                                DASH_LOG(info) << "PREMUTO Pulsante giù a sinistra\r\n";
                            }
                            if (dataHex.at(1) == "84")
                            {
                                DASH_LOG(info) << "PREMUTO Pulsante manopola sinistra\r\n";
                            }
                            if (dataHex.at(1) == "91")
                            {
                                DASH_LOG(info) << "PREMUTO Pulsante destro in alto (successivo)\r\n";
                            }
                            if (dataHex.at(1) == "92")
                            {
                                DASH_LOG(info) << "PREMUTO Pulsante in basso a destra\r\n";
                            }
                        }
                        else
                        {
                            if (dataHex.at(1) == "81")
                            {
                                DASH_LOG(info) << "pulsante in alto a sinistra\r\n";
                                this->arbiter->set_curr_page(1);
                            }
                            if (dataHex.at(1) == "82")
                            {
                                DASH_LOG(info) << "Pulsante giù a sinistra\r\n";
                                // this->arbiter->vehicle_update_data("mpg", 18.5);
                            }
                            if (dataHex.at(1) == "84")
                            {
                                DASH_LOG(info) << "Pulsante manopola sinistra\r\n";
                            }
                            if (dataHex.at(1) == "91")
                            {
                                DASH_LOG(info) << "Pulsante destro in alto (successivo)\r\n";
                            }
                            if (dataHex.at(1) == "92")
                            {
                                DASH_LOG(info) << "Pulsante in basso a destra\r\n";
                            }
                        }
                    }
                    if (dataHex.at(0) == "08")
                    {
                        if (dataHex.at(1) == "83")
                        {
                            if (dataHex.at(2).at(1) == "F")
                            {
                                DASH_LOG(info) << "Manopola sinistra SU\r\n";
                            }
                            else
                            {
                                DASH_LOG(info) << "Manopola sinistra GIU\r\n";
                            }
                        }

                        if (dataHex.at(1) == "93")
                        {
                            if (dataHex.at(2).at(1) == "F")
                            {
                                DASH_LOG(info) << "Manopola destra (Volume) GIU\r\n";
                                this->arbiter->decrease_volume(10);
                            }
                            else
                            {
                                DASH_LOG(info) << "Manopola destra (Volume) SU\r\n";
                                this->arbiter->increase_volume(10);
                            }
                        }
                    }
                }

                // Esempio RX1 0450-460706FF

                if (id == "0450")
                {
                    bool okk;

                    uint luminosita_nuova = dataHex[3].toUInt(&okk, 16);
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
                        this->arbiter->set_brightness(luminosita_nuova);
                        

                        lumws = valore_ws;
                    }
                }
            }

            // HS-CAN

            if (canbus == "RX2")
            {
                // this->arbiter->vehicle_update_data("mpg", 18.5);
            }
        }
    }
}
