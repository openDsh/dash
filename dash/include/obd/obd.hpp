#ifndef OBD_HPP_
#define OBD_HPP_

#include <termios.h>
#include <time.h>
#include <iostream>
#include <mutex>
#include <string>

#include "obd/command.hpp"

class OBD {
   public:
    OBD();
    ~OBD();

    bool is_connected();

    template <typename T>
    bool query(Command<T> &command, T &value)
    {
        std::lock_guard<std::mutex> guard(this->obd_mutex);
        if (this->send(command.request)) {
            Response response = this->receive();
            value = command.decoder(response);
            return response.success;
        }
        else
            std::cout << "unable to send command" << std::endl;

        return false;
    }

    template <typename T>
    bool query(T v, T &r)
    {
        std::lock_guard<std::mutex> guard(this->obd_mutex);
        r = v;
        return true;
    }

    static OBD *get_instance();

   private:
    std::string device;
    speed_t baudrate;
    int controller_fd;

    bool connected = false;

    std::mutex obd_mutex;

    void connect(std::string device, speed_t baudrate);
    void initialize();

    std::string raw_query(std::string);

    bool send(Request);

    int _write(std::string);

    bool is_failed_response(std::string);

    Response receive();

    std::string _read();
};

#endif
