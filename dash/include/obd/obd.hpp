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

    bool query(Command &cmd, double &val);

    inline bool is_connected() { return this->connected; }

    static OBD *get_instance();

   private:
    void connect(std::string dev_path, speed_t baudrate);
    void initialize();
    bool send(Request req);
    int _write(std::string str);
    bool is_failed_response(std::string str);
    Response receive();
    std::string _read();

    inline std::string raw_query(std::string cmd) { return (this->_write(cmd) > 0) ? this->_read() : ""; }

    int fd;
    std::mutex obd_mutex;

    bool connected = false;
};

#endif
