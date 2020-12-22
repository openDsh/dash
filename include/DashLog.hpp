#pragma once

#include <boost/log/trivial.hpp>

#define DASH_LOG(severity) BOOST_LOG_TRIVIAL(severity) << "[Dash] "
