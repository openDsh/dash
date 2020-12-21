#pragma once

#include <functional>
#include <string>

#include <obd/message.hpp>

struct Command {
    std::string description;
    Request request;
    std::function<double(Response)> decoder;
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
