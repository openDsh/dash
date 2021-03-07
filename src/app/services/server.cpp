#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "app/arbiter.hpp"
#include "app/session.hpp"
#include "app/pages/page.hpp"

#include "app/services/server.hpp"

Server::Server(Arbiter &arbiter)
    : QWebSocketServer("dash", QWebSocketServer::NonSecureMode)
    , arbiter(arbiter)
    , enabled_(this->arbiter.settings().value("System/Server/enabled", false).toBool())
{
    this->add_state_handlers();
    this->add_action_handlers();

    if (this->enabled_)
        this->start();

    connect(this, &QWebSocketServer::newConnection, [this]{
        if (auto client = this->nextPendingConnection()) {
            this->clients.append(client);

            auto path = client->requestUrl().path();
            if (path == "/state") {
                connect(client, &QWebSocket::textMessageReceived, [this, client](QString msg) {
                    this->handle_state_msg(client, msg);
                });
            }
            else if (path == "/action") {
                connect(client, &QWebSocket::textMessageReceived, [this, client](QString msg) {
                    this->handle_action_msg(client, msg);
                });
            }
        }
    });
}

Server::~Server()
{
    this->stop();
}

void Server::enable(bool enable)
{
    this->enabled_ = enable;
    this->arbiter.settings().setValue("System/Server/enabled", enable);

    if (enable)
        this->start();
    else
        this->stop();

    emit changed(enable);
}

void Server::register_handler(QString key, StateHandler handler)
{
    this->state_handlers.insert(key, handler);
}

void Server::register_handler(QString key, ActionHandler handler)
{
    this->action_handlers.insert(key, handler);
}

void Server::add_state_handlers()
{
    auto &arbiter = this->arbiter;

    this->register_handler("mode", Server::StateHandler{
        [&arbiter]{ return Session::Theme::to_str(arbiter.theme().mode); },
        [&arbiter](QVariant val){ arbiter.set_mode(Session::Theme::from_str(val.toString())); },
        [&arbiter]{ return QMap<QString, QVariant>({{"color", arbiter.theme().color().name()}}); }
    });
    this->register_handler("color", Server::StateHandler{
        [&arbiter]{ return arbiter.theme().color().name(); },
        [&arbiter](QVariant val){ arbiter.set_color(QColor(val.toString())); }
    });
    this->register_handler("page", Server::StateHandler{
        [&arbiter]{ return arbiter.layout().page_id(arbiter.layout().curr_page); },
        [&arbiter](QVariant val){ arbiter.set_curr_page(val.toInt()); }
    });
    this->register_handler("brightness", Server::StateHandler{
        [&arbiter]{ return arbiter.system().brightness.value; },
        [&arbiter](QVariant val){ arbiter.set_brightness(val.toUInt()); }
    });
    this->register_handler("volume", Server::StateHandler{
        [&arbiter]{ return arbiter.system().volume; },
        [&arbiter](QVariant val){ arbiter.set_volume(val.toUInt()); }
    });

    auto page_key = [this](Page *page){ return QString("page%1").arg(this->arbiter.layout().page_id(page)); };
    for (auto page : this->arbiter.layout().pages()) {
        this->register_handler(page_key(page), Server::StateHandler{
            [page]{ return QJsonObject({{"enabled", page->enabled()}, {"name", page->name()}}); },
            [&arbiter, page](QVariant val){ arbiter.set_page(page, val.toBool()); }
        });
    }
    this->register_handler("pages", Server::StateHandler{
        [this, page_key]{
            QJsonArray pages;
            for (auto page : this->arbiter.layout().pages())
                pages.append(QJsonObject({{"key", page_key(page)}, {"enabled", page->enabled()}, {"name", page->name()}}));
            return pages;
        },
        [&arbiter](QVariant val){
            auto pages = val.toJsonArray();
            for (auto i = 0; i < pages.size(); i++)
                arbiter.set_page(arbiter.layout().page(i), pages[i].toBool());
        }
    });
}

