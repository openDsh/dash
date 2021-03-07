#include <QFileInfo>

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

bool OfficialRPi::supported()
{
    return QFileInfo(this->brightness_attribute).isWritable();
}

uint8_t OfficialRPi::priority()
{
    return 3;
}

void OfficialRPi::set(int brightness)
{
    if (this->brightness_attribute.isOpen()) {
        this->brightness_attribute.reset();
        this->brightness_attribute.write(std::to_string(brightness).c_str());
        this->brightness_attribute.flush();
    }
}
