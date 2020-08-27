#pragma once

#include <QtWidgets>
#include <thread>
#include "aasdk/TCP/TCPWrapper.hpp"
#include "aasdk/USB/AccessoryModeQueryChain.hpp"
#include "aasdk/USB/AccessoryModeQueryChainFactory.hpp"
#include "aasdk/USB/AccessoryModeQueryFactory.hpp"
#include "aasdk/USB/ConnectedAccessoriesEnumerator.hpp"
#include "aasdk/USB/USBHub.hpp"
#include "openauto/App.hpp"
#include "openauto/Configuration/Configuration.hpp"
#include "openauto/Configuration/IConfiguration.hpp"
#include "openauto/Configuration/RecentAddressesList.hpp"
#include "openauto/Service/AndroidAutoEntityFactory.hpp"
#include "openauto/Service/ServiceFactory.hpp"

#include "app/config.hpp"
#include "app/theme.hpp"

class OpenAutoWorker : public QObject {
    Q_OBJECT

   public:
    OpenAutoWorker(std::function<void(bool)> callback = nullptr, QWidget *parent = nullptr, bool night_mode = false);
    ~OpenAutoWorker();
    void connect_wireless(QString address);

    inline void start() { this->app->waitForDevice(true); }
    inline void set_opacity(unsigned int alpha) { this->service_factory.setOpacity(alpha); }
    inline void resize() { this->service_factory.resize(); }
    inline void set_night_mode(bool mode) { this->service_factory.setNightMode(mode); }
    inline void send_key_event(QKeyEvent *event) { this->service_factory.sendKeyEvent(event); }

   private:
    const int OPENAUTO_PORT = 5277;

    void create_usb_workers();
    void create_io_service_workers();

    libusb_context *usb_context;
    boost::asio::io_service io_service;
    boost::asio::io_service::work work;
    std::shared_ptr<openauto::configuration::Configuration> configuration;
    aasdk::tcp::TCPWrapper tcp_wrapper;
    aasdk::usb::USBWrapper usb_wrapper;
    aasdk::usb::AccessoryModeQueryFactory query_factory;
    aasdk::usb::AccessoryModeQueryChainFactory query_chain_factory;
    openauto::service::ServiceFactory service_factory;
    openauto::service::AndroidAutoEntityFactory android_auto_entity_factory;
    std::shared_ptr<aasdk::usb::USBHub> usb_hub;
    std::shared_ptr<aasdk::usb::ConnectedAccessoriesEnumerator> connected_accessories_enumerator;
    std::shared_ptr<openauto::App> app;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    std::vector<std::thread> thread_pool;

   signals:
    void wireless_connection_success(QString address);
    void wireless_connection_failure();
};

class OpenAutoFrame : public QWidget {
    Q_OBJECT

   public:
    OpenAutoFrame(QWidget *parent);

    inline bool is_fullscreen() { return this->fullscreen; }
    inline void toggle_fullscreen() { this->fullscreen = !this->fullscreen; }

   protected:
    void mouseDoubleClickEvent(QMouseEvent *);
    inline void enterEvent(QEvent *) { this->setFocus(); }

   private:
    bool fullscreen = false;

   signals:
    void double_clicked(bool fullscreen);
    void toggle(bool enable);
};

class OpenAutoTab : public QWidget {
    Q_OBJECT

   public:
    OpenAutoTab(QWidget *parent = nullptr);
    inline void send_key_event(QKeyEvent *event) { if (this->worker != nullptr) this->worker->send_key_event(event); }

   private:
    QWidget *msg_widget();
    QWidget *wireless_widget();

    Config *config;
    Theme *theme;
    OpenAutoWorker *worker = nullptr;

   signals:
    void connect_wireless();
};

