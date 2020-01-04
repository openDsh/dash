#include <QPoint>
#include <QRadioTuner>
#include <QThread>
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

#include <BluezQt/Device>
#include <BluezQt/PendingCall>

#include "app/bluetooth.hpp"
#include "app/config.hpp"
#include "app/progress.hpp"
#include "app/switch.hpp"
#include "app/tabs.hpp"
#include "app/theme.hpp"
#include "app/tuner.hpp"
#include "app/window.hpp"
#include "obd/conversions.hpp"

#include <stdio.h>
#include <stdlib.h>

namespace aasdk = f1x::aasdk;
namespace autoapp = f1x::openauto::autoapp;
using ThreadPool = std::vector<std::thread>;

QFont f("Montserrat", 18);
QFont ff("Montserrat", 14);
QFont fff("Montserrat", 36);

OpenAutoWorker::OpenAutoWorker(QWidget *parent, std::function<void(bool)> callback)
    : active_area(parent),
      active_callback(callback),
      io_service(),
      work(io_service),
      configuration(Config::get_instance()->open_auto_config),
      //   recent_addresses(7),
      tcp_wrapper(),
      usb_wrapper((init_usb_context(), usb_context)),
      query_factory(usb_wrapper, io_service),
      query_chain_factory(usb_wrapper, io_service, query_factory),
      service_factory(io_service, configuration, active_area, active_callback),
      android_auto_entity_factory(io_service, configuration, service_factory),
      usb_hub(std::make_shared<aasdk::usb::USBHub>(this->usb_wrapper, this->io_service, this->query_chain_factory)),
      connected_accessories_enumerator(std::make_shared<aasdk::usb::ConnectedAccessoriesEnumerator>(
          this->usb_wrapper, this->io_service, this->query_chain_factory)),
      app(std::make_shared<autoapp::App>(this->io_service, this->usb_wrapper, this->tcp_wrapper,
                                         this->android_auto_entity_factory, usb_hub, connected_accessories_enumerator))
{
    this->create_usb_workers();
    this->create_io_service_workers();
}

void OpenAutoWorker::start()
{
    // this->recent_addresses.read();

    // QObject::connect(&this->connect_dialog, &autoapp::ui::ConnectDialog::connectionSucceed, [this](auto socket) {
    //     this->app->start(std::move(socket));
    // });

    this->app->waitForUSBDevice();
}

void OpenAutoWorker::init_usb_context() { libusb_init(&this->usb_context); }

OpenAutoWorker::~OpenAutoWorker()
{
    std::for_each(this->thread_pool.begin(), this->thread_pool.end(),
                  std::bind(&std::thread::join, std::placeholders::_1));
    libusb_exit(this->usb_context);
}

void OpenAutoWorker::create_usb_workers()
{
    auto worker = [this]() {
        timeval event_timeout = {180, 0};
        while (!this->io_service.stopped())
            libusb_handle_events_timeout_completed(this->usb_context, &event_timeout, nullptr);
    };

    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
}

void OpenAutoWorker::create_io_service_workers()
{
    auto worker = [this]() { this->io_service.run(); };

    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
}

OpenAutoTab::OpenAutoTab(QWidget *parent) : QWidget(parent)
{
    this->app = qobject_cast<DashMainWindow *>(parent);

    connect(this->app, SIGNAL(open_auto_tab_toggle(unsigned int)), this, SLOT(toggle_open_auto(unsigned int)));

    this->mainLayout = new QStackedLayout;
    this->mainLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *waiting_widget = new QWidget;
    QVBoxLayout *waiting_layout = new QVBoxLayout;
    waiting_layout->setContentsMargins(24, 24, 24, 24);
    QLabel *waiting = new QLabel("waiting for device...");
    waiting->setFont(f);
    waiting->setAlignment(Qt::AlignHCenter);
    QLabel *plugin = new QLabel("plug in your device to start OpenAuto");
    plugin->setFont(f);
    plugin->setAlignment(Qt::AlignHCenter);
    waiting_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    waiting_layout->addWidget(waiting);
    waiting_layout->addWidget(plugin);
    waiting_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    waiting_widget->setLayout(waiting_layout);
    this->mainLayout->addWidget(waiting_widget);
#ifdef USE_OMX
    QWidget *black = new QWidget;
    black->setStyleSheet("background-color: black;");
    this->mainLayout->addWidget(black);
#endif
    setLayout(this->mainLayout);
}

void OpenAutoTab::start_worker()
{
    auto callback = [this](bool is_active) { this->toggle((is_active) ? 1 : 0); };
    if (this->worker == nullptr) this->worker = new OpenAutoWorker(this, callback);
    this->worker->start();
}

