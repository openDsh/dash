#include "app/tabs/openauto.hpp"

#include "app/config.hpp"
#include "app/theme.hpp"
#include "app/widgets/ip_input.hpp"
#include "app/widgets/progress.hpp"
#include "app/window.hpp"

OpenAutoWorker::OpenAutoWorker(std::function<void(bool)> callback, bool night_mode, QWidget *frame)
    : QObject(qApp),
      io_service(),
      work(io_service),
      configuration(Config::get_instance()->openauto_config),
      tcp_wrapper(),
      usb_wrapper((libusb_init(&usb_context), usb_context)),
      query_factory(usb_wrapper, io_service),
      query_chain_factory(usb_wrapper, io_service, query_factory),
      service_factory(io_service, configuration, frame, callback, night_mode),
      android_auto_entity_factory(io_service, configuration, service_factory),
      usb_hub(std::make_shared<aasdk::usb::USBHub>(usb_wrapper, io_service, query_chain_factory)),
      connected_accessories_enumerator(
          std::make_shared<aasdk::usb::ConnectedAccessoriesEnumerator>(usb_wrapper, io_service, query_chain_factory)),
      app(std::make_shared<openauto::App>(io_service, usb_wrapper, tcp_wrapper, android_auto_entity_factory, usb_hub,
                                          connected_accessories_enumerator)),
      socket(std::make_shared<boost::asio::ip::tcp::socket>(io_service))
{
    this->create_usb_workers();
    this->create_io_service_workers();
}

OpenAutoWorker::~OpenAutoWorker()
{
    std::for_each(this->thread_pool.begin(), this->thread_pool.end(),
                  std::bind(&std::thread::join, std::placeholders::_1));
    libusb_exit(this->usb_context);
}

void OpenAutoWorker::connect_wireless(QString address)
{
    try {
        this->tcp_wrapper.asyncConnect(*this->socket, address.toStdString(), this->OPENAUTO_PORT,
                                       [this, address](const boost::system::error_code &ec) {
                                           if (!ec) {
                                               this->app->start(this->socket);
                                               emit wireless_connection_success(address);
                                           }
                                           else {
                                               emit wireless_connection_failure();
                                           }
                                       });
    }
    catch (const boost::system::system_error &se) {
        emit wireless_connection_failure();
    }
}

void OpenAutoWorker::create_usb_workers()
{
    std::function<void()> worker = [this]() {
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
    std::function<void()> worker = [this]() { this->io_service.run(); };

    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
}

void OpenAutoFrame::mouseDoubleClickEvent(QMouseEvent *)
{
    this->toggle_fullscreen();
    emit double_clicked(this->fullscreen);
}

OpenAutoSettingsSubTab::OpenAutoSettingsSubTab(QWidget *parent) : QWidget(parent)
{
    this->bluetooth = Bluetooth::get_instance();
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(this->settings_widget());
}

QWidget *OpenAutoSettingsSubTab::settings_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    layout->addLayout(this->rhd_row_widget(), 1);
    layout->addWidget(Theme::br(), 1);
    layout->addLayout(this->frame_rate_row_widget(), 1);
    layout->addLayout(this->resolution_row_widget(), 1);
    layout->addLayout(this->dpi_row_widget(), 1);
    layout->addWidget(Theme::br(), 1);
    layout->addLayout(this->rt_audio_row_widget(), 1);
    layout->addLayout(this->audio_channels_row_widget(), 1);
    layout->addWidget(Theme::br(), 1);
    layout->addLayout(this->bluetooth_row_widget(), 1);
    layout->addWidget(Theme::br(), 1);
    layout->addLayout(this->touchscreen_row_widget(), 1);
    layout->addLayout(this->buttons_row_widget(), 1);

    QScrollArea *scroll_area = new QScrollArea(this);
    Theme::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QBoxLayout *OpenAutoSettingsSubTab::rhd_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Right-Hand-Drive");
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->openauto_config->getHandednessOfTrafficType() ==
                       openauto::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE);
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setHandednessOfTrafficType(
            state ? openauto::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE
                  : openauto::configuration::HandednessOfTrafficType::LEFT_HAND_DRIVE);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QBoxLayout *OpenAutoSettingsSubTab::frame_rate_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Frame Rate");
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox();
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QRadioButton *fps_30_button = new QRadioButton("30fps", group);
    fps_30_button->setFont(Theme::font_14);
    fps_30_button->setChecked(this->config->openauto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    connect(fps_30_button, &QRadioButton::clicked,
            [config = this->config]() { config->openauto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_30); });
    group_layout->addWidget(fps_30_button);

    QRadioButton *fps_60_button = new QRadioButton("60fps", group);
    fps_60_button->setFont(Theme::font_14);
    fps_60_button->setChecked(this->config->openauto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_60);
    connect(fps_60_button, &QRadioButton::clicked,
            [config = this->config]() { config->openauto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_60); });
    group_layout->addWidget(fps_60_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return layout;
}

