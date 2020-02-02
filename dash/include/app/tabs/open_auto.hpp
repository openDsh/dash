#ifndef OPEN_AUTO_HPP_
#define OPEN_AUTO_HPP_

#include <QtWidgets>

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

namespace aasdk = f1x::aasdk;
namespace autoapp = f1x::openauto::autoapp;

class OpenAutoWorker {
   public:
    OpenAutoWorker(std::function<void(bool)> callback = nullptr, QWidget *parent = nullptr);
    ~OpenAutoWorker();

    inline void start() { this->app->waitForUSBDevice(); }
    inline void set_opacity(unsigned int alpha) { this->service_factory.setOpacity(alpha); }

   private:
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
    std::vector<std::thread> thread_pool;
};

class OpenAutoTab : public QWidget {
    Q_OBJECT

   public:
    OpenAutoTab(QWidget *parent = nullptr);

    void start_worker();

   private:
    QWidget *msg_widget();

    OpenAutoWorker *worker = nullptr;
};

#endif