void OpenAutoTab::toggle_open_auto(unsigned int alpha)
{
    if (this->worker != nullptr) this->worker->setOpacity(alpha);
    if (alpha > 0) this->setFocus();
}

MediaTab::MediaTab(QWidget *parent) : QWidget(parent)
{
    this->bluetooth = Bluetooth::get_instance();

    connect(this->bluetooth, SIGNAL(media_player_changed(QString, BluezQt::MediaPlayerPtr)), this,
            SLOT(update_media_player(QString, BluezQt::MediaPlayerPtr)));
    connect(this->bluetooth, SIGNAL(media_player_status_changed(BluezQt::MediaPlayer::Status)), this,
            SLOT(update_media_player_status(BluezQt::MediaPlayer::Status)));
    connect(this->bluetooth, SIGNAL(media_player_track_changed(BluezQt::MediaPlayerTrack)), this,
            SLOT(update_media_player_track(BluezQt::MediaPlayerTrack)));

    auto media_player = this->bluetooth->get_media_player().second;

    this->theme = Theme::get_instance();

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    this->tabWidget = new QTabWidget;
    QWidget *bluetooth_tab = new QWidget;
    this->tabWidget->tabBar()->setFont(f);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(24, 24, 24, 24);
    QHBoxLayout *layout2 = new QHBoxLayout;
    QPushButton *back_button = new QPushButton;
    back_button->setFlat(true);
    back_button->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    this->theme->add_button_icon("skip_previous", back_button);
    connect(back_button, SIGNAL(clicked()), this, SLOT(back()));
    layout2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout2->addWidget(back_button);
    layout2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    this->play_button = new QPushButton;
    this->play_button->setFlat(true);
    this->play_button->setCheckable(true);
    this->play_button->setChecked(
        (media_player != nullptr) ? media_player->status() == BluezQt::MediaPlayer::Status::Playing : false);
    this->play_button->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    this->theme->add_button_icon("pause", play_button, "play");
    connect(this->play_button, SIGNAL(clicked(bool)), this, SLOT(toggle_play(bool)));
    layout2->addWidget(this->play_button);
    QPushButton *forward_button = new QPushButton;
    forward_button->setFlat(true);
    forward_button->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    this->theme->add_button_icon("skip_next", forward_button);

    connect(forward_button, SIGNAL(clicked()), this, SLOT(forward()));
    layout2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout2->addWidget(forward_button);
    layout2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QVBoxLayout *layout3 = new QVBoxLayout;
    QLabel *ar = new QLabel("Artist");
    ar->setFont(f);
    this->artist = new QLabel((media_player != nullptr) ? media_player->track().artist() : QString());
    this->artist->setFont(ff);
    this->artist->setStyleSheet("padding-left: 16px;");
    QLabel *al = new QLabel("Album");
    al->setFont(f);
    album = new QLabel((media_player != nullptr) ? media_player->track().album() : QString());
    this->album->setFont(ff);
    this->album->setStyleSheet("padding-left: 16px;");
    QLabel *ti = new QLabel("Title");
    ti->setFont(f);
    this->title = new QLabel((media_player != nullptr) ? media_player->track().title() : QString());
    this->title->setFont(ff);
    this->title->setStyleSheet("padding-left: 16px;");
    layout3->addWidget(ar);
    layout3->addWidget(this->artist);
    layout3->addWidget(al);
    layout3->addWidget(this->album);
    layout3->addWidget(ti);
    layout3->addWidget(this->title);

    layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addLayout(layout3);
    layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addLayout(layout2);
    layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QWidget *radio = new QWidget;

    Config *config = Config::get_instance();

    QVBoxLayout *radio_layout = new QVBoxLayout;
    radio->setContentsMargins(24, 24, 24, 24);
    Tuner *tuner = new Tuner(config->get_radio_station());
    tuner->setFont(ff);

    QLabel *tuner_value = new QLabel(QString::number(tuner->sliderPosition() / 10.0, 'f', 1));
    tuner_value->setAlignment(Qt::AlignCenter);
    tuner_value->setFont(fff);

    connect(tuner, &Tuner::station_updated, [tuner_value, config](int station) {
        tuner_value->setText(QString::number(station / 10.0, 'f', 1));
        config->set_radio_station(station);
    });

    radio_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    radio_layout->addWidget(tuner_value);
    radio_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    radio_layout->addWidget(tuner);
    radio_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QHBoxLayout *controls = new QHBoxLayout;
    QPushButton *scan_reverse = new QPushButton;
    scan_reverse->setFlat(true);
    scan_reverse->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    this->theme->add_button_icon("fast_rewind", scan_reverse);
    connect(scan_reverse, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 5); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(scan_reverse);
    QPushButton *prev_station = new QPushButton;
    prev_station->setFlat(true);
    prev_station->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    this->theme->add_button_icon("skip_previous", prev_station);
    connect(prev_station, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 1); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(prev_station);
    QPushButton *next_station = new QPushButton;
    next_station->setFlat(true);
    next_station->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    this->theme->add_button_icon("skip_next", next_station);
    connect(next_station, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() + 1); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(next_station);
    QPushButton *scan_forward = new QPushButton;
    scan_forward->setFlat(true);
    scan_forward->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    this->theme->add_button_icon("fast_forward", scan_forward);
    connect(scan_forward, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() + 5); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(scan_forward);
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QFrame *line = new QFrame;
    line->setLineWidth(1);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Plain);
    controls->addWidget(line);
    QPushButton *mute = new QPushButton;
    mute->setFlat(true);
    mute->setCheckable(true);
    mute->setChecked(config->get_radio_muted());
    mute->setIconSize(QSize(56 * RESOLUTION, 56 * RESOLUTION));
    connect(mute, &QPushButton::clicked, [config](bool checked = false) { config->set_radio_muted(checked); });
    this->theme->add_button_icon("volume_off", mute);
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(mute);
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

    radio_layout->addLayout(controls);
    radio_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    radio->setLayout(radio_layout);

    this->tabWidget->addTab(radio, "Radio");

    bluetooth_tab->setLayout(layout);
    this->tabWidget->addTab(bluetooth_tab, "Bluetooth");

    bool active = media_player != nullptr;
    this->tabWidget->setTabEnabled(1, active);
    this->tabWidget->setCurrentIndex(active ? 1 : 0);

    main_layout->addWidget(this->tabWidget);
    setLayout(main_layout);
}

