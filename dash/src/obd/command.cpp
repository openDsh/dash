#include "obd/command.hpp"
#include "obd/decoders.hpp"

Commands commands = {
    { "Calculated Engine Load", Request(0x01, 0x04), percentage },
    { "Engine Coolant Temperature", Request(0x01, 0x05), temp },
    { "Engine Revolutions Per Minute (RPM)", Request(0x01, 0x0C), rpm },
    { "Vehicle Speed", Request(0x01, 0x0D), speed },
    { "Intake Air Temperature", Request(0x01, 0x0F), temp },
    { "Mass Air Flow (MAF) Rate", Request(0x01, 0x10), flow }
};
