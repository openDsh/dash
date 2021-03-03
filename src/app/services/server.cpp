#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "app/arbiter.hpp"
#include "app/session.hpp"

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
            connect(client, &QWebSocket::textMessageReceived, [this, client](QString msg) {
                this->handle_msg(client, msg);
            });
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

void Server::register_state_handler(QString key, std::function<QVariant()> get, std::function<void(QVariant)> set)
{
    this->handlers[key] = { Server::Handler::Type::State, get, set };
}

void Server::register_action_handler(QString key, std::function<QVariant()> get)
{
    this->handlers[key] = { Server::Handler::Type::Action, get, nullptr };
}

void Server::add_state_handlers()
{
    auto &arbiter = this->arbiter;

    this->register_state_handler("mode",
        [&arbiter]{ return QVariant(Session::Theme::to_str(arbiter.theme().mode)); },
        [&arbiter](QVariant val){ arbiter.set_mode(Session::Theme::from_str(val.toString())); }
    );
    this->register_state_handler("page",
        [&arbiter]{ return QVariant(arbiter.layout().page_id(arbiter.layout().curr_page)); },
        [&arbiter](QVariant val){ arbiter.set_curr_page(val.toInt()); }
    );
    this->register_state_handler("brightness",
        [&arbiter]{ return QVariant(arbiter.system().brightness.value); },
        [&arbiter](QVariant val){ arbiter.set_brightness(val.toUInt()); }
    );
    this->register_state_handler("volume",
        [&arbiter]{ return QVariant(arbiter.system().volume); },
        [&arbiter](QVariant val){ arbiter.set_volume(val.toUInt()); }
    );

    auto page_key = [this](Page *page){ return QString("page%1").arg(this->arbiter.layout().page_id(page)); };
    for (auto page : this->arbiter.layout().pages()) {
        this->register_state_handler(page_key(page),
            [page]{ return QVariant(QJsonObject({{"enabled", page->enabled()}, {"name", page->name()}})); },
            [&arbiter, page](QVariant val){ arbiter.set_page(page, val.toBool()); }
        );
    }
    this->register_state_handler("pages",
        [this, page_key]{
            QJsonArray pages;
            for (auto page : this->arbiter.layout().pages())
                pages.append(QJsonObject({{"key", page_key(page)}, {"enabled", page->enabled()}, {"name", page->name()}}));
            return QVariant(pages);
        },
        [&arbiter](QVariant val){
            auto pages = val.toJsonArray();
            for (auto i = 0; i < pages.size(); i++)
                arbiter.set_page(arbiter.layout().page(i), pages[i].toBool());
        }
    );
}

void Server::add_action_handlers()
{
    auto &arbiter = this->arbiter;

    this->register_action_handler("cycle_page",
        [&arbiter]{
            auto page = arbiter.layout().next_enabled_page(arbiter.layout().curr_page);
            return QVariant((arbiter.set_curr_page(page), arbiter.layout().page_id(arbiter.layout().curr_page)));
        }
    );
    this->register_action_handler("decrease_brightness",
        [&arbiter]{ return QVariant((arbiter.decrease_brightness(4), arbiter.system().brightness.value)); }
    );
    this->register_action_handler("increase_brightness",
        [&arbiter]{ return QVariant((arbiter.increase_brightness(4), arbiter.system().brightness.value)); }
    );
    this->register_action_handler("decrease_volume",
        [&arbiter]{ return QVariant((arbiter.decrease_volume(2), arbiter.system().volume)); }
    );
    this->register_action_handler("increase_volume",
        [&arbiter]{ return QVariant((arbiter.increase_volume(2), arbiter.system().volume)); }
    );
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

void Server::handle_msg(QWebSocket *client, QString request) const
{
    auto json = QJsonDocument::fromJson(request.toUtf8());
    QJsonObject repsonse;
    if (json.isObject()) {
        auto object = json.object();
        for (auto key : json.object().keys()) {
            auto it = this->handlers.find(key);
            if (it != this->handlers.end()) {
                auto handler = *it;
                if (handler.type == Server::Handler::Type::State) {
                    handler.set(object.value(key));
                    repsonse.insert(key, handler.get().toJsonValue());
                }
            }
        }
    }
    else if (json.isArray()) {
        for (auto value : json.array()) {
            auto key = value.toString();
            auto it = this->handlers.find(key);
            if (it != this->handlers.end()) {
                auto handler = *it;
                repsonse.insert(key, handler.get().toJsonValue());
            }
        }
    }
    client->sendTextMessage(QString(QJsonDocument(repsonse).toJson()));
}
