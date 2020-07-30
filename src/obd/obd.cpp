// #include <fcntl.h>
// #include <unistd.h>

// #include <algorithm>
// #include <iostream>
// #include <vector>

// #include "obd/obd.hpp"

// OBD::OBD()
// {
//     this->connect("/dev/ttyUSB0", B115200);
//     if (this->connected) this->initialize();
// }

// OBD::~OBD()
// {
//     this->raw_query("ATZ");
//     sleep(1);
//     close(this->fd);
//     this->connected = false;
// }

// bool OBD::query(Command &cmd, double &val)
// {
//     if (!this->connected) return false;

//     std::lock_guard<std::mutex> guard(this->obd_mutex);
//     if (this->send(cmd.request)) {
//         Response resp = this->receive();
//         val = cmd.decoder(resp);
//         return resp.success;
//     }
//     else {
//         std::cout << "unable to send command" << std::endl;
//     }

//     return false;
// }

// OBD *OBD::get_instance()
// {
//     static OBD obd;
//     return &obd;
// }

// void OBD::connect(std::string dev_path, speed_t baudrate)
// {
//     if (this->connected) return;

//     this->fd = open(dev_path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
//     if (this->fd == -1) {
//         this->connected = false;
//         return;
//     }

//     fcntl(this->fd, F_SETFL, 0);

//     struct termios options;
//     tcgetattr(this->fd, &options);

//     options.c_cflag |= (CLOCAL | CREAD);
//     options.c_lflag &= !(ICANON | ECHO | ECHOE | ISIG);
//     options.c_oflag &= !(OPOST);
//     options.c_cc[VMIN] = 0;
//     options.c_cc[VTIME] = 100;

//     tcflush(this->fd, TCIOFLUSH);
//     tcsetattr(this->fd, TCSANOW, &options);

//     cfsetispeed(&options, baudrate);
//     cfsetospeed(&options, baudrate);
//     tcsetattr(this->fd, TCSANOW, &options);
//     tcflush(this->fd, TCIOFLUSH);

//     this->connected = true;
// }

// void OBD::initialize()
// {
//     std::vector<std::string> cmds = {"ATD", "ATZ", "ATE0", "ATL0", "ATS0", "ATH0", "ATSP0", "0100"};
//     for (auto const cmd : cmds) {
//         this->raw_query(cmd);
//         usleep(500000);
//     }
// }

// bool OBD::send(Request req)
// {
//     std::string req_str = req.to_str();

//     return this->_write(req_str) >= 0;
// }

// int OBD::_write(std::string str)
// {
//     str += '\r';
//     int size;
//     if ((size = write(this->fd, str.c_str(), str.length())) < 0) {
//         std::cout << "failed write" << std::endl;
//         this->connected = false;
//         return 0;
//     }

//     return size;
// }

// bool OBD::is_failed_response(std::string str)
// {
//     std::vector<std::string> failed_msgs = {
//         "UNABLE TO CONNECT", "BUS INIT...ERROR", "NO DATA", "STOPPED", "ERROR", "?"};

//     for (auto const &msg : failed_msgs)
//         if (str.find(msg) != std::string::npos) return true;

//     return false;
// }

// Response OBD::receive()
// {
//     std::string resp_str = this->_read();

//     if (is_failed_response(resp_str)) return Response();

//     std::string searching_phrase = "SEARCHING...";
//     std::string::size_type i = resp_str.find(searching_phrase);
//     if (i != std::string::npos) resp_str.erase(i, searching_phrase.length());
//     resp_str.erase(
//         std::remove_if(resp_str.begin(), resp_str.end(), [](auto const &c) -> bool { return !std::isalnum(c); }),
//         resp_str.end());

//     return Response(resp_str);
// }

// std::string OBD::_read()
// {
//     char buf[1];
//     std::string str;

//     while (true) {
//         if (read(this->fd, (void *)buf, 1) != 1) {
//             std::cout << "failed read" << std::endl;
//             this->connected = false;
//             return "";
//         }
//         if (buf[0] == '>')
//             break;
//         else
//             str += buf[0];
//     }

//     return str;
// }