void MediaTab::update_media_player(QString name, BluezQt::MediaPlayerPtr media_player)
{
    bool active = media_player != nullptr;
    this->tabWidget->setTabEnabled(1, active);
    this->tabWidget->setCurrentIndex(active ? 1 : 0);
}

void MediaTab::update_media_player_status(BluezQt::MediaPlayer::Status status)
{
    this->play_button->setChecked(status == BluezQt::MediaPlayer::Status::Playing);
}

void MediaTab::update_media_player_track(BluezQt::MediaPlayerTrack track)
{
    static QString artist, album, title;
    if (!track.artist().isEmpty() || !track.isValid()) artist = track.artist();
    if (!track.album().isEmpty() || !track.isValid()) album = track.album();
    if (!track.title().isEmpty() || !track.isValid()) title = track.title();

    this->artist->setText(artist);
    this->album->setText(album);
    this->title->setText(title);
}

void MediaTab::toggle_play(bool checked)
{
    this->play_button->setChecked(!checked);

    auto media_player = this->bluetooth->get_media_player().second;
    if (media_player != nullptr) {
        if (checked)
            media_player->play()->waitForFinished();
        else
            media_player->pause()->waitForFinished();
    }
}

void MediaTab::back()
{
    auto media_player = this->bluetooth->get_media_player().second;
    if (media_player != nullptr) media_player->previous()->waitForFinished();
}

void MediaTab::forward()
{
    auto media_player = this->bluetooth->get_media_player().second;
    if (media_player != nullptr) media_player->next()->waitForFinished();
}

