#include "obd/command.hpp"
#include "obd/decoders.hpp"
#include "obd/conversions.hpp"

Commands cmds = {
  {"load", "Calculated Engine Load", {"%", "%"}, 
    {10, 16, 12}, 1, [](double x, bool _) { return x; }, 
    QCanBusFrame(0x7df, QByteArray::fromHex("0201040000000000")), percentage
  },
  {"coolant_temp", "Engine Coolant Temperature", {"°F", "°C"}, 
    {10, 16, 12}, 1, [](double x, bool si) { return si ? x : c_to_f(x); },
    QCanBusFrame(0x7df, QByteArray::fromHex("0201050000000000")), temp
  },
  {"rpm", "Engine Revolutions Per Minute (RPM)", {"x1000rpm", "x1000rpm"}, 
    {0, 24, 12}, 1, [](double x, bool _) { return x / 1000.0; },
    QCanBusFrame(0x7df, QByteArray::fromHex("02010C0000000000")), rpm
  },
  {"speed", "Vehicle Speed", {"mph", "km/h"}, 
    {0, 36, 16}, 0, [](double x, bool si) { return si ? x : kph_to_mph(x); },
    QCanBusFrame(0x7df, QByteArray::fromHex("02010D0000000000")), speed
  },
  {"intake_temp", "Intake Air Temperature", {"°F", "°C"}, 
    {10, 16, 12}, 1, [](double x, bool si) { return si ? x : c_to_f(x); },
    QCanBusFrame(0x7df, QByteArray::fromHex("02010F0000000000")), temp
  },
  {"maf_rate", "Mass Air Flow (MAF) Rate", {"g/s", "g/s"},  
    {10, 16, 12}, 1, [](double x, bool si) { return x; },
    QCanBusFrame(0x7df, QByteArray::fromHex("0201100000000000")), flow
  }
};
