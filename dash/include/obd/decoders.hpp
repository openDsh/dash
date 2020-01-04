#ifndef DECODERS_HPP_
#define DECODERS_HPP_

#include "obd/message.hpp"

double percentage(Response resp) { return (100.0 / 255.0) * resp.A; }

double temp(Response resp) { return resp.A - 40; }

double rpm(Response resp) { return ((256.0 * resp.A) + resp.B) / 4.0; }

double speed(Response resp) { return resp.A; }

double flow(Response resp) { return ((256.0 * resp.A) + resp.B) / 100.0; }

#endif