QBoxLayout *OpenAutoSettingsSubTab::resolution_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Resolution");
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox();
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QRadioButton *res_480_button = new QRadioButton("480p", group);
    res_480_button->setFont(Theme::font_14);
    res_480_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                               aasdk::proto::enums::VideoResolution::_480p);
    connect(res_480_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_480p);
    });
    group_layout->addWidget(res_480_button);

    QRadioButton *res_720_button = new QRadioButton("720p", group);
    res_720_button->setFont(Theme::font_14);
    res_720_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                               aasdk::proto::enums::VideoResolution::_720p);
    connect(res_720_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_720p);
    });
    group_layout->addWidget(res_720_button);

    QRadioButton *res_1080_button = new QRadioButton("1080p", group);
    res_1080_button->setFont(Theme::font_14);
    res_1080_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                                aasdk::proto::enums::VideoResolution::_1080p);
    connect(res_1080_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_1080p);
    });
    group_layout->addWidget(res_1080_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return layout;
}

QBoxLayout *OpenAutoSettingsSubTab::dpi_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("DPI");
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    layout->addLayout(this->dpi_widget(), 1);

    return layout;
}

QBoxLayout *OpenAutoSettingsSubTab::dpi_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setTracking(false);
    slider->setRange(0, 512);
    slider->setValue(this->config->openauto_config->getScreenDPI());
    QLabel *value = new QLabel(QString::number(slider->value()));
    value->setFont(Theme::font_14);
    connect(slider, &QSlider::valueChanged, [config = this->config, value](int position) {
        config->openauto_config->setScreenDPI(position);
        value->setText(QString::number(position));
    });
    connect(slider, &QSlider::sliderMoved, [value](int position) {
        value->setText(QString::number(position));
    });

    layout->addStretch(2);
    layout->addWidget(slider, 4);
    layout->addWidget(value, 2);

    return layout;
}

QBoxLayout *OpenAutoSettingsSubTab::rt_audio_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("RtAudio");
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->openauto_config->getAudioOutputBackendType() ==
                       openauto::configuration::AudioOutputBackendType::RTAUDIO);
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setAudioOutputBackendType(state
                                                               ? openauto::configuration::AudioOutputBackendType::RTAUDIO
                                                               : openauto::configuration::AudioOutputBackendType::QT);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QBoxLayout *OpenAutoSettingsSubTab::audio_channels_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Audio Channels");
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox();
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QCheckBox *music_button = new QCheckBox("Music", group);
    music_button->setFont(Theme::font_14);
    music_button->setChecked(this->config->openauto_config->musicAudioChannelEnabled());
    connect(music_button, &QCheckBox::toggled,
            [config = this->config](bool checked) { config->openauto_config->setMusicAudioChannelEnabled(checked); });
    group_layout->addWidget(music_button);
    group_layout->addStretch(2);

    QCheckBox *speech_button = new QCheckBox("Speech", group);
    speech_button->setFont(Theme::font_14);
    speech_button->setChecked(this->config->openauto_config->speechAudioChannelEnabled());
    connect(speech_button, &QCheckBox::toggled,
            [config = this->config](bool checked) { config->openauto_config->setSpeechAudioChannelEnabled(checked); });
    group_layout->addWidget(speech_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return layout;
}

