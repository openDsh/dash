#ifndef OBD_HPP_
#define OBD_HPP_

#include <termios.h>

#include <mutex>
#include <string>

#include <obd/command.hpp>

class OBD {
   public:
    OBD();
    ~OBD();

    bool is_connected() { return this->connected; }

    bool query(Command &cmd, double &val);

    static OBD *get_instance();

   private:
    int fd;
    std::mutex obd_mutex;

    bool connected = false;

    std::string raw_query(std::string cmd) { return (this->_write(cmd) > 0) ? this->_read() : ""; }

    void connect(std::string, speed_t);
    void initialize();
    bool send(Request);
    int _write(std::string);
    bool is_failed_response(std::string);
    Response receive();
    std::string _read();
};

#endif
