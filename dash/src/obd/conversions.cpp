#include "obd/conversions.hpp"

double kph_to_mph(double value) { return value * 0.621371; }

double c_to_f(double value) { return (value * 9 / 5) + 32; }

double gps_to_gph(double value) { return value * 0.0805; }

double gps_to_lph(double value) { return value * 3.6; }
