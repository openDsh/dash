#ifndef DECODERS_HPP_
#define DECODERS_HPP_

#include "obd/message.hpp"

double percentage(Response response) { return (100.0 / 255.0) * response.A; }

double temp(Response response) { return response.A - 40; }

double rpm(Response response) { return ((256.0 * response.A) + response.B) / 4.0; }

double speed(Response response) { return response.A; }

double flow(Response response) { return ((256.0 * response.A) + response.B) / 100.0; }

#endif
