#include "app/pages/openauto.hpp"

#include "app/config.hpp"
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
                                          connected_accessories_enumerator))
{
    this->create_usb_workers();
    this->create_io_service_workers();

    this->app->waitForDevice(true);
}

OpenAutoWorker::~OpenAutoWorker()
{
    std::for_each(this->thread_pool.begin(), this->thread_pool.end(),
                  std::bind(&std::thread::join, std::placeholders::_1));
    libusb_exit(this->usb_context);
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

OpenAutoPage::Settings::Settings(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
    this->config = Config::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(this->settings_widget());
}

QSize OpenAutoPage::Settings::sizeHint() const
{
    int label_width = QFontMetrics(this->font()).averageCharWidth() * 20;
    return QSize(label_width * 2, this->height());
}

QLayout *OpenAutoPage::Settings::settings_widget()
{
    QVBoxLayout *layout = new QVBoxLayout();

    layout->addLayout(this->rhd_row_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addLayout(this->frame_rate_row_widget(), 1);
    layout->addLayout(this->resolution_row_widget(), 1);
    layout->addLayout(this->dpi_row_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addLayout(this->rt_audio_row_widget(), 1);
    layout->addLayout(this->audio_channels_row_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addLayout(this->bluetooth_row_widget(), 1);
    layout->addLayout(this->autoconnect_row_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addLayout(this->connected_indicator_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addLayout(this->touchscreen_row_widget(), 1);
    layout->addLayout(this->buttons_row_widget(), 1);

    return layout;
}

QLayout *OpenAutoPage::Settings::rhd_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Right-Hand-Drive");
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->config->openauto_config->getHandednessOfTrafficType() ==
                       openauto::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE);
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setHandednessOfTrafficType(
            state ? openauto::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE
                  : openauto::configuration::HandednessOfTrafficType::LEFT_HAND_DRIVE);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QLayout *OpenAutoPage::Settings::frame_rate_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Frame Rate");
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox();
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QRadioButton *fps_30_button = new QRadioButton("30fps", group);
    fps_30_button->setChecked(this->config->openauto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    connect(fps_30_button, &QRadioButton::clicked,
            [config = this->config]() { config->openauto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_30); });
    group_layout->addWidget(fps_30_button);

    QRadioButton *fps_60_button = new QRadioButton("60fps", group);
    fps_60_button->setChecked(this->config->openauto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_60);
    connect(fps_60_button, &QRadioButton::clicked,
            [config = this->config]() { config->openauto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_60); });
    group_layout->addWidget(fps_60_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return layout;
}

QLayout *OpenAutoPage::Settings::resolution_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Resolution");
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox();
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QRadioButton *res_480_button = new QRadioButton("480p", group);
    res_480_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                               aasdk::proto::enums::VideoResolution::_480p);
    connect(res_480_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_480p);
    });
    group_layout->addWidget(res_480_button);

    QRadioButton *res_720_button = new QRadioButton("720p", group);
    res_720_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                               aasdk::proto::enums::VideoResolution::_720p);
    connect(res_720_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_720p);
    });
    group_layout->addWidget(res_720_button);

    QRadioButton *res_1080_button = new QRadioButton("1080p", group);
    res_1080_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                                aasdk::proto::enums::VideoResolution::_1080p);
    connect(res_1080_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_1080p);
    });
    group_layout->addWidget(res_1080_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return layout;
}

QLayout *OpenAutoPage::Settings::dpi_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("DPI");
    layout->addWidget(label, 1);

    layout->addLayout(this->dpi_widget(), 1);

    return layout;
}

QLayout *OpenAutoPage::Settings::dpi_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setTracking(false);
    slider->setRange(0, 512);
    slider->setValue(this->config->openauto_config->getScreenDPI());
    QLabel *value = new QLabel(QString::number(slider->value()));
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

