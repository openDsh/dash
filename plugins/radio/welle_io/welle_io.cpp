#include "welle_io.hpp"

WelleIo::WelleIo()
    : server()
    , player()
    , socket()
{
    this->server.start("welle-cli", {"-c", "welle_io.conf"});
    sleep(3);
    this->socket.connectToHost("localhost", 1234);
    this->socket.waitForConnected();

    this->player.setProgram("ffplay");
    this->player.setArguments({"http://localhost:1234", "-nodisp"});
}

void WelleIo::play()
{
    this->player.start();
}

void WelleIo::stop()
{
    this->player.terminate();
}

void WelleIo::freq(int hz)
{
    if(this->socket.state() == QAbstractSocket::ConnectedState)
        this->socket.write(QJsonDocument(QJsonObject({{"method", "SetFrequency"}, {"params", QJsonArray({hz})}})).toJson());
}
