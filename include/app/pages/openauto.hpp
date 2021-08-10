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
#include "app/widgets/switch.hpp"
#include "app/widgets/dialog.hpp"
#include "openauto/App.hpp"
#include "openauto/Configuration/Configuration.hpp"
#include "openauto/Configuration/IConfiguration.hpp"
#include "openauto/Configuration/RecentAddressesList.hpp"
#include "openauto/Service/AndroidAutoEntityFactory.hpp"
#include "openauto/Service/ServiceFactory.hpp"

#include "app/pages/page.hpp"

#include "DashLog.hpp"

class Arbiter;

class OpenAutoWorker : public QObject {
    Q_OBJECT

   public:
    OpenAutoWorker(std::function<void(bool)> callback, bool night_mode, QWidget *frame);
    ~OpenAutoWorker();

    inline void start() { this->app->waitForDevice(true); }
    inline void set_opacity(unsigned int alpha) { this->service_factory.setOpacity(alpha); }
    inline void update_size() { this->service_factory.resize(); }
    inline void set_night_mode(bool mode) { this->service_factory.setNightMode(mode); }
    inline void send_key_event(QKeyEvent *event) { this->service_factory.sendKeyEvent(event); }
    inline void send_button_press(aasdk::proto::enums::ButtonCode::Enum buttonCode, openauto::projection::WheelDirection wheelDirection) { this->service_factory.sendButtonPress(buttonCode, wheelDirection); };

   private:
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
    std::vector<std::thread> thread_pool;
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

class OpenAutoPage : public QStackedWidget, public Page {
    Q_OBJECT

   public:
    OpenAutoPage(Arbiter &arbiter, QWidget *parent = nullptr);
    inline void pass_key_event(QKeyEvent *event) { this->worker->send_key_event(event); }
    void set_full_screen(bool fullscreen);
    void init() override;
    const QString &connected_icon_name() { return this->connected_icon_name_; }
    inline bool is_connected() { return this->device_connected; }

   protected:
    void resizeEvent(QResizeEvent *event);

   private:
    class Settings : public QWidget {
       public:
        Settings(Arbiter &arbiter, QWidget *parent = nullptr);

       protected:
        QSize sizeHint() const override;

       private:
        QLayout *settings_widget();
        QLayout *rhd_row_widget();
        QLayout *frame_rate_row_widget();
        QLayout *resolution_row_widget();
        QLayout *dpi_row_widget();
        QLayout *dpi_widget();
        QLayout *rt_audio_row_widget();
        QLayout *audio_channels_row_widget();
        QLayout *bluetooth_row_widget();
        QLayout *autoconnect_row_widget();
        QLayout *touchscreen_row_widget();
        QLayout *connected_indicator_widget();
        QLayout *force_aa_fullscreen_widget();
        QCheckBox *button_checkbox(QString name, QString key, aasdk::proto::enums::ButtonCode::Enum code);
        QLayout *buttons_row_widget();

        Arbiter &arbiter;
        Config *config;
    };

    bool device_connected = false;
    const QString connected_icon_name_;
    QWidget *connect_msg();

    Config *config;
    OpenAutoFrame *frame;
    OpenAutoWorker *worker;

   signals:
    void toggle_fullscreen(QWidget *widget);
    void connected(bool yes);
};
