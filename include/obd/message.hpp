#pragma once

#include <iomanip>
#include <sstream>
#include <string>
#include <QByteArray>

struct Message {
    unsigned char length;
    unsigned char mode;
    unsigned char PID;
};

struct Request : Message {
    Request(unsigned char mode, unsigned char PID)
    {
        this->mode = mode;
        this->PID = PID;
    }

    std::string to_str()
    {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        ss << std::setw(2) << static_cast<unsigned>(this->mode) << std::setw(2) << static_cast<unsigned>(this->PID);
        return ss.str();
    }
};

struct Response : Message {
    bool success = true;
    QByteArray data;

    Response(QByteArray payload)
    {
        this->length = payload.at(0);
        if(length > 0) this->mode = payload.at(1)-0x40;
        if(length > 1) this->PID = payload.at(2);
        if(length > 2) data = payload.mid(3);
        
    }

    Response() { this->success = false; }
};