DataTab::DataTab(QWidget *parent) : QWidget(parent)
{
    this->app = qobject_cast<DashMainWindow *>(parent);

    this->obd = OBD::get_instance();

    connect(this->app, SIGNAL(data_tab_toggle(bool)), this, SLOT(toggle_updates(bool)));

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    this->obd_status = new QFrame();
    this->obd_status->setFixedHeight(4 * RESOLUTION);
    this->obd_status->setAutoFillBackground(true);
    main_layout->addWidget(this->obd_status);

    QWidget *right_w = new QWidget;
    QVBoxLayout *right = new QVBoxLayout(right_w);

    Config *config = Config::get_instance();
    bool si = config->get_si_units();

    this->gauges.push_back(new Gauge("°F", 0, 48, 16, 5000, {commands.COOLANT_TEMP}, 1,
                                     [](std::vector<double> x, bool si) { return (si) ? x[0] : c_to_f(x[0]); }, si,
                                     "°C"));
    this->gauges.push_back(
        new Gauge("%", 0, 48, 16, 500, {commands.LOAD}, 1, [](std::vector<double> x, bool _) { return x[0]; }, si));

    QFont description_font("Montserrat", 16, QFont::Light);

    right->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    right->addWidget(this->gauges[0]);
    QLabel *coolant_temp_description = new QLabel("coolant");
    coolant_temp_description->setFont(description_font);
    coolant_temp_description->setAlignment(Qt::AlignHCenter);
    coolant_temp_description->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    right->addWidget(coolant_temp_description);
    QFrame *line2 = new QFrame;
    line2->setLineWidth(1);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Plain);
    right->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    right->addWidget(line2);
    right->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    right->addWidget(this->gauges[1]);
    QLabel *load_description = new QLabel("load");
    load_description->setFont(description_font);
    load_description->setAlignment(Qt::AlignHCenter);
    load_description->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    right->addWidget(load_description);
    right->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QWidget *drive_w = new QWidget;
    QHBoxLayout *drive = new QHBoxLayout(drive_w);

    this->gauges.push_back(new Gauge("mph", 1, 92, 24, 100, {commands.SPEED}, 0,
                                     [](std::vector<double> x, bool si) { return (si) ? kph_to_mph(x[0]) : x[0]; }, si,
                                     "km/h"));
    this->gauges.push_back(new Gauge("x1000rpm", 1, 92, 24, 100, {commands.RPM}, 1,
                                     [](std::vector<double> x, bool _) { return x[0] / 1000.0; }, si));
    drive->addWidget(this->gauges[2]);
    drive->addWidget(this->gauges[3]);

    QWidget *left_w = new QWidget;
    QVBoxLayout *left = new QVBoxLayout(left_w);
    this->gauges.push_back(new Gauge("mpg", 1, 48, 16, 100, {commands.SPEED, commands.MAF}, 1,
                                     [](std::vector<double> x, bool si) {
                                         return ((si) ? x[0] : kph_to_mph(x[0])) /
                                                ((si) ? gps_to_lph(x[1]) : gps_to_gph(x[1]));
                                     },
                                     si, "km/L"));

    left->addWidget(drive_w);
    QFrame *line3 = new QFrame;
    line3->setLineWidth(1);
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Plain);
    left->addWidget(line3);
    left->addWidget(this->gauges[4]);

    QWidget *tab = new QWidget;
    QHBoxLayout *data_stuff = new QHBoxLayout(tab);
    data_stuff->setContentsMargins(24, 24, 24, 24);
    data_stuff->addWidget(left_w);
    QFrame *line = new QFrame;
    line->setLineWidth(1);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Plain);
    data_stuff->addWidget(line);
    data_stuff->addWidget(right_w);

    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch(5);
    left_w->setSizePolicy(sp_left);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(2);
    right_w->setSizePolicy(sp_right);

    main_layout->addWidget(tab);

    setLayout(main_layout);
}

void DataTab::toggle_updates(bool toggle)
{
    if (toggle)
        enable_updates();
    else
        disable_updates();
}

void DataTab::enable_updates()
{
    QString status_color = "rgb(211, 47, 47)";
    if (this->obd->is_connected() || std::atoi(std::getenv("DEBUG"))) {
        status_color = "rgb(56, 142, 60)";
        for (auto &x : this->gauges) x->start();
    }

    this->obd_status->setStyleSheet("background-color: " + status_color);
}

void DataTab::disable_updates()
{
    for (auto &x : this->gauges) x->stop();
}

void DataTab::convert_gauges(bool si)
{
    for (auto &x : this->gauges) x->convert(si);
}

