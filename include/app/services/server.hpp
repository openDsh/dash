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
    struct StateHandler {
        std::function<QVariant()> state;
        std::function<void(QVariant)> update;
        std::function<QMap<QString, QVariant>()> ext = nullptr;
    };

    struct ActionHandler {
        std::function<QMap<QString, QVariant>(QVariant)> action;
    };

    Server(Arbiter &arbiter);
    ~Server();
    void enable(bool enable);
    void register_handler(QString key, StateHandler handler);
    void register_handler(QString key, ActionHandler handler);

    bool enabled() const { return this->enabled_; }

   private:
    const uint16_t PORT = 54545; // thats 0xD511 (DSH) in decimal

    Arbiter &arbiter;
    QList<QWebSocket *> clients;
    QMap<QString, StateHandler> state_handlers;
    QMap<QString, ActionHandler> action_handlers;
    bool enabled_;

    void add_state_handlers();
    void add_action_handlers();
    void start();
    void stop();
    void fill_resp(QJsonObject &resp, QMap<QString, QVariant> entries) const;
    void handle_state_msg(QWebSocket *client, QString msg) const;
    void handle_action_msg(QWebSocket *client, QString msg) const;

   signals:
    void changed(bool enabled);
};
