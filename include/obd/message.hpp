#pragma once

#include <iomanip>
#include <sstream>
#include <string>

struct Message {
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
    unsigned char A;
    unsigned char B;
    unsigned char C;
    unsigned char D;

    Response(std::string resp_str)
    {
        this->mode = std::stoi(resp_str.substr(0, 2), nullptr, 16);
        this->PID = std::stoi(resp_str.substr(2, 2), nullptr, 16);
        this->A = std::stoi(resp_str.substr(4, 2), nullptr, 16);
        if (resp_str.length() >= 8) this->B = std::stoi(resp_str.substr(6, 2), nullptr, 16);
        if (resp_str.length() >= 10) this->C = std::stoi(resp_str.substr(8, 2), nullptr, 16);
        if (resp_str.length() == 12) this->D = std::stoi(resp_str.substr(10, 2), nullptr, 16);
    }

    Response() { this->success = false; }
};