SettingsTab::SettingsTab(QWidget *parent) : QWidget(parent)
{
    this->app = qobject_cast<DashMainWindow *>(parent);
    this->config = Config::get_instance();

    this->theme = Theme::get_instance();
    this->bluetooth = Bluetooth::get_instance();

    connect(this->bluetooth, SIGNAL(media_player_changed(QString, BluezQt::MediaPlayerPtr)), this,
            SLOT(media_player_changed(QString, BluezQt::MediaPlayerPtr)));

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(24, 24, 24, 24);
    Switch *dark_mode = new Switch;
    dark_mode->setChecked(this->config->get_dark_mode());
    connect(dark_mode, &Switch::stateChanged, [this](bool state) {
        this->theme->set_mode(state);
        this->config->set_dark_mode(state);
    });
    QLabel *l;
    l = new QLabel("Dark Mode");
    l->setFont(f);
    QHBoxLayout *ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(dark_mode, 1, Qt::AlignHCenter);
    layout->addStretch(1);
    layout->addLayout(ll);
    layout->addStretch(1);
    l = new QLabel("Brightness");
    l->setFont(f);
    QHBoxLayout *bl = new QHBoxLayout;
    QPushButton *bd = new QPushButton;
    bd->setFlat(true);
    bd->setIconSize(QSize(36 * RESOLUTION, 36 * RESOLUTION));
    this->theme->add_button_icon("brightness_low", bd);
    connect(bd, &QPushButton::clicked, this, [this] {
        int position = this->brightness_control->sliderPosition() - 18;
        this->brightness_control->setSliderPosition(position);
        emit brightness_updated(position);
    });
    QPushButton *bu = new QPushButton();
    bu->setFlat(true);
    bu->setIconSize(QSize(36 * RESOLUTION, 36 * RESOLUTION));
    this->theme->add_button_icon("brightness_high", bu);
    connect(bu, &QPushButton::clicked, this, [this] {
        int position = this->brightness_control->sliderPosition() + 18;
        this->brightness_control->setSliderPosition(position);
        emit brightness_updated(position);
    });
    this->brightness_control = new QSlider(Qt::Orientation::Horizontal);
    this->brightness_control->setRange(76, 255);
    this->brightness_control->setSliderPosition(this->config->get_brightness());
    connect(this->brightness_control, &QSlider::sliderMoved,
            [this](int position) { emit brightness_updated(position); });
    connect(this->brightness_control, &QSlider::valueChanged,
            [this](int position) { this->config->set_brightness(position); });
    bl->addStretch(1);
    bl->addWidget(bd);
    bl->addWidget(this->brightness_control, 6);
    bl->addWidget(bu);
    bl->addStretch(1);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addLayout(bl, 1);
    layout->addLayout(ll);
    layout->addStretch(1);
    Switch *units = new Switch;
    units->setChecked(this->config->get_si_units());
    connect(units, &Switch::stateChanged, [this](bool state) {
        emit si_units_changed(state);
        this->config->set_si_units(state);
    });
    l = new QLabel("SI Units");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(units, 1, Qt::AlignHCenter);
    layout->addLayout(ll);
    layout->addStretch(1);
    l = new QLabel("Color");
    l->setFont(f);
    QComboBox *box = new QComboBox;
    box->setItemDelegate(new QStyledItemDelegate());
    box->setFont(ff);

    auto colors = this->theme->get_colors();
    QMap<QString, QColor>::iterator i;
    QPixmap pixmap(16, 16);
    for (i = colors.begin(); i != colors.end(); i++) {
        pixmap.fill(i.value());
        box->addItem(QIcon(pixmap), i.key());
    }
    box->setCurrentText(this->config->get_color());

    connect(box, QOverload<const QString &>::of(&QComboBox::activated), [this](const QString &color) {
        this->theme->set_color(color);
        this->config->set_color(color);
    });
    connect(this->theme, &Theme::color_updated, [box](QMap<QString, QColor> &colors) {
        QMap<QString, QColor>::iterator i;
        QPixmap pixmap(16, 16);
        int idx;
        for (i = colors.begin(); i != colors.end(); i++) {
            idx = box->findText(i.key());
            pixmap.fill(i.value());
            box->setItemIcon(idx, QIcon(pixmap));
        }
    });

    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    bl = new QHBoxLayout;
    bl->addStretch(1);
    bl->addWidget(box, 6);
    bl->addStretch(1);
    ll->addLayout(bl, 1);
    layout->addLayout(ll);
    layout->addStretch(1);
    QPushButton *save = new QPushButton("save");
    save->setFont(f);
    save->setFlat(true);
    save->setIconSize(QSize(48 * RESOLUTION, 48 * RESOLUTION));
    this->theme->add_button_icon("save", save);
    connect(save, &QPushButton::clicked, [this]() { this->config->save(); });
    ll = new QHBoxLayout;
    ll->addStretch(1);
    ll->addWidget(save);
    layout->addLayout(ll);
    tab->setLayout(layout);

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->tabBar()->setFont(f);
    tabWidget->addTab(tab, "General");

    QWidget *bw = new QWidget;
    QHBoxLayout *holder = new QHBoxLayout;
    holder->setContentsMargins(24, 24, 24, 24);
    QVBoxLayout *left_layout = new QVBoxLayout;
    QHBoxLayout *lleft_layout = new QHBoxLayout;
    QPushButton *scan = new QPushButton("scan");
    scan->setFont(f);
    scan->setFlat(true);
    scan->setIconSize(QSize(48 * RESOLUTION, 48 * RESOLUTION));
    this->theme->add_button_icon("bluetooth_searching", scan);
    connect(scan, &QPushButton::clicked, [this]() { this->bluetooth->scan(); });
    lleft_layout->addWidget(scan, 0, Qt::AlignLeft);
    ProgressIndicator *waiting = new ProgressIndicator;
    connect(this->bluetooth, &Bluetooth::scan_status, [scan, waiting](bool status) {
        scan->setEnabled(!status);
        if (status)
            waiting->startAnimation();
        else
            waiting->stopAnimation();
    });
    lleft_layout->addWidget(waiting);
    QLabel *mp = new QLabel("Media Player");
    mp->setFont(f);
    auto media_player_name = this->bluetooth->get_media_player().first;
    this->media_player = new QLabel((media_player_name.isEmpty()) ? "not connected" : media_player_name);
    this->media_player->setStyleSheet("padding-left: 16px;");
    this->media_player->setFont(ff);
    left_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    left_layout->addWidget(mp);
    left_layout->addWidget(this->media_player);
    left_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    left_layout->addLayout(lleft_layout);
    this->bluetooth_devices = new QVBoxLayout;
    for (auto device : this->bluetooth->get_devices()) {
        if (device->address() == this->config->get_bluetooth_device()) device->connectToDevice()->waitForFinished();
        QPushButton *device_button = new QPushButton(device->name());
        device_button->setFont(f);
        device_button->setCheckable(true);
        if (device->isConnected()) device_button->setChecked(true);
        connect(device_button, &QPushButton::clicked, [this, device_button, device](bool checked = false) {
            device_button->setChecked(!checked);
            if (checked) {
                device->connectToDevice()->waitForFinished();
                this->config->set_bluetooth_device(device->address());
            }
            else {
                device->disconnectFromDevice()->waitForFinished();
                this->config->set_bluetooth_device(QString());
            }
        });

        this->bluetooth_device_buttons[device] = device_button;
        this->bluetooth_devices->addWidget(device_button);
    };
    holder->addLayout(left_layout);
    holder->addLayout(this->bluetooth_devices);
    bw->setLayout(holder);

    QWidget *oaw = new QWidget;
    layout = new QVBoxLayout;
    layout->setContentsMargins(24, 24, 24, 24);
    Switch *handedness = new Switch;
    handedness->setChecked(this->config->open_auto_config->getHandednessOfTrafficType() ==
                           autoapp::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE);
    connect(handedness, &Switch::stateChanged, [this](bool state) {
        this->config->open_auto_config->setHandednessOfTrafficType(
            state ? autoapp::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE
                  : autoapp::configuration::HandednessOfTrafficType::LEFT_HAND_DRIVE);
    });
    l = new QLabel("Right-Hand-Drive");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(handedness, 1, Qt::AlignHCenter);
    layout->addLayout(ll);
    QFrame *line = new QFrame;
    line->setLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    layout->addWidget(line);
    l = new QLabel("Frame Rate");
    l->setFont(f);
    QGroupBox *groupBox = new QGroupBox;
    QRadioButton *radio1 = new QRadioButton("30fps", groupBox);
    radio1->setFont(ff);
    radio1->setChecked(this->config->open_auto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    connect(radio1, &QRadioButton::clicked,
            [this]() { this->config->open_auto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_30); });
    QRadioButton *radio2 = new QRadioButton("60fps", groupBox);
    radio2->setFont(ff);
    radio2->setChecked(this->config->open_auto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_60);
    radio1->setChecked(this->config->open_auto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    connect(radio2, &QRadioButton::clicked,
            [this]() { this->config->open_auto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_60); });
    QVBoxLayout *lll = new QVBoxLayout;
    lll->addWidget(radio1);
    lll->addWidget(radio2);
    groupBox->setLayout(lll);
    ll = new QHBoxLayout;
    ll->addWidget(l);
    ll->addWidget(groupBox, 0, Qt::AlignHCenter);
    layout->addLayout(ll);
    l = new QLabel("Resolution");
    l->setFont(f);
    groupBox = new QGroupBox;
    radio1 = new QRadioButton("480p", groupBox);
    radio1->setFont(ff);
    radio1->setChecked(this->config->open_auto_config->getVideoResolution() ==
                       aasdk::proto::enums::VideoResolution::_480p);
    connect(radio1, &QRadioButton::clicked, [this]() {
        this->config->open_auto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_480p);
    });
    radio2 = new QRadioButton("720p", groupBox);
    radio2->setFont(ff);
    radio2->setChecked(this->config->open_auto_config->getVideoResolution() ==
                       aasdk::proto::enums::VideoResolution::_720p);
    connect(radio2, &QRadioButton::clicked, [this]() {
        this->config->open_auto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_720p);
    });
    QRadioButton *radio3 = new QRadioButton("1080p", groupBox);
    radio3->setFont(ff);
    radio3->setChecked(this->config->open_auto_config->getVideoResolution() ==
                       aasdk::proto::enums::VideoResolution::_1080p);
    connect(radio3, &QRadioButton::clicked, [this]() {
        this->config->open_auto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_1080p);
    });
    lll = new QVBoxLayout;
    lll->addWidget(radio1);
    lll->addWidget(radio2);
    lll->addWidget(radio3);
    groupBox->setLayout(lll);
    ll = new QHBoxLayout;
    ll->addWidget(l);
    ll->addWidget(groupBox, 0, Qt::AlignHCenter);
    layout->addLayout(ll);
    l = new QLabel("DPI");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    bl = new QHBoxLayout;
    QSlider *dpi = new QSlider(Qt::Orientation::Horizontal);
    dpi->setRange(0, 512);
    dpi->setSliderPosition(this->config->open_auto_config->getScreenDPI());
    bl->addStretch(1);
    bl->addWidget(dpi, 4);
    l = new QLabel(QString::number(dpi->sliderPosition()));
    l->setFont(ff);
    bl->addWidget(l, 1, Qt::AlignHCenter);
    bl->addStretch(1);
    ll->addLayout(bl, 1);
    layout->addLayout(ll);
    connect(dpi, &QSlider::valueChanged, [this, l](int value) {
        l->setText(QString::number(value));
        this->config->open_auto_config->setScreenDPI(value);
    });
    line = new QFrame;
    line->setLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    layout->addWidget(line);
    Switch *audio_backend = new Switch;
    audio_backend->setChecked(this->config->open_auto_config->getAudioOutputBackendType() ==
                              autoapp::configuration::AudioOutputBackendType::RTAUDIO);
    connect(audio_backend, &Switch::stateChanged, [this](bool state) {
        this->config->open_auto_config->setAudioOutputBackendType(
            state ? autoapp::configuration::AudioOutputBackendType::RTAUDIO
                  : autoapp::configuration::AudioOutputBackendType::QT);
    });
    l = new QLabel("RtAudio");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(audio_backend, 1, Qt::AlignHCenter);
    layout->addLayout(ll);
    l = new QLabel("Audio Channels");
    l->setFont(f);
    groupBox = new QGroupBox;
    QCheckBox *checkBox1 = new QCheckBox("Music");
    checkBox1->setFont(ff);
    checkBox1->setChecked(this->config->open_auto_config->musicAudioChannelEnabled());
    connect(checkBox1, &QCheckBox::toggled,
            [this](bool checked) { this->config->open_auto_config->setMusicAudioChannelEnabled(checked); });
    QCheckBox *checkBox2 = new QCheckBox("Speech");
    checkBox2->setFont(ff);
    checkBox2->setChecked(this->config->open_auto_config->speechAudioChannelEnabled());
    connect(checkBox2, &QCheckBox::toggled,
            [this](bool checked) { this->config->open_auto_config->setSpeechAudioChannelEnabled(checked); });
    lll = new QVBoxLayout;
    lll->addWidget(checkBox1);
    lll->addWidget(checkBox2);
    groupBox->setLayout(lll);
    ll = new QHBoxLayout;
    ll->addWidget(l);
    ll->addWidget(groupBox, 0, Qt::AlignHCenter);
    layout->addLayout(ll);
    line = new QFrame;
    line->setLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    layout->addWidget(line);
    Switch *bluetooth = new Switch;
    bluetooth->setChecked(this->config->open_auto_config->getBluetoothAdapterType() ==
                          autoapp::configuration::BluetoothAdapterType::LOCAL);
    connect(bluetooth, &Switch::stateChanged, [this](bool state) {
        this->config->open_auto_config->setBluetoothAdapterType(
            state ? autoapp::configuration::BluetoothAdapterType::LOCAL
                  : autoapp::configuration::BluetoothAdapterType::NONE);
    });
    l = new QLabel("Bluetooth");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(bluetooth, 1, Qt::AlignHCenter);
    layout->addLayout(ll);
    /* l = new QLabel("Wireless");
    l->setFont(f);
    box = new QComboBox;
    box->setItemDelegate(new QStyledItemDelegate());
    box->setFont(ff);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    bl = new QHBoxLayout;
    bl->addStretch(1);
    bl->addWidget(box, 6);
    bl->addStretch(1);
    ll->addLayout(bl, 1);
    layout->addLayout(ll); */
    oaw->setLayout(layout);

    connect(this->bluetooth, SIGNAL(device_added(BluezQt::DevicePtr)), this,
            SLOT(bluetooth_device_added(BluezQt::DevicePtr)));
    connect(this->bluetooth, SIGNAL(device_changed(BluezQt::DevicePtr)), this,
            SLOT(bluetooth_device_changed(BluezQt::DevicePtr)));
    connect(this->bluetooth, SIGNAL(device_removed(BluezQt::DevicePtr)), this,
            SLOT(bluetooth_device_removed(BluezQt::DevicePtr)));

    tabWidget->addTab(bw, "Bluetooth");
    tabWidget->addTab(oaw, "OpenAuto");
    main_layout->addWidget(tabWidget);

    setLayout(main_layout);
}

