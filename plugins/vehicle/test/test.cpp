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
    if (this->obd)
        delete this->obd;
}

QList<QWidget *> Test::widgets()
{
    QList<QWidget *> tabs;
    if (this->vehicle)
        tabs.append(this->vehicle);
    if (this->obd)
        //tabs.append(this->obd);
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
        this->climate->max_fan_speed(7);
        /*

                    static bool toggle = false;

                    switch(rand() % 50) {
                        case 0:
                            this->climate->airflow(Airflow::OFF);
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
                    }
        */
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
        // DASH_LOG(info) << "[PLUGIN] CAN-BUS = " << linea.toStdString() << "\n";

        if (linea.contains(" ") && linea.contains("-")) // checks to avoid a split crash
        {

            QString canbus = linea.split(" ")[0];
            QString id = linea.split(" ")[1].split("-")[0];
            QString data = linea.split(" ")[1].split("-")[1];

            uint8_t canMsg[8];

            QString duo;

            int j = 0;
            bool ok;
            for (int i = 0; i < data.length(); i++)
            {
                if (i % 2 == 0 && i != 0)
                {
                    canMsg[j] = duo.toUInt(&ok, 16);
                    duo = "";
                    j++;
                }
                duo.append(data[i]);
            }

            // MS-CAN

            if (canbus == "RX1")
            {

                // Esempio RX1 04E8-460F0D18000001

                if (id == "04E8")
                {
                    int rpm = (int)(((canMsg[2] << 8) + canMsg[3]) >> 2);                        // tachometer=((2_byte<<8)+3_byte)>>2
                    int velocita = (int)(((canMsg[4] << 1) & 0xFE) + ((canMsg[5] >> 7) & 0x01)); // speed=((5_bytes<<1)&0xFE)+((6_bytes>>7)&0x01)

                    if (velocita != velsalvata)
                    {
                        this->arbiter->vehicle_update_data("speed", velocita);
                        velsalvata = velocita;
                    }
                    if (rpm != rpmsalvati)
                    {
                        this->arbiter->vehicle_update_data("rpm", rpm);
                        rpmsalvati = rpm;
                    }

                    // DASH_LOG(info) << "VELOCITA:" << QString::number(velocita).toStdString() << "\r\n";
                    // DASH_LOG(info) << "RPM:" << QString::number(rpm).toStdString() << "\r\n";
                }

                // Esempio RX1 0206-008401

                if (id == "0206")
                {

                    if (canMsg[0] == 0x01 && canMsg[2] > 4 && premuto == false)
                    {
                        premuto = true;

                        switch (canMsg[1])
                        {
                        case 0x81:
                            this->arbiter->set_curr_page(0);
                            // DASH_LOG(info) << "PREMUTO pulsante in alto a sinistra\r\n";
                            break;
                        case 0x82:
                            this->arbiter->set_curr_page(3);
                            // DASH_LOG(info) << "PREMUTO Pulsante giù a sinistra\r\n";
                            break;
                        case 0x84:
                            // non usare temp
                            DASH_LOG(info) << "PREMUTO Pulsante manopola sinistra\r\n";
                            break;
                        case 0x91:
                            this->arbiter->android_auto().handler->injectButtonPressHelper(aasdk::proto::enums::ButtonCode::MICROPHONE_1, Action::ActionState::Triggered);
                            // DASH_LOG(info) << "PREMUTO Pulsante destro in alto (successivo)\r\n";
                            break;
                        case 0x92:
                            this->arbiter->android_auto().handler->injectButtonPressHelper(aasdk::proto::enums::ButtonCode::TOGGLE_PLAY, Action::ActionState::Triggered);
                            // DASH_LOG(info) << "PREMUTO Pulsante in basso a destra\r\n";
                            break;
                        }
                    }

                    if (canMsg[0] == 0x00)
                    {
                        if (premuto == false)
                        {
                            switch (canMsg[1])
                            {
                            case 0x81:
                                // DASH_LOG(info) << "pulsante in alto a sinistra\r\n";
                                this->arbiter->android_auto().handler->injectButtonPressHelper(aasdk::proto::enums::ButtonCode::ENTER, Action::ActionState::Triggered);
                                break;
                            case 0x82: // DASH_LOG(info) << "Pulsante giù a sinistra\r\n";
                                this->arbiter->set_curr_page(this->arbiter->layout().next_enabled_page(this->arbiter->layout().curr_page));
                                break;
                            case 0x84:
                                // non usare temp
                                DASH_LOG(info) << "Pulsante manopola sinistra\r\n";
                                break;
                            case 0x91:
                                this->arbiter->android_auto().handler->injectButtonPressHelper(aasdk::proto::enums::ButtonCode::NEXT, Action::ActionState::Triggered);
                                // DASH_LOG(info) << "Pulsante destro in alto (successivo)\r\n";
                                break;
                            case 0x92:
                                this->arbiter->android_auto().handler->injectButtonPressHelper(aasdk::proto::enums::ButtonCode::PREV, Action::ActionState::Triggered);
                                // DASH_LOG(info) << "Pulsante in basso a destra\r\n";
                                break;
                            }
                        }
                        else
                        {
                            premuto = false;
                        }
                    }

                    if (canMsg[0] == 0x08)
                    {
                        if (canMsg[1] == 0x83)
                        {
                            if (canMsg[2] == 0xFF)
                            {
                                this->arbiter->android_auto().handler->injectButtonPress(aasdk::proto::enums::ButtonCode::SCROLL_WHEEL, openauto::projection::WheelDirection::RIGHT);
                                // DASH_LOG(info) << "Manopola sinistra SU\r\n";
                            }
                            else
                            {
                                this->arbiter->android_auto().handler->injectButtonPress(aasdk::proto::enums::ButtonCode::SCROLL_WHEEL, openauto::projection::WheelDirection::LEFT);
                                // DASH_LOG(info) << "Manopola sinistra GIU\r\n";
                            }
                        }

                        if (canMsg[1] == 0x93)
                        {
                            if (canMsg[2] == 0xFF)
                            {
                                // DASH_LOG(info) << "Manopola destra (Volume) GIU\r\n";
                                this->arbiter->decrease_volume(10);
                            }
                            else
                            {
                                // DASH_LOG(info) << "Manopola destra (Volume) SU\r\n";
                                this->arbiter->increase_volume(10);
                            }
                        }
                    }
                }

                // Esempio RX1 0450-460706FF

                if (id == "0450")
                {
                    int valore_ws = (int)(canMsg[3] / 25.5);

                    if (valore_ws != lumws)
                    {
                        // DASH_LOG(info) << "NUOVA LUMINOSITA:" << QString::number(valore_ws).toStdString() << "\r\n";
                        this->arbiter->set_brightness(canMsg[3]);
                        lumws = valore_ws;
                    }
                }

                // Esempio RX1 04EE-46030130

                if (id == "04EE")
                {
                    int valore_km = 0;

                    if (canMsg[2] == 0x10)
                    {
                        valore_km = canMsg[3] / 2;
                    }
                    else
                    {
                        valore_km = (int)(((canMsg[2] << 8) + canMsg[3]) / 2);
                    }

                    if (valore_km != kmsalvati)
                    {
                        // DASH_LOG(info) << "NUOVA AUTONOMIA:" << QString::number(valore_km).toStdString() << "\r\n";
                        this->arbiter->vehicle_update_data("autonomia", valore_km);
                        kmsalvati = valore_km;
                    }
                }

                // Esempio RX1 0682-46017A

                if (id == "0682")
                {
                    double temp = ((double)canMsg[2]) / 2 - 40;

                    if (temp != tempsalvata)
                    {
                        // DASH_LOG(info) << "Temperatura esterna:" << QString::number(temp).toStdString() << "\r\n";
                        this->arbiter->vehicle_update_data("ext_temp", temp);
                        tempsalvata = temp;
                    }
                }

                // Esempio RX1 04EC-46075A2C00

                if (id == "04EC")
                {

                    int tempCoolant = ((int)canMsg[2]) - 40;

                    if (ttCoolRX1 != tempCoolant)
                    {
                        // DASH_LOG(info) << "Antigelo:" << QString::number(tempCoolant).toStdString() << "\r\n";
                        this->arbiter->vehicle_update_data("coolant_temp", tempCoolant);
                        ttCoolRX1 = tempCoolant;
                    }
                }

                // Esempio RX1 06C8-21004800692401E0

                if (id == "06C8")
                {
                    switch (canMsg[0]) // mode?
                    {
                    case 0x21: // normal mode, change flow direction
                        if (canMsg[1] == 0xE0)
                        {
                            switch (canMsg[2])
                            {
                            case 0x52: // all:
                                this->climate->airflow(Airflow::DEFROST | Airflow::BODY | Airflow::FEET);
                                break;
                            case 0x53: // up:
                                this->climate->airflow(Airflow::DEFROST);
                                break;
                            case 0x54: // up_middle:
                                this->climate->airflow(Airflow::DEFROST | Airflow::BODY);
                                break;
                            case 0x55: // middle:
                                this->climate->airflow(Airflow::BODY);
                                break;
                            case 0x56: // middle_down:
                                this->climate->airflow(Airflow::BODY | Airflow::FEET);
                                break;
                            case 0x57: // down:
                                this->climate->airflow(Airflow::FEET);
                                break;
                            case 0x58: // up_down:
                                this->climate->airflow(Airflow::DEFROST | Airflow::FEET);
                                break;
                            case 0x59: // dir_auto:
                                DASH_LOG(info) << "AC AUTO \r\n";
                                break;
                            }
                        }
                        break;

                    case 0x22: // normal mode, change flow speed or temperature
                        switch (canMsg[1])
                        {
                        case 0x03:
                            tempAC = 10 * (canMsg[3] - 0x30) + (canMsg[5] - 0x30);
                            this->climate->left_temp(tempAC);
                            this->climate->right_temp(tempAC);
                            break;

                        case 0x50: // fan set. canMsg[3] = canMsg[4] = ascii
                            fanAC = canMsg[3] - 0x30;
                            this->climate->fan_speed(fanAC);
                            break;
                        }
                        break;

                    case 0x24: // normal mode, auto flow? status 4 is speed, 30 - 37
                        /*fanAC = canMsg[3] - 0x30;
                        DASH_LOG(info) << "VENTOLE:" << QString::number(fanAC).toStdString() << "\r\n";
                        this->climate->fan_speed(fanAC);*/
                        break;

                    case 0x25: // normal mode, auto flow speed, status
                        // 4 is E0 = full auto speed, 41 = manual flow direction
                        break;

                    case 0x26: // air distribution mode,
                        // [7] is flow direction , 52 - 59
                        break;
                    }
                }
            }

            // HS-CAN

            if (canbus == "RX2")
            {

                // Esempio RX2 0510-0442432E

                if (id == "0510")
                {

                    int tempCoolant = ((int)canMsg[1]) - 40;

                    if (ttCoolRX2 != tempCoolant)
                    {
                        // DASH_LOG(info) << "Antigelo:" << QString::number(tempCoolant).toStdString() << "\r\n";
                        // this->arbiter->vehicle_update_data("coolant_temp", tempCoolant);
                        ttCoolRX2 = tempCoolant;
                    }
                }
            }
        }
    }
}
