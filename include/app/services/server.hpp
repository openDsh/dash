#pragma once

#include <functional>

#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QWebSocket>
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

    Server(Arbiter &arbiter);
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
    void handle_msg(QWebSocket *client, QString request) const;

   signals:
    void changed(bool enabled);
};
