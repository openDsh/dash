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

struct Command {
    QString id;
    QString description;
    units_t units;
    font_size_t font_size;
    int precision;
    std::function<double(double x, bool si)> decoder;
};

struct Commands {
    Command LOAD;
    Command COOLANT_TEMP;
    Command RPM;
    Command SPEED;
    Command INTAKE_TEMP;
    Command MAF;
};

extern Commands cmds;