QBoxLayout *OpenAutoSettingsSubTab::bluetooth_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Bluetooth");
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->openauto_config->getBluetoothAdapterType() ==
                       openauto::configuration::BluetoothAdapterType::LOCAL);
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setBluetoothAdapterType(state ? openauto::configuration::BluetoothAdapterType::LOCAL
                                                               : openauto::configuration::BluetoothAdapterType::NONE);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QBoxLayout *OpenAutoSettingsSubTab::touchscreen_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Touchscreen");
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->openauto_config->getTouchscreenEnabled());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged,
            [config = this->config](bool state) { config->openauto_config->setTouchscreenEnabled(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QCheckBox *OpenAutoSettingsSubTab::button_checkbox(QString name, QString key,
                                                   aasdk::proto::enums::ButtonCode::Enum code)
{
    QCheckBox *checkbox = new QCheckBox(QString("%1 [%2]").arg(name).arg(key));
    checkbox->setFont(Theme::font_14);
    checkbox->setChecked(std::find(this->config->openauto_button_codes.begin(),
                                   this->config->openauto_button_codes.end(),
                                   code) != this->config->openauto_button_codes.end());
    connect(checkbox, &QCheckBox::toggled, [config = this->config, code](bool checked) {
        if (checked) {
            config->openauto_button_codes.push_back(code);
        }
        else {
            config->openauto_button_codes.erase(
                std::remove(config->openauto_button_codes.begin(), config->openauto_button_codes.end(), code),
                config->openauto_button_codes.end());
        }
    });

    return checkbox;
}

QBoxLayout *OpenAutoSettingsSubTab::buttons_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Buttons");
    label->setFont(Theme::font_16);

    QGroupBox *group = new QGroupBox();
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    group_layout->addWidget(this->button_checkbox("Enter", "Enter", aasdk::proto::enums::ButtonCode::ENTER));
    group_layout->addWidget(this->button_checkbox("Left", "Left", aasdk::proto::enums::ButtonCode::LEFT));
    group_layout->addWidget(this->button_checkbox("Right", "Right", aasdk::proto::enums::ButtonCode::RIGHT));
    group_layout->addWidget(this->button_checkbox("Up", "Up", aasdk::proto::enums::ButtonCode::UP));
    group_layout->addWidget(this->button_checkbox("Down", "Down", aasdk::proto::enums::ButtonCode::DOWN));
    group_layout->addWidget(this->button_checkbox("Back", "Esc", aasdk::proto::enums::ButtonCode::BACK));
    group_layout->addWidget(this->button_checkbox("Home", "H", aasdk::proto::enums::ButtonCode::HOME));
    group_layout->addWidget(this->button_checkbox("Phone", "P", aasdk::proto::enums::ButtonCode::PHONE));
    group_layout->addWidget(this->button_checkbox("Call End", "O", aasdk::proto::enums::ButtonCode::CALL_END));
    group_layout->addWidget(this->button_checkbox("Play", "X", aasdk::proto::enums::ButtonCode::PLAY));
    group_layout->addWidget(this->button_checkbox("Pause", "C", aasdk::proto::enums::ButtonCode::PAUSE));
    group_layout->addWidget(this->button_checkbox("Prev Track", "V", aasdk::proto::enums::ButtonCode::PREV));
    group_layout->addWidget(this->button_checkbox("Next Track", "N", aasdk::proto::enums::ButtonCode::NEXT));
    group_layout->addWidget(this->button_checkbox("Toggle Play", "B", aasdk::proto::enums::ButtonCode::TOGGLE_PLAY));
    group_layout->addWidget(this->button_checkbox("Voice", "M", aasdk::proto::enums::ButtonCode::MICROPHONE_1));
    group_layout->addWidget(this->button_checkbox("Scroll", "1/2", aasdk::proto::enums::ButtonCode::SCROLL_WHEEL));

    layout->addWidget(label, 1);
    layout->addWidget(group, 1, Qt::AlignHCenter);

    return layout;
}

OpenAutoTab::OpenAutoTab(QWidget *parent) : QStackedWidget(parent)
{
    this->config = Config::get_instance();
    this->theme = Theme::get_instance();

    this->frame = new OpenAutoFrame(this);

    std::function<void(bool)> callback = [frame = this->frame](bool active) { frame->toggle(active); };
    this->worker = new OpenAutoWorker(callback, this->theme->get_mode(), frame);
    this->worker->start();

    connect(this->frame, &OpenAutoFrame::toggle, [this](bool enable) {
        if (!enable && this->frame->is_fullscreen()) {
            this->addWidget(frame);
            this->frame->toggle_fullscreen();
        }
        this->setCurrentIndex(enable ? 1 : 0);
    });
    connect(this->frame, &OpenAutoFrame::double_clicked, [this](bool fullscreen) {
        if (fullscreen) {
            emit toggle_fullscreen(this->frame);
        }
        else {
            this->addWidget(frame);
            this->setCurrentWidget(frame);
        }
        this->worker->update_size();
    });
    connect(this->theme, &Theme::mode_updated, [this](bool mode) { this->worker->set_night_mode(mode); });

    this->addWidget(this->connect_msg());
    this->addWidget(this->frame);
}

void OpenAutoTab::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    this->frame->resize(this->size());
    this->worker->update_size();
}

