#pragma once

#include <functional>

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QtWebSockets>
#include <QVariant>
#include <QWebSocketServer>

class Arbiter;

class Server : public QWebSocketServer
{
    Q_OBJECT

   public:
    struct Handler {
        std::function<QVariant()> get;
        std::function<void(QVariant)> set;
    };

    Server(Arbiter &arbiter, QObject *parent = nullptr);
    ~Server();

    void enable(bool enable);
    void register_handler(QString key, Handler handler);

    bool enabled() const { return this->enabled_; }

   private:
    const uint16_t PORT = 54545; // thats 0xD511 (DSH) in decimal

    Arbiter &arbiter;
    QList<QWebSocket *> clients;
    QMap<QString, Handler> handlers;

    bool enabled_;

    void start();
    void stop();
    void handle_request(QWebSocket *client, QString request);

   signals:
    void changed(bool enabled);
};