QLayout *OpenAutoPage::Settings::rt_audio_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("RtAudio");
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->config->openauto_config->getAudioOutputBackendType() ==
                       openauto::configuration::AudioOutputBackendType::RTAUDIO);
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setAudioOutputBackendType(state
                                                               ? openauto::configuration::AudioOutputBackendType::RTAUDIO
                                                               : openauto::configuration::AudioOutputBackendType::QT);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QLayout *OpenAutoPage::Settings::audio_channels_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Audio Channels");
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox();
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QCheckBox *music_button = new QCheckBox("Music", group);
    music_button->setChecked(this->config->openauto_config->musicAudioChannelEnabled());
    connect(music_button, &QCheckBox::toggled,
            [config = this->config](bool checked) { config->openauto_config->setMusicAudioChannelEnabled(checked); });
    group_layout->addWidget(music_button);
    group_layout->addStretch(2);

    QCheckBox *speech_button = new QCheckBox("Speech", group);
    speech_button->setChecked(this->config->openauto_config->speechAudioChannelEnabled());
    connect(speech_button, &QCheckBox::toggled,
            [config = this->config](bool checked) { config->openauto_config->setSpeechAudioChannelEnabled(checked); });
    group_layout->addWidget(speech_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return layout;
}

QLayout *OpenAutoPage::Settings::bluetooth_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Bluetooth");
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->config->openauto_config->getBluetoothAdapterType() ==
                       openauto::configuration::BluetoothAdapterType::LOCAL);
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setBluetoothAdapterType(state ? openauto::configuration::BluetoothAdapterType::LOCAL
                                                               : openauto::configuration::BluetoothAdapterType::NONE);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QLayout *OpenAutoPage::Settings::autoconnect_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Autoconnect Last Device");
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->config->openauto_config->getAutoconnectBluetooth());
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state){
        config->openauto_config->setAutoconnectBluetooth(state);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QLayout *OpenAutoPage::Settings::connected_indicator_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Show connection status");
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->config->get_show_aa_connected());
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state){
        config->set_show_aa_connected(state);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QLayout *OpenAutoPage::Settings::touchscreen_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Touchscreen");
    layout->addWidget(label, 1);

    Switch *toggle = new Switch();
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->config->openauto_config->getTouchscreenEnabled());
    connect(toggle, &Switch::stateChanged,
            [config = this->config](bool state) { config->openauto_config->setTouchscreenEnabled(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return layout;
}

QCheckBox *OpenAutoPage::Settings::button_checkbox(QString name, QString key,
                                                   aasdk::proto::enums::ButtonCode::Enum code)
{
    QCheckBox *checkbox = new QCheckBox(QString("%1 [%2]").arg(name).arg(key));
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

QLayout *OpenAutoPage::Settings::buttons_row_widget()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel("Buttons");

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

OpenAutoPage::OpenAutoPage(Arbiter &arbiter, QWidget *parent)
    : QStackedWidget(parent)
    , Page(arbiter, "Android Auto", "android_auto", true, this)
    , connected_icon_name_("android_auto_color")
{
}

void OpenAutoPage::init()
{
    this->config = Config::get_instance();

    this->frame = new OpenAutoFrame(this);

    std::function<void(bool)> callback = [frame = this->frame](bool active) { frame->toggle(active); };
    this->worker = new OpenAutoWorker(callback, this->arbiter.theme().mode == Session::Theme::Dark, frame);

    connect(this->frame, &OpenAutoFrame::toggle, [this](bool enable) {
        if (!enable && this->frame->is_fullscreen()) {
            this->addWidget(frame);
            this->frame->toggle_fullscreen();
        }
        this->setCurrentIndex(enable ? 1 : 0);
        emit connected(enable);
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
    connect(&this->arbiter, &Arbiter::mode_changed, [this](Session::Theme::Mode mode){
        this->worker->set_night_mode(mode == Session::Theme::Dark);
    });

    this->addWidget(this->connect_msg());
    this->addWidget(this->frame);
}

void OpenAutoPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    this->frame->resize(this->size());
    this->worker->update_size();
}

QWidget *OpenAutoPage::connect_msg()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *label = new QLabel("Connect Device to Start Android Auto", widget);
    label->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->setSpacing(0);

    Dialog *dialog = new Dialog(this->arbiter, true, this->window());
    dialog->set_body(new OpenAutoPage::Settings(this->arbiter, this));
    QPushButton *save_button = new QPushButton("save");
    connect(save_button, &QPushButton::clicked, [this]() {
        this->config->openauto_config->setButtonCodes(this->config->openauto_button_codes);
        this->config->openauto_config->save();
    });
    dialog->set_button(save_button);

    QPushButton *settings_button = new QPushButton(widget);
    settings_button->setFlat(true);
    this->arbiter.forge().iconize("settings", settings_button, 24);
    connect(settings_button, &QPushButton::clicked, [dialog]() { dialog->open(); });

    layout2->addStretch();
    layout2->addWidget(settings_button);

    layout->addLayout(layout2);
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();

    return widget;
}
