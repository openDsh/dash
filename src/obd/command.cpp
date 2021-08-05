#include "obd/command.hpp"
#include "obd/decoders.hpp"

Commands cmds = {{"Torque", QCanBusFrame(0xa8, QByteArray::fromHex("0201040000000000")), torque},
                 {"Engine Coolant Temperature", QCanBusFrame(0x1d0, QByteArray::fromHex("0201050000000000")), temp},
                 {"Engine Revolutions Per Minute (RPM)", QCanBusFrame(0x0a5, QByteArray::fromHex("02010C0000000000")), rpm},
                 {"Vehicle Speed", QCanBusFrame(0x1a1, QByteArray::fromHex("02010D0000000000")), speed},
                 {"Intake Air Temperature", QCanBusFrame(0x7df, QByteArray::fromHex("02010F0000000000")), temp},
                 {"Mass Air Flow (MAF) Rate", QCanBusFrame(0x7df, QByteArray::fromHex("0201100000000000")), flow}};