void SettingsTab::bluetooth_device_added(BluezQt::DevicePtr device)
{
    QPushButton *device_button = new QPushButton(device->name());
    device_button->setFont(f);
    device_button->setCheckable(true);
    if (device->isConnected()) device_button->setChecked(true);
    connect(device_button, &QPushButton::clicked, [device_button, device](bool checked = false) {
        device_button->setChecked(!checked);
        if (checked)
            device->connectToDevice()->waitForFinished();
        else
            device->disconnectFromDevice()->waitForFinished();
    });
    this->bluetooth_device_buttons[device] = device_button;
    this->bluetooth_devices->addWidget(device_button);
}

void SettingsTab::bluetooth_device_changed(BluezQt::DevicePtr device)
{
    this->bluetooth_device_buttons[device]->setText(device->name());
    this->bluetooth_device_buttons[device]->setChecked(device->isConnected());
}

void SettingsTab::bluetooth_device_removed(BluezQt::DevicePtr device)
{
    this->bluetooth_devices->removeWidget(this->bluetooth_device_buttons[device]);
    delete this->bluetooth_device_buttons[device];
    this->bluetooth_device_buttons.remove(device);
}

void SettingsTab::media_player_changed(QString name, BluezQt::MediaPlayerPtr)
{
    this->media_player->setText((name.isEmpty()) ? "not connected" : name);
}

