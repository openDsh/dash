#include "canbus/elm327.hpp"

elm327::elm327(QString canInterface, bool bluetooth)
{
    DASH_LOG(info)<<"[ElM327] Attempting to connect to elm device: "<<canInterface.toStdString();
    if(bluetooth){
        this->adapterType = BT;
        btSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
        QObject::connect(btSocket, &QBluetoothSocket::connected, this, &elm327::btConnected);
        QObject::connect(btSocket, &QBluetoothSocket::stateChanged, this, &elm327::socketChanged);
        btSocket->connectToService(QBluetoothAddress(canInterface), QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB")), QIODevice::ReadWrite);
        bluetooth_watchdog = new QTimer(this);
    }
    else{
        this->connect(canInterface, B115200);
        if (this->connected) this->initialize();
    }

}
elm327::~elm327()
{
    this->raw_query("ATZ");
    sleep(1);
    close(this->fd);
    this->connected = false;
}

elm327 *elm327::get_usb_instance()
{
    static elm327 elm_usb(Config::get_instance()->get_vehicle_interface());
    return &elm_usb;
}

elm327 *elm327::get_bt_instance()
{
    static elm327 elm_bt(Config::get_instance()->get_vehicle_interface(), true);
    return &elm_bt;
}

void elm327::btConnected()
{
    this->connected=true;
    this->initialize();
}

void elm327::socketChanged(QBluetoothSocket::SocketState state)
{
    switch(state){
        case(QBluetoothSocket::UnconnectedState):
            DASH_LOG(info)<<"[ElM327][Bluetooth] Unconnected";
            break;
        case(QBluetoothSocket::ConnectingState):
            DASH_LOG(info)<<"[ElM327][Bluetooth] Connecting";
            break;
        case(QBluetoothSocket::ConnectedState):
            DASH_LOG(info)<<"[ElM327][Bluetooth] Connected";
            break;
        case(QBluetoothSocket::ServiceLookupState):
            DASH_LOG(info)<<"[ElM327][Bluetooth] Looking up Services";
            break;
        default:
            DASH_LOG(info)<<"[ElM327][Bluetooth] Unimplemented";
    }
}

int elm327::_write(std::string str)
{
    str += '\r';
    int size;
    if ((size = (adapterType==BT)?(::send(btSocket->socketDescriptor(), str.c_str(), str.length(), 0)):(write(this->fd, str.c_str(), str.length()))) < 0)
    {
        DASH_LOG(error) << "[ELM327] failed write" << std::endl;
        this->connected = false;
        return 0;
    }

    return size;
}


void elm327::connect(QString dev_path, speed_t baudrate)
{
    if (this->connected) return;

    this->fd = open(dev_path.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (this->fd == -1) {
        this->connected = false;
        return;
    }

    fcntl(this->fd, F_SETFL, 0);

    struct termios options;
    tcgetattr(this->fd, &options);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_lflag &= !(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= !(OPOST);
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 100;

    tcflush(this->fd, TCIOFLUSH);
    tcsetattr(this->fd, TCSANOW, &options);

    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    tcsetattr(this->fd, TCSANOW, &options);
    tcflush(this->fd, TCIOFLUSH);

    this->connected = true;
}
void elm327::initialize()
{
    DASH_LOG(info)<<"[ELM327] Initializing elm";

    std::vector<std::string> cmds = {"ATD", "ATZ", "ATE0", "ATL0", "ATS0", "AT H0", "ATSP0", "0100"};
    for (auto const cmd : cmds) {
        this->raw_query(cmd);
        usleep(500000);
    }
}


bool elm327::writeFrame(QCanBusFrame frame)
{
    if(!this->connected) return false;
    std::lock_guard<std::mutex> guard(this->elm_mutex);
    std::stringstream ss;
        
    //this is an obd message, so we can send it with the elm327
    if(frame.frameId() == 0x7df){
        ss << std::hex << std::uppercase << std::setfill('0');
        for(int i = 1; i<=frame.payload().at(0); i++){
            ss << std::setw(2) << static_cast<unsigned>(frame.payload().at(i));
        }
        if(this->_write(ss.str())>=0){
            QCanBusFrame retFrame = this->receive();
            if(retFrame.frameType()!=QCanBusFrame::ErrorFrame){
                handleFrame(retFrame);
            }
        }
        else{
            DASH_LOG(error) << "[ELM327] Unable to send command";
        }


    }
    else{
        //not implementing elm327 support to send frames
        return false;
    }
    
    return true; 
}

bool elm327::is_failed_response(std::string str)
{
    std::vector<std::string> failed_msgs = {
        "UNABLE TO CONNECT", "BUS INIT...ERROR", "NO DATA", "STOPPED", "ERROR", "?"};

    for (auto const &msg : failed_msgs)
        if (str.find(msg) != std::string::npos) return true;

    return false;
}

QCanBusFrame elm327::receive()
{
    QCanBusFrame *retFrame = new QCanBusFrame();
    std::string resp_str = this->_read();

    if (is_failed_response(resp_str)){
        retFrame->setFrameType(QCanBusFrame::ErrorFrame);
        return *retFrame;
    }

    std::string searching_phrase = "SEARCHING...";
    std::string::size_type i = resp_str.find(searching_phrase);
    if (i != std::string::npos) resp_str.erase(i, searching_phrase.length());
    resp_str.erase(
        std::remove_if(resp_str.begin(), resp_str.end(), [](auto const &c) -> bool { return !std::isalnum(c); }),
        resp_str.end());

    retFrame->setFrameId(0x7E8);
    QByteArray payload = QByteArray::fromHex("0000000000000000");
    for( uint i=0; i < resp_str.length()/2; i++){
        payload[i+1] = std::stoi(resp_str.substr(2*i, 2), nullptr, 16);
    }
    payload[0] = resp_str.length()/2;
    retFrame->setPayload(payload);
    return *retFrame;
}
void elm327::handleFrame(QCanBusFrame frame)
{

    if (callbacks.find(frame.frameId()) != callbacks.end())
    {
        for(auto callback : callbacks[frame.frameId()]){
            callback(frame.payload());
        }
    }
    
} 


void elm327::registerFrameHandler(int id, std::function<void(QByteArray)> callback)
{
    callbacks[id].push_back(callback);
}

std::string elm327::_read()
{
    char buf[1];
    std::string str;
    
    if(adapterType==BT)
        bluetooth_watchdog->start(5000);

    while (true) {

        if (((adapterType==BT)?(::recv(btSocket->socketDescriptor(), (void *) buf, 1, 0)):(read(this->fd, (void *)buf, 1))) != 1)
        {
            if(adapterType != BT)
            {
                DASH_LOG(error) << "[ELM327] failed read";
                this->connected = false;
                return "";
            }
            else
            {
                if(bluetooth_watchdog->remainingTime() == 0)
                {
                    DASH_LOG(error) << "[ELM327] Bluetooth watchdog expired, disconnecting";
                    this->connected = false;
                    return "";
                }
                continue;
            }
        }
        if (buf[0] == '>')
            break;
        else
            str += buf[0];
        // feed watchdog
        if(adapterType==BT)
            bluetooth_watchdog->start(5000);
    }

    return str;
}
