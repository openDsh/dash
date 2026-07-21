#pragma once

#include <iomanip>
#include <sstream>
#include <string>
#include <QByteArray>
#include <algorithm>

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
    bool success = false;
    QByteArray data;
    
    explicit Response(const QByteArray &payload)
    : Message{0, 0, 0}
    {
        if (payload.size() < 3)
            return;
        
        this->length =
        static_cast<unsigned char>(payload.at(0));
        
        if (this->length < 2)
            return;
        
        this->mode =
        static_cast<unsigned char>(payload.at(1)) - 0x40;
        
        this->PID =
        static_cast<unsigned char>(payload.at(2));
        
        const int data_length =
        std::min<int>(this->length - 2, payload.size() - 3);
        
        if (data_length > 0)
            this->data = payload.mid(3, data_length);
        
        this->success = true;
    }
    
    Response()
    : Message{0, 0, 0}
    , success(false)
    {
    }
};

/*
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
*/
