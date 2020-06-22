#ifndef OPENAUTO_HPP_
#define OPENAUTO_HPP_

#include <QtWidgets>
#include <thread>
#include <f1x/aasdk/TCP/TCPWrapper.hpp>
#include <f1x/aasdk/USB/AccessoryModeQueryChain.hpp>
#include <f1x/aasdk/USB/AccessoryModeQueryChainFactory.hpp>
#include <f1x/aasdk/USB/AccessoryModeQueryFactory.hpp>
#include <f1x/aasdk/USB/ConnectedAccessoriesEnumerator.hpp>
#include <f1x/aasdk/USB/USBHub.hpp>
#include <f1x/openauto/autoapp/App.hpp>
#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>
#include <f1x/openauto/autoapp/Configuration/IConfiguration.hpp>
#include <f1x/openauto/autoapp/Configuration/RecentAddressesList.hpp>
#include <f1x/openauto/autoapp/Service/AndroidAutoEntityFactory.hpp>
#include <f1x/openauto/autoapp/Service/ServiceFactory.hpp>

#include <app/config.hpp>
#include <app/theme.hpp>

namespace aasdk = f1x::aasdk;
namespace autoapp = f1x::openauto::autoapp;

class OpenAutoWorker : public QObject {
    Q_OBJECT

   public:
    OpenAutoWorker(std::function<void(bool)> callback = nullptr, QWidget *parent = nullptr, bool night_mode = false);
    ~OpenAutoWorker();
    void connect_wireless(QString address);

    inline void start() { this->app->waitForUSBDevice(); }
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
    std::shared_ptr<autoapp::configuration::Configuration> configuration;
    aasdk::tcp::TCPWrapper tcp_wrapper;
    aasdk::usb::USBWrapper usb_wrapper;
    aasdk::usb::AccessoryModeQueryFactory query_factory;
    aasdk::usb::AccessoryModeQueryChainFactory query_chain_factory;
    autoapp::service::ServiceFactory service_factory;
    autoapp::service::AndroidAutoEntityFactory android_auto_entity_factory;
    std::shared_ptr<aasdk::usb::USBHub> usb_hub;
    std::shared_ptr<aasdk::usb::ConnectedAccessoriesEnumerator> connected_accessories_enumerator;
    std::shared_ptr<autoapp::App> app;
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

#endif