QWidget *OpenAutoTab::connect_msg()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("connect device to start Android Auto", widget);
    label->setFont(Theme::font_16);
    label->setAlignment(Qt::AlignCenter);

    QWidget *connection = this->wireless_config();
    if (!this->config->get_wireless_active())
        connection->hide();

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->setSpacing(0);

    QCheckBox *wireless_button = new QCheckBox("wireless", widget);
    wireless_button->setFont(Theme::font_14);
    wireless_button->setChecked(this->config->get_wireless_active());
    connect(wireless_button, &QCheckBox::toggled, [config = this->config, connection](bool checked) {
        checked ? connection->show() : connection->hide();
        config->set_wireless_active(checked);
    });

    QPushButton *settings_button = new QPushButton(widget);
    settings_button->setFlat(true);
    settings_button->setIconSize(Theme::icon_24);
    settings_button->setIcon(this->theme->make_button_icon("settings", settings_button));
    connect(settings_button, &QPushButton::clicked, [this]() {
        Dialog *dialog = new Dialog(true, this->window());
        dialog->set_body(new OpenAutoSettingsSubTab());
        QPushButton *save_button = new QPushButton("save");
        dialog->set_button(save_button);
        dialog->open();
    });

    layout2->addWidget(wireless_button);
    layout2->addStretch();
    layout2->addWidget(settings_button);

    layout->addWidget(label);
    layout->addWidget(connection);
    layout->addLayout(layout2);

    return widget;
}

QWidget *OpenAutoTab::wireless_config()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    IpInput *ip_input = new IpInput(this->config->get_wireless_address(), QFont("Titillium Web", 24), widget);
    layout->addWidget(ip_input);

    QLabel *status = new QLabel(widget);
    status->setFont(Theme::font_16);
    status->setAlignment(Qt::AlignCenter);
    layout->addWidget(status);

    QPushButton *button = new QPushButton("connect", widget);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    button->setFont(Theme::font_14);
    button->setFlat(true);
    button->setIconSize(Theme::icon_36);
    button->setIcon(this->theme->make_button_icon("wifi", button));
    connect(button, &QPushButton::clicked, [this, widget, ip_input, status]() {
        status->setText("connecting...");
        widget->setEnabled(false);
        this->worker->connect_wireless(ip_input->active_address());
    });
    layout->addWidget(button, 0, Qt::AlignCenter);

    connect(this->worker, &OpenAutoWorker::wireless_connection_success,
            [this, widget, ip_input, status](QString address) {
                status->setText(QString());
                widget->setEnabled(true);
                this->config->set_wireless_address(address);
            });
    connect(this->worker, &OpenAutoWorker::wireless_connection_failure, [widget, status]() {
        status->setText("connection failed");
        widget->setEnabled(true);
    });

    return widget;
}
