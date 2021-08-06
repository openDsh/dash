#pragma once

#include <functional>
#include <string>
#include <QCanBusFrame>
#include "obd/message.hpp"

typedef QPair<QString, QString> units_t;

struct Command {
    QString id;
    std::string description;
    units_t units;
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
