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
    this->handlers = {
        {"mode", {
            [&arbiter]{ return QVariant(Session::Theme::to_str(arbiter.theme().mode)); },
            [&arbiter](QVariant val){ arbiter.set_mode(Session::Theme::from_str(val.toString())); }
        }},
        {"color", {
            [&arbiter]{ return QVariant(arbiter.theme().color().name()); },
            [&arbiter](QVariant val){ arbiter.set_color(QColor(val.toString())); }
        }},
        {"page", {
            [&arbiter]{ return QVariant(arbiter.layout().page_id(arbiter.layout().curr_page)); },
            [&arbiter](QVariant val){ arbiter.set_curr_page(val.toInt()); }
        }},
        {"brightness", {
            [&arbiter]{ return QVariant(arbiter.system().brightness.value); },
            [&arbiter](QVariant val){ arbiter.set_brightness(val.toUInt()); }
        }},
        {"volume", {
            [&arbiter]{ return QVariant(arbiter.system().volume); },
            [&arbiter](QVariant val){ arbiter.set_volume(val.toUInt()); }
        }}
    };

    QJsonArray pages;
    for (auto page : this->arbiter.layout().pages()) {
        auto key = QString("page%1").arg(this->arbiter.layout().page_id(page));
        this->handlers.insert(key, {
            [page]{ return QVariant(page->enabled()); },
            [&arbiter, page](QVariant val){ arbiter.set_page(page, val.toBool()); }
        });
        pages.append(key);
    }
    this->handlers.insert("pages", {[pages]{ return QVariant(QJsonDocument(pages)); }, [](QVariant){}});

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

void Server::register_handler(QString key, Handler handler)
{
    this->handlers[key] = handler;
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
    if (json.isObject()) {
        auto object = json.object();
        for (auto key : object.keys()) {
            if (this->handlers.contains(key))
                this->handlers[key].set(object.value(key));
        }
    }
    else if (json.isArray()) {
        QJsonObject repsonse;
        for (auto value : json.array()) {
            auto key = value.toString();
            if (this->handlers.contains(key))
                repsonse.insert(key, this->handlers[key].get().toJsonValue());
        }
        client->sendTextMessage(QString(QJsonDocument(repsonse).toJson()));
    }
}
