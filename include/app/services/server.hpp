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
    Server(Arbiter &arbiter);
    ~Server();
    void enable(bool enable);
    void register_state_handler(QString key, std::function<QVariant()> get, std::function<void(QVariant)> set);
    void register_action_handler(QString key, std::function<QVariant()> get);

    bool enabled() const { return this->enabled_; }

   private:
    const uint16_t PORT = 54545; // thats 0xD511 (DSH) in decimal

    struct Handler {
        enum class Type {
            State,
            Action
        };

        Type type;
        std::function<QVariant()> get;
        std::function<void(QVariant)> set;
    };

    Arbiter &arbiter;
    QList<QWebSocket *> clients;
    QMap<QString, Handler> handlers;
    bool enabled_;

    void add_state_handlers();
    void add_action_handlers();
    void start();
    void stop();
    void handle_msg(QWebSocket *client, QString request) const;

   signals:
    void changed(bool enabled);
};
