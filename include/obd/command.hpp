#pragma once

#include <functional>
#include <string>

#include <QTimer>
#include <QCanBusFrame>
#include "obd/message.hpp"

typedef std::function<double(Response)> decoder_t;

struct Command {
    QString id;
    QCanBusFrame frame;
    decoder_t decoder;
    QTimer *timer;
    int rate;
};

typedef QList<Command> Commands;
// struct Commands {
//     Command LOAD;
//     Command COOLANT_TEMP;
//     Command RPM;
//     Command SPEED;
//     Command INTAKE_TEMP;
//     Command MAF;
// };

extern Commands cmds;
