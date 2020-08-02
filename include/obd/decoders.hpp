#pragma once

#include <obd/message.hpp>

double percentage(Response resp) { return (100.0 / 255.0) * (int)resp.data.at(0); }

double temp(Response resp) { return (int)resp.data.at(0) - 40; }

double rpm(Response resp) { return ((256.0 * (int)resp.data.at(0)) + (int)resp.data.at(1)) / 4.0; }

double speed(Response resp) { return (int)resp.data.at(0); }

double flow(Response resp) { return ((256.0 * (int)resp.data.at(0)) + (int)resp.data.at(1)) / 100.0; }

