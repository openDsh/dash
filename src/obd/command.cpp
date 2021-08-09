#include "obd/command.hpp"
#include "obd/decoders.hpp"

Commands cmds = {
  {"load", QCanBusFrame(0x7df, QByteArray::fromHex("0201040000000000")), percentage, NULL, 200},
  {"coolant_temp", QCanBusFrame(0x7df, QByteArray::fromHex("0201050000000000")), temp, NULL, 5000},
  {"rpm", QCanBusFrame(0x7df, QByteArray::fromHex("02010C0000000000")), rpm, NULL, 200},
  {"speed", QCanBusFrame(0x7df, QByteArray::fromHex("02010D0000000000")), speed, NULL, 200},
  {"intake_temp", QCanBusFrame(0x7df, QByteArray::fromHex("02010F0000000000")), temp, NULL, 1000},
  {"maf_rate", QCanBusFrame(0x7df, QByteArray::fromHex("0201100000000000")), flow, NULL, 1000}
};
