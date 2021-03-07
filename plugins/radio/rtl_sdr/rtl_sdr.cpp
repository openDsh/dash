#include <unistd.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTcpSocket>

#include "rtl_sdr.hpp"

RtlSdr::RtlSdr()
    : server()
    , player()
    , socket()
{
    this->server.start("rtl_fm_streamer", {"-P", "2346", "-j", "2345"});
    sleep(1);
    this->socket.connectToHost("localhost", 2345);
    this->socket.waitForConnected();

    this->player.setProgram("ffplay");
    this->player.setArguments({"http://localhost:2346", "-nodisp"});
}

void RtlSdr::play()
{
    this->player.start();
}

void RtlSdr::stop()
{
    this->player.terminate();
}

void RtlSdr::freq(int hz)
{
    if(this->socket.state() == QAbstractSocket::ConnectedState)
        this->socket.write(QJsonDocument(QJsonObject({{"method", "SetFrequency"}, {"params", QJsonArray({hz})}})).toJson());
}
