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

    connect(&this->server, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this](int, QProcess::ExitStatus status){
        if (status != QProcess::NormalExit)
            this->kill();
    });
    connect(&this->player, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this](int, QProcess::ExitStatus status){
        if (status != QProcess::NormalExit)
            this->kill();
    });
    connect(&this->socket, &QTcpSocket::disconnected, [this](){ this->kill(); });
}

bool RtlSdr::play()
{
    if ((this->server.state() != QProcess::Running) || (this->player.state() != QProcess::Running) || !this->socket.isValid())
        return false;

    this->player.start();
    return true;
}

bool RtlSdr::stop()
{
    if ((this->server.state() != QProcess::Running) || (this->player.state() != QProcess::Running) || !this->socket.isValid())
        return false;

    this->player.terminate();
    return true;
}

bool RtlSdr::freq(int hz)
{
    if(this->socket.state() == QAbstractSocket::ConnectedState)
        return (this->socket.write(QJsonDocument(QJsonObject({{"method", "SetFrequency"}, {"params", QJsonArray({hz})}})).toJson()) != -1);
    else
        return false;
}

void RtlSdr::kill()
{
    this->socket.close();
    this->player.kill();
    this->server.kill();
}
