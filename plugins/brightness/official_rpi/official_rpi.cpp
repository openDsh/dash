#include "official_rpi.hpp"

OfficialRPi::OfficialRPi() : brightness_attribute(this->PATH)
{
    this->brightness_attribute.open(QIODevice::WriteOnly);
}

OfficialRPi::~OfficialRPi()
{
    if (this->brightness_attribute.isOpen())
        this->brightness_attribute.close();
}

void OfficialRPi::set(int brightness)
{
    if (this->brightness_attribute.isOpen()) {
        this->brightness_attribute.reset();
        this->brightness_attribute.write(std::to_string(brightness).c_str());
        this->brightness_attribute.flush();
    }
}
