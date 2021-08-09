#pragma once

#include <functional>
#include <string>
#include <QCanBusFrame>
#include "obd/message.hpp"

typedef QPair<QString, QString> units_t;
struct font_size_t {
    int label;
    int value;
    int unit;
};

typedef std::function<double(double, bool)> unit_converter_t;
typedef std::function<double(Response)> decoder_t;

struct Command {
    QString id;
    QString description;
    units_t units;
    font_size_t font_size;
    int precision;
    unit_converter_t converter;
    QCanBusFrame frame;
    decoder_t decoder;
};


typedef Command Commands[6];
// struct Commands {
//     Command LOAD;
//     Command COOLANT_TEMP;
//     Command RPM;
//     Command SPEED;
//     Command INTAKE_TEMP;
//     Command MAF;
// };

extern Commands cmds;
