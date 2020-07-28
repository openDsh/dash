#pragma once

#include <QtWidgets>
#include <thread>

#include "aasdk/TCP/TCPWrapper.hpp"
#include "aasdk/USB/AccessoryModeQueryChain.hpp"
#include "aasdk/USB/AccessoryModeQueryChainFactory.hpp"
#include "aasdk/USB/AccessoryModeQueryFactory.hpp"
#include "aasdk/USB/ConnectedAccessoriesEnumerator.hpp"
#include "aasdk/USB/USBHub.hpp"
#include "app/config.hpp"
#include "app/bluetooth.hpp"
#include "app/widgets/switch.hpp"
#include "app/widgets/dialog.hpp"
#include "app/theme.hpp"
#include "openauto/App.hpp"
#include "openauto/Configuration/Configuration.hpp"
#include "openauto/Configuration/IConfiguration.hpp"
#include "openauto/Configuration/RecentAddressesList.hpp"
#include "openauto/Service/AndroidAutoEntityFactory.hpp"
#include "openauto/Service/ServiceFactory.hpp"

class OpenAutoWorker : public QObject {
    Q_OBJECT

   public:
    OpenAutoWorker(std::function<void(bool)> callback, bool night_mode, QWidget *frame);
    ~OpenAutoWorker();
    void connect_wireless(QString address);

    inline void start() { this->app->waitForUSBDevice(); }
    inline void update_size() { this->service_factory.resize(); }
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
    OpenAutoFrame(QWidget *parent) : QWidget(parent) {}

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

class OpenAutoSettingsSubTab : public QWidget {
    Q_OBJECT

   public:
    OpenAutoSettingsSubTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *rhd_row_widget();
    QWidget *frame_rate_row_widget();
    QWidget *resolution_row_widget();
    QWidget *dpi_row_widget();
    QWidget *dpi_widget();
    QWidget *rt_audio_row_widget();
    QWidget *audio_channels_row_widget();
    QWidget *bluetooth_row_widget();
    QWidget *touchscreen_row_widget();
    QCheckBox *button_checkbox(QString name, QString key, aasdk::proto::enums::ButtonCode::Enum code, QWidget *parent);
    QWidget *buttons_row_widget();

    Bluetooth *bluetooth;
    Config *config;
    Theme *theme;
};

class OpenAutoTab : public QStackedWidget {
    Q_OBJECT

   public:
    OpenAutoTab(QWidget *parent = nullptr);

    inline void pass_key_event(QKeyEvent *event) { this->worker->send_key_event(event); }

   protected:
    void resizeEvent(QResizeEvent *event);

   private:
    QWidget *connect_msg();
    QWidget *wireless_config();

    Config *config;
    Theme *theme;
    OpenAutoFrame *frame;
    OpenAutoWorker *worker;

   signals:
    void toggle_fullscreen(QWidget *widget);
};
