#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>

#include "obd/obd.hpp"

OBD::OBD()
{
    this->connect("/dev/rfcomm0", B115200);
    if (this->connected) this->initialize();
}

OBD::~OBD()
{
    this->raw_query("ATZ");
    sleep(1);
    close(this->controller_fd);
    this->connected = false;
}

bool OBD::is_connected() { return this->connected; }

void OBD::connect(std::string device, speed_t baudrate)
{
    if (this->connected) return;

    this->controller_fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    if (this->controller_fd == -1) {
        this->connected = false;
        return;
    }

    fcntl(this->controller_fd, F_SETFL, 0);

    // Get the current options for the port
    struct termios options;
    tcgetattr(this->controller_fd, &options);

    // very important stuff that I absolutely fully understand
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_lflag &= !(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= !(OPOST);
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 100;

    tcflush(this->controller_fd, TCIOFLUSH);
    tcsetattr(this->controller_fd, TCSANOW, &options);

    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    tcsetattr(this->controller_fd, TCSANOW, &options);
    tcflush(this->controller_fd, TCIOFLUSH);

    this->connected = true;
}

void OBD::initialize()
{
    std::string defaults = this->raw_query("ATD");
    std::cout << defaults << std::endl;
    usleep(500000);

    std::string reset = this->raw_query("ATZ");
    std::cout << reset << std::endl;
    usleep(500000);

    std::string echo = this->raw_query("ATE0");
    std::cout << echo << std::endl;
    usleep(500000);

    std::string line = this->raw_query("ATL0");
    std::cout << line << std::endl;
    usleep(500000);

    std::string time = this->raw_query("ATS0");
    std::cout << time << std::endl;
    usleep(500000);

    std::string headers = this->raw_query("ATH0");
    std::cout << headers << std::endl;
    usleep(500000);

    std::string auto_mode = this->raw_query("ATSP0");
    std::cout << auto_mode << std::endl;
    usleep(500000);

    std::string pids = this->raw_query("0100");
    std::cout << pids << std::endl;
    usleep(500000);
}

std::string OBD::raw_query(std::string command)
{
    if (this->_write(command) > 0)
        return this->_read();
    else
        return "";
}

bool OBD::send(Request request)
{
    std::string request_str = request.to_string();

    return this->_write(request_str) >= 0;
}

int OBD::_write(std::string str)
{
    str += '\r';
    int size;
    if ((size = write(this->controller_fd, str.c_str(), str.length())) < 0) {
        std::cout << "failed write" << std::endl;
        this->connected = false;
		return 0;
    }
    std::cout << "[_write] '" << str << "'" << std::endl;
    return size;
}

bool OBD::is_failed_response(std::string str)
{
    std::vector<std::string> failed_messages = {"UNABLE TO CONNECT", "BUS INIT...ERROR", "?",
                                                "NO DATA",           "STOPPED",          "ERROR"};

    for (auto const &message : failed_messages) {
        if (str.find(message) != std::string::npos) return true;
    }
    return false;
}

Response OBD::receive()
{
    std::string response_str = this->_read();

    if (is_failed_response(response_str)) return Response();

    std::string searching_phrase = "SEARCHING...";
    std::string::size_type i = response_str.find(searching_phrase);
    if (i != std::string::npos) response_str.erase(i, searching_phrase.length());
    response_str.erase(std::remove_if(response_str.begin(), response_str.end(),
                                      [](auto const &c) -> bool { return !std::isalnum(c); }),
                       response_str.end());

    return Response(response_str);
}

std::string OBD::_read()
{
    char buf[1];
    std::string str;

    while (true) {
        if (read(this->controller_fd, (void *)buf, 1) != 1) {
            std::cout << "failed read" << std::endl;
            this->connected = false;
            return "";
        }
        if (buf[0] == '>')
            break;
        else
            str += buf[0];
    }

    std::cout << "[_read] '" << str << "'" << std::endl;
    return str;
}

OBD *OBD::get_instance()
{
    static OBD obd;
    return &obd;
}
