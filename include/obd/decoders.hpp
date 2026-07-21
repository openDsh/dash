#pragma once
#include <limits>
#include <obd/message.hpp>

namespace {
	
	inline bool has_obd_data(const Response &resp, int required_bytes)
	{
		return resp.success && resp.data.size() >= required_bytes;
	}
	
	inline int obd_byte(const Response &resp, int index)
	{
		return static_cast<unsigned char>(resp.data.at(index));
	}
	
	inline double invalid_obd_value()
	{
		return std::numeric_limits<double>::quiet_NaN();
	}
	
} // namespace

inline double percentage(const Response &resp)
{
	if (!has_obd_data(resp, 1))
		return invalid_obd_value();
	
	const int a = obd_byte(resp, 0);
	return (100.0 / 255.0) * a;
}

inline double temp(const Response &resp)
{
	if (!has_obd_data(resp, 1))
		return invalid_obd_value();
	
	const int a = obd_byte(resp, 0);
	return a - 40.0;
}

inline double rpm(const Response &resp)
{
	if (!has_obd_data(resp, 2))
		return invalid_obd_value();
	
	const int a = obd_byte(resp, 0);
	const int b = obd_byte(resp, 1);
	
	return ((256.0 * a) + b) / 4.0;
}

inline double speed(const Response &resp)
{
	if (!has_obd_data(resp, 1))
		return invalid_obd_value();
	
	return obd_byte(resp, 0);
}

inline double flow(const Response &resp)
{
	if (!has_obd_data(resp, 2))
		return invalid_obd_value();
	
	const int a = obd_byte(resp, 0);
	const int b = obd_byte(resp, 1);
	
	return ((256.0 * a) + b) / 100.0;
}

/*
double percentage(Response resp) { return (100.0 / 255.0) * (int)resp.data.at(0); }

double temp(Response resp) { return (int)resp.data.at(0) - 40; }

double rpm(Response resp) { return ((256.0 * (int)resp.data.at(0)) + (int)resp.data.at(1)) / 4.0; }

double speed(Response resp) { return (int)resp.data.at(0); }

double flow(Response resp) { return ((256.0 * (int)resp.data.at(0)) + (int)resp.data.at(1)) / 100.0; }
*/
