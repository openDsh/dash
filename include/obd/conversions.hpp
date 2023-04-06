#pragma once

class Conversion
{
public:
    static double kph_to_mph(double val) { return val * 0.621371; };

    static double c_to_f(double val) { return (val * 9 / 5) + 32; };

    static double gps_to_gph(double val) { return val * 0.0805; };

    static double gps_to_lph(double val) { return val * 3.6; };

    static double l100km_to_mpg(double val) { return (val == 0 ? 0 : (235.214583 / val)); };
};