#include "obd/command.hpp"
#include "obd/decoders.hpp"
#include "obd/conversions.hpp"

Commands cmds = {{"load", "Calculated Engine Load", {"%", "%"}, {10, 16, 12}, 1, [](double x, bool _) { return x; }},
                 {"coolant_temp", "Engine Coolant Temperature", {"°F", "°C"}, {10, 16, 12}, 1, [](double x, bool si) { return si ? x : c_to_f(x); }},
                 {"rpm", "Engine Revolutions Per Minute (RPM)", {"x1000rpm", "x1000rpm"}, {0, 24, 12}, 1, [](double x, bool _) { return x / 1000.0; }},
                 {"speed", "Vehicle Speed", {"mph", "km/h"},  {0, 36, 16}, 0, [](double x, bool si) { return si ? x : kph_to_mph(x); }},
                 {"intake_temp", "Intake Air Temperature", {"°F", "°C"},  {10, 16, 12}, 1, [](double x, bool si) { return si ? x : c_to_f(x); }},
                 {"maf_rate", "Mass Air Flow (MAF) Rate", {"g/s", "g/s"},  {10, 16, 12}, 1, [](double x, bool si) { return x; }}};