void Server::add_action_handlers()
{
    auto &arbiter = this->arbiter;

    this->register_handler("cycle_page", Server::ActionHandler{
        [&arbiter](QVariant val){
            auto cycles = val.isNull() ? 1 : val.toUInt();
            auto page = arbiter.layout().curr_page;
            for (unsigned int i = 0; i < cycles; i++)
                page = arbiter.layout().next_enabled_page(page);
            arbiter.set_curr_page(page);
            return QMap<QString, QVariant>({{"page", arbiter.layout().page_id(arbiter.layout().curr_page)}});
        }
    });
    this->register_handler("decrease_brightness", Server::ActionHandler{
        [&arbiter](QVariant val){
            arbiter.decrease_brightness(val.isNull() ? 4 : val.toUInt());
            return QMap<QString, QVariant>({{"brightness", arbiter.system().brightness.value}});
        }
    });
    this->register_handler("increase_brightness", Server::ActionHandler{
        [&arbiter](QVariant val){
            arbiter.increase_brightness(val.isNull() ? 4 : val.toUInt());
            return QMap<QString, QVariant>({{"brightness", arbiter.system().brightness.value}});
        }
    });
    this->register_handler("decrease_volume", Server::ActionHandler{
        [&arbiter](QVariant val){
            arbiter.decrease_volume(val.isNull() ? 2 : val.toUInt());
            return QMap<QString, QVariant>({{"volume", arbiter.system().volume}});
        }
    });
    this->register_handler("increase_volume", Server::ActionHandler{
        [&arbiter](QVariant val){
            arbiter.increase_volume(val.isNull() ? 2 : val.toUInt());
            return QMap<QString, QVariant>({{"volume", arbiter.system().volume}});
        }
    });
}

void Server::start()
{
    this->listen(QHostAddress::Any, this->PORT);
}

void Server::stop()
{
    this->close();
    for (auto client : this->clients)
        delete client;
    this->clients.clear();
}

void Server::fill_resp(QJsonObject &resp, QMap<QString, QVariant> entries) const
{
    for (auto entry = entries.begin(); entry != entries.end(); entry++)
        resp.insert(entry.key(), entry.value().toJsonValue());
}

void Server::handle_state_msg(QWebSocket *client, QString msg) const
{
    QJsonObject resp;

    auto json = QJsonDocument::fromJson(msg.toUtf8());
    if (json.isObject()) {
        auto object = json.object();
        for (auto entry = object.begin(); entry != object.end(); entry++) {
            auto handler = this->state_handlers.find(entry.key());
            if (handler != this->state_handlers.end()) {
                handler->update(entry.value().toVariant());
                resp.insert(entry.key(), handler->state().toJsonValue());
                if (handler->ext)
                    this->fill_resp(resp, handler->ext());
            }
        }
    }
    else if (json.isArray()) {
        for (auto value : json.array()) {
            auto key = value.toString();
            if (this->state_handlers.contains(key))
                resp.insert(key, this->state_handlers[key].state().toJsonValue());
        }
    }

    client->sendTextMessage(QString(QJsonDocument(resp).toJson()));
}

void Server::handle_action_msg(QWebSocket *client, QString msg) const
{
    QJsonObject resp;

    auto json = QJsonDocument::fromJson(msg.toUtf8());
    if (json.isObject()) {
        auto object = json.object();
        for (auto entry = object.begin(); entry != object.end(); entry++) {
            auto key = entry.key();
            if (this->action_handlers.contains(key))
                this->fill_resp(resp, this->action_handlers[key].action(entry.value().toVariant()));
        }
    }
    else if (json.isArray()) {
        for (auto value : json.array()) {
            auto key = value.toString();
            if (this->action_handlers.contains(key))
                this->fill_resp(resp, this->action_handlers[key].action(QVariant()));
        }
    }

    client->sendTextMessage(QString(QJsonDocument(resp).toJson()));
}
