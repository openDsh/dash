#include <QDebug>

#include "official_rpi.hpp"

OfficialRPi::OfficialRPi() : brightness_attribute(this->PATH)
{
    qDebug() << "[Dash][OfficialRPi] brightness attribute exists: " << this->brightness_attribute.exists(); // temp
    bool status = this->brightness_attribute.open(QIODevice::WriteOnly);
    qDebug() << "[Dash][OfficialRPi] brightness attribute open: " << status; // temp
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
        qDebug() << "[Dash][OfficialRPi] writing brightness attribute value " << brightness; // temp
        this->brightness_attribute.write(std::to_string(brightness).c_str());
        this->brightness_attribute.flush();
    }
    else {
        qDebug() << "[Dash][OfficialRPi] brightness attribute is not open"; // temp
    }
}
