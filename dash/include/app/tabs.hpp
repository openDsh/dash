#ifndef TABS_HPP_
#define TABS_HPP_

#include <QPoint>
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
#include <iostream>
#include <regex>
#include <thread>

#include <QtDBus/QtDBus>

#include <BluezQt/Device>

#include <vector>

#include "app/bluetooth.hpp"
#include "app/config.hpp"
#include "app/theme.hpp"
#include "app/window.hpp"
#include "obd/obd.hpp"

namespace aasdk = f1x::aasdk;
namespace autoapp = f1x::openauto::autoapp;
using ThreadPool = std::vector<std::thread>;

class QTabWidget;
class DashMainWindow;
class OpenAutoTab;
class DataTab;

class OpenAutoWorker {
   public:
    // make this take a generic tab class (all tabs based off this class)
    OpenAutoWorker(QWidget *parent = nullptr, std::function<void(bool)> callback = nullptr);

    ~OpenAutoWorker();

    void start();

    inline void setOpacity(unsigned int alpha) { this->service_factory.setOpacity(alpha); }

   private:
    QWidget *active_area;
    std::function<void(bool)> active_callback;

    libusb_context *usb_context;

    boost::asio::io_service io_service;
    boost::asio::io_service::work work;
    std::shared_ptr<autoapp::configuration::Configuration> configuration;
    // autoapp::configuration::RecentAddressesList recent_addresses;
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

    void init_usb_context();

    void create_usb_workers();
    void create_io_service_workers();
};

class OpenAutoTab : public QWidget {
    Q_OBJECT

   public:
    explicit OpenAutoTab(QWidget *parent = 0);

    void start_worker();

    void toggle(bool active) { this->mainLayout->setCurrentIndex((active) ? 1 : 0); }

   private slots:
    void toggle_open_auto(unsigned int);

   private:
    QStackedLayout *mainLayout;

    DashMainWindow *app;
    OpenAutoWorker *worker = nullptr;
};

class MediaTab : public QWidget {
    Q_OBJECT

   public:
    explicit MediaTab(QWidget *parent = 0);

   private slots:
    void toggle_play(bool checked = false);
    void back();
    void forward();
    void update_media_player(QString, BluezQt::MediaPlayerPtr);
    void update_media_player_status(BluezQt::MediaPlayer::Status);
    void update_media_player_track(BluezQt::MediaPlayerTrack);

   private:
    Theme *theme;
    Bluetooth *bluetooth;
    QTabWidget *tabWidget;
    QPushButton *play_button;
    QString media_player_path;
    QDBusInterface *media_player_interface = nullptr;

    QLabel *artist;
    QLabel *album;
    QLabel *title;

    void set_media_stats();
};

class Gauge : public QWidget {
    Q_OBJECT

   public:
    explicit Gauge(QString unit, int pos, int font_v, int font_u, int refresh_rate,
                   std::vector<Command<double>> commands, int precision,
                   std::function<double(std::vector<double>, bool)> result, bool si, QString alt_unit = QString(),
                   QWidget *parent = 0);

    void convert(bool);
    void update_value();
    inline void start() { this->timer->start(this->refresh_rate); }
    inline void stop() { this->timer->stop(); }

   private slots:
    void update_gauge();

   private:
    DataTab *tab;

    QString unit;
    QString alt_unit;

    bool si;

    QLabel *value_label;
    QLabel *unit_label;

    QTimer *timer;
    int refresh_rate;

    std::vector<Command<double>> commands;
    std::function<double(std::vector<double>, bool)> result;
    int precision;
};

class DataTab : public QWidget {
    Q_OBJECT

   public:
    explicit DataTab(QWidget *parent = 0);

    void enable_updates();

    void disable_updates();

    void convert_gauges(bool);

   private slots:
    void toggle_updates(bool);

   private:
    OBD *obd;

    DashMainWindow *app;

    QFrame *obd_status;

    std::vector<Gauge *> gauges;
};

class SettingsTab : public QWidget {
    Q_OBJECT

   public:
    explicit SettingsTab(QWidget *parent = 0);

   private slots:
    void bluetooth_device_added(BluezQt::DevicePtr);
    void bluetooth_device_changed(BluezQt::DevicePtr);
    void bluetooth_device_removed(BluezQt::DevicePtr);
    void media_player_changed(QString, BluezQt::MediaPlayerPtr);

   signals:
    void brightness_updated(int);
    void si_units_changed(bool);

   private:
    DashMainWindow *app;

    Config *config;
    Theme *theme;
    Bluetooth *bluetooth;
    QSlider *brightness_control;
    bool dark_mode = true;

    QMap<BluezQt::DevicePtr, QPushButton *> bluetooth_device_buttons;
    QVBoxLayout *bluetooth_devices;
    QLabel *media_player;
};

#endif
