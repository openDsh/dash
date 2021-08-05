#pragma once

#include <obd/message.hpp>

double percentage(Response resp) { return (100.0 / 255.0) * (int)resp.data.at(0); }

double temp(Response resp) { return (int)resp.data.at(0) - 48; }

double torque(Response resp) { return ((256.0 * (int)resp.data.at(2)) + (int)resp.data.at(1)) / 32.0; }

double rpm(Response resp) { return ((256.0 * (int)resp.data.at(6)) + (int)resp.data.at(5)) / 4.0; }

double speed(Response resp) { return ((256.0 * (int)resp.data.at(3)) + (int)resp.data.at(2)) / 100.0; }

double flow(Response resp) { return ((256.0 * (int)resp.data.at(0)) + (int)resp.data.at(1)) / 100.0; }

