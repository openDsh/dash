#ifndef CONVERSIONS_HPP_
#define CONVERSIONS_HPP_

double kph_to_mph(double val) { return val * 0.621371; }

double c_to_f(double val) { return (val * 9 / 5) + 32; }

double gps_to_gph(double val) { return val * 0.0805; }

double gps_to_lph(double val) { return val * 3.6; }

#endif
