#include <QProcess>
#include <QTcpSocket>

#include "rtl_sdr.hpp"

RtlSdr::RtlSdr()
    : server()
    , socket()
{
    server.start("rtl_fm_streamer", {"-P", "2346", "-j", "2345"});

    socket.connectToHost("localhost", 2345);
}

bool RtlSdr::supported()
{
    return this->socket.isValid();
}

void RtlSdr::freq(uint32_t hz)
{
    if(socket.state() == QAbstractSocket::ConnectedState)
        socket.write("{\"method\": \"SetFrequency\", \"params\": [103500000]}");
}

int16_t RtlSdr::power_level()
{
    return -1;
}