Gauge::Gauge(QString unit, int pos, int font_v, int font_u, int refresh_rate, std::vector<Command<double>> commands,
             int precision, std::function<double(std::vector<double>, bool)> result, bool si, QString alt_unit,
             QWidget *parent)
    : QWidget(parent)
{
    this->tab = qobject_cast<DataTab *>(parent);

    this->unit = unit;
    this->alt_unit = alt_unit;
    this->si = si;

    this->commands = commands;
    this->result = result;
    this->precision = precision;

    this->refresh_rate = refresh_rate;

    this->timer = new QTimer;
    connect(this->timer, SIGNAL(timeout()), this, SLOT(update_gauge()));

    QBoxLayout *layout;

    QSpacerItem *spacer;
    Qt::Alignment alignment;
    if (pos) {
        layout = new QVBoxLayout(this);
        spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        alignment = Qt::AlignHCenter;
    }
    else {
        layout = new QHBoxLayout(this);
        spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding);
        alignment = Qt::AlignVCenter;
    }

    this->value_label = new QLabel(QString::number(0, 'f', this->precision));
    QFont value_font("Titillium Web", font_v);
    this->value_label->setFont(value_font);
    this->value_label->setAlignment(alignment);

    this->unit_label = new QLabel((this->si) ? this->alt_unit : this->unit);
    QFont unit_font("Montserrat", font_u, QFont::Light, true);
    this->unit_label->setFont(unit_font);
    this->unit_label->setAlignment(alignment);

    layout->addSpacerItem(spacer);
    layout->addWidget(this->value_label);
    layout->addWidget(this->unit_label);
    if (pos) layout->addSpacerItem(spacer);

    setLayout(layout);
}

void Gauge::convert(bool si)
{
    this->si = si;
    if (!this->alt_unit.isNull()) {
        this->unit_label->setText((this->si) ? this->alt_unit : this->unit);
        this->update_gauge();
    }
}

void Gauge::update_gauge()
{
    double val;

    std::vector<double> results;
    for (auto &x : this->commands) {
        if (std::atoi(std::getenv("DEBUG")))
            val = rand() % 256;
        else if (!OBD::get_instance()->query<double>(x, val))
            return;
        results.push_back(val);
    }

    val = this->result(results, this->si);

    QString label;
    if (this->precision == 0)
        label = QString::number((int)val);
    else
        label = QString::number(val, 'f', this->precision);
    this->value_label->setText(label);
}
