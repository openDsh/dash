#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include <functional>
#include <string>

#include "obd/message.hpp"

template <typename T>
struct Command {
    std::string description;
    Request request;
    std::function<T(Response)> decoder;
};

struct Commands {
    Command<double> LOAD;
    Command<double> COOLANT_TEMP;
    Command<double> RPM;
    Command<double> SPEED;
    Command<double> INTAKE_TEMP;
    Command<double> MAF;
};

extern Commands commands;

#endif
