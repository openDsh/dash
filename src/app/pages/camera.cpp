#include <QLineEdit>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraImageCapture>
#include <QTimer>

#include "app/pages/camera.hpp"
#include "app/window.hpp"

CameraPage::CameraPage(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , Page(arbiter, "Camera", "camera", true, this)
{
    this->theme = Theme::get_instance();
    this->player = new QMediaPlayer(this);
    this->local_cam = nullptr;
    this->local_index = 0;
    this->reconnect_timer = new QTimer(this);
    connect(this->reconnect_timer, &QTimer::timeout, this, &CameraPage::count_down);

    this->config = Config::get_instance();

    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->connect_widget());
    layout->addWidget(this->local_camera_widget());
    layout->addWidget(this->network_camera_widget());

    connect(this, &CameraPage::disconnected, [layout,this]() {
        layout->setCurrentIndex(0);
        if (this->config->get_cam_autoconnect()) {
            qDebug() << "Camera disconnected. Auto reconnect in" << this->config->get_cam_autoconnect_time_secs() << "seconds";
            this->reconnect_message = this->status->text() + ". reconnecting in %1 ";
            this->reconnect_in_secs = this->config->get_cam_autoconnect_time_secs();
            this->reconnect_timer->start(1000);
        }
    });
    connect(this, &CameraPage::connected_local, [layout]() { layout->setCurrentIndex(1); });
    connect(this, &CameraPage::connected_network, [layout]() { layout->setCurrentIndex(2); });

    if (this->config->get_cam_autoconnect())
        this->connect_cam();
}

QWidget *CameraPage::connect_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("connect camera", widget);

    this->status = new QLabel(widget);

    QWidget *cam_stack_widget = new QWidget(widget);
    QStackedLayout *cam_stack = new QStackedLayout(cam_stack_widget);
    cam_stack->addWidget(this->local_cam_selector());
    cam_stack->addWidget(this->network_cam_selector());

    QCheckBox *auto_reconnect_toggle = new QCheckBox("Automatically reconnect", this);
    auto_reconnect_toggle->setLayoutDirection(Qt::RightToLeft);
    auto_reconnect_toggle->setChecked(this->config->get_cam_autoconnect());
    connect(auto_reconnect_toggle, &QCheckBox::toggled, [this](bool checked) {
        this->config->set_cam_autoconnect(checked);
        if (!checked) emit autoconnect_disabled(); });
    connect(this, &CameraPage::autoconnect_disabled, [auto_reconnect_toggle, this]() {
        this->reconnect_timer->stop();
        this->config->set_cam_autoconnect(false);
        auto_reconnect_toggle->setChecked(false);
    });

    QCheckBox *network_toggle = new QCheckBox("Network", this);
    connect(network_toggle, &QCheckBox::toggled, [this, cam_stack](bool checked) {
        cam_stack->setCurrentIndex(checked? 1 : 0);
        this->status->clear();
        this->config->set_cam_is_network(checked);
    });
    network_toggle->setChecked(this->config->get_cam_is_network());

    QWidget *checkboxes_widget = new QWidget(this);
    QHBoxLayout *checkboxes = new QHBoxLayout(checkboxes_widget);
    checkboxes->addWidget(network_toggle);
    checkboxes->addStretch();
    checkboxes->addWidget(auto_reconnect_toggle);

    layout->addStretch();
    layout->addWidget(label, 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(cam_stack_widget);
    layout->addWidget(this->status, 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(this->connect_button(), 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(checkboxes_widget);

    return widget;
}

QWidget *CameraPage::local_camera_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *disconnect = new QPushButton(widget);
    disconnect->setFlat(true);
    disconnect->setIconSize(Theme::icon_16);
    connect(disconnect, &QPushButton::clicked, [this]() {
        this->status->setText(QString());
        emit autoconnect_disabled();
        this->local_cam->stop();
        this->local_cam->unload();
        this->local_cam->deleteLater();
        this->local_cam = nullptr;
    });
    disconnect->setIcon(this->theme->make_button_icon("close", disconnect));
    layout->addWidget(disconnect, 0, Qt::AlignRight);

    this->local_video_widget = new QCameraViewfinder(widget);
    layout->addWidget(this->local_video_widget);

    return widget;
}

QWidget *CameraPage::network_camera_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *disconnect = new QPushButton(widget);
    disconnect->setFlat(true);
    disconnect->setIconSize(Theme::icon_16);
    connect(disconnect, &QPushButton::clicked, [this]() {
        emit autoconnect_disabled();
        this->player->setMedia(QUrl());
        this->status->setText(QString());
        this->player->stop();
    });
    disconnect->setIcon(this->theme->make_button_icon("close", disconnect));
    layout->addWidget(disconnect, 0, Qt::AlignRight);

    QVideoWidget *video = new QVideoWidget(widget);
    this->player->setVideoOutput(video);
    layout->addWidget(video);

    return widget;
}

QPushButton *CameraPage::connect_button()
{
    QPushButton *connect_button = new QPushButton("connect", this);
    connect_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect_button->setFlat(true);
    connect(connect_button, &QPushButton::clicked, this, &CameraPage::connect_cam);

    return connect_button;
}

void CameraPage::count_down()
{
    this->reconnect_in_secs--;
    this->status->setText(this->reconnect_message.arg(this->reconnect_in_secs) + (this->reconnect_in_secs == 1? "second":"seconds"));
    if (this->reconnect_in_secs == 0)
        this->connect_cam();
}

void CameraPage::connect_cam()
{
    this->reconnect_timer->stop();
    this->status->clear();
    if (this->config->get_cam_is_network())
        this->connect_network_stream();
    else
        this->connect_local_stream();
}

QWidget *CameraPage::local_cam_selector()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel(widget);
    label->setAlignment(Qt::AlignCenter);
    QWidget *selector = this->selector_widget(label);
    this->populate_local_cams();
    connect(this, &CameraPage::prev_cam, [this, label]() {
        this->local_index = (this->local_index - 1 + this->local_cams.size()) % this->local_cams.size();
        auto cam = this->local_cams.at(local_index);
        label->setText(cam.first);
        this->config->set_cam_local_device(cam.second);
    });
    connect(this, &CameraPage::next_cam, [this, label]() {
        this->local_index = (this->local_index + 1) % this->local_cams.size();
        auto cam = this->local_cams.at(this->local_index);
        label->setText(cam.first);
        this->config->set_cam_local_device(cam.second);
    });
    label->setText(this->local_cams.at(local_index).first);
    layout->addWidget(selector);

    QHBoxLayout *refresh_row = new QHBoxLayout();
    QPushButton *refresh_button = new QPushButton(widget);
    refresh_button->setFlat(true);
    refresh_button->setIcon(this->theme->make_button_icon("refresh", refresh_button));

    refresh_row->addStretch(1);
    refresh_row->addWidget(refresh_button);
    refresh_row->addStretch(1);
    layout->addLayout(refresh_row);
    connect(refresh_button, &QPushButton::clicked, this, [this,label]{
        this->populate_local_cams();
        label->setText(this->local_cams.at(local_index).first);
    });

    return widget;
}

QWidget *CameraPage::selector_widget(QWidget *selection)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    left_button->setIcon(this->theme->make_button_icon("arrow_left", left_button));
    connect(left_button, &QPushButton::clicked, this, &CameraPage::prev_cam);

    QPushButton *right_button = new QPushButton(this);
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    right_button->setIcon(this->theme->make_button_icon("arrow_right", right_button));
    connect(right_button, &QPushButton::clicked, this, &CameraPage::next_cam);

    layout->addStretch(1);
    layout->addWidget(left_button);
    layout->addWidget(selection, 2);
    layout->addWidget(right_button);
    layout->addStretch(1);

    return widget;
}

void CameraPage::populate_local_cams()
{
    this->local_cams.clear();
    this->local_index=0;
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    QString default_device = this->config->get_cam_local_device();
    if (default_device.isEmpty() && !QCameraInfo::defaultCamera().isNull())
        default_device = QCameraInfo::defaultCamera().deviceName();

    int i = 0;
    for (auto const &cam : cameras) {
      QString pretty_name = cam.description() + " at " + cam.deviceName();
      this->local_cams.append(QPair<QString,QString>(pretty_name, cam.deviceName()));
      if (cam.deviceName() == default_device)
          this->local_index = i;
      i++;
    }

    if (this->local_cams.isEmpty()) {
        this->local_cams.append(QPair<QString,QString>(QString("<No local cameras found>"),QString()));
    }
}

QWidget *CameraPage::network_cam_selector()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLineEdit *input = new QLineEdit(this->config->get_cam_network_url(), widget);
    input->setContextMenuPolicy(Qt::NoContextMenu);
    input->setFont(Theme::font_18);
    input->setAlignment(Qt::AlignCenter);
    connect(input, &QLineEdit::textEdited, [this](QString text) {
        this->status->clear();
        this->config->set_cam_network_url(text);
        this->reconnect_timer->stop();
    });
    connect(input, &QLineEdit::cursorPositionChanged, [this](int, int) {
        this->reconnect_timer->stop();
        this->status->clear(); });
    connect(input, &QLineEdit::returnPressed, this, &CameraPage::connect_network_stream);

    layout->addStretch(1);
    layout->addWidget(input, 4);
    layout->addStretch(1);

    return widget;
}

void CameraPage::update_network_status(QMediaPlayer::MediaStatus media_status)
{
    qInfo() << "camera status changed to: " << media_status;

    switch (media_status) {
        case QMediaPlayer::LoadingMedia:
        case QMediaPlayer::LoadedMedia:
        case QMediaPlayer::BufferedMedia:
            this->status->setText("connecting...");
            break;
        default:
            this->status->setText("connection failed");
            emit disconnected();
            break;
    }
}

void CameraPage::connect_network_stream()
{
    connect(this->player, &QMediaPlayer::mediaStatusChanged,
            [this](QMediaPlayer::MediaStatus media_status) { this->update_network_status(media_status); });
    connect(this->player, QOverload<>::of(&QMediaPlayer::metaDataChanged), [this]() { emit connected_network(); });
    qInfo() << "playing stream: " << this->config->get_cam_network_url();
    this->player->setMedia(QUrl(this->config->get_cam_network_url()));
    this->player->play();
}

void CameraPage::connect_local_stream()
{
    if (this->local_cam != nullptr) {
        delete this->local_cam;
        this->local_cam = nullptr;
    }

    const QString &local = this->config->get_cam_local_device();
    if (!this->local_cam_available(local)) {
        this->status->setText("Camera unavailable");
        return;
    }

    qDebug() << "Connecting to local cam " << local;
    this->local_cam = new QCamera(local.toUtf8(), this);
    this->local_cam->setViewfinder(this->local_video_widget);
    connect(this->local_cam, &QCamera::statusChanged, this, &CameraPage::update_local_status);
    this->local_cam->start();
}

void CameraPage::choose_video_resolution()
{
    QSize window_size = this->size();
    QCameraImageCapture imageCapture(this->local_cam);
    int min_gap = 10000, xgap, ygap;
    QSize max_fit;
    for (auto const &resolution : imageCapture.supportedResolutions()) {
        xgap = window_size.width() - resolution.width();
        ygap = window_size.height() - resolution.height();
        if (xgap >= 0 && ygap >= 0 && xgap+ygap < min_gap) {
            min_gap = xgap + ygap;
            max_fit = resolution;
        }
    }
    if (max_fit.isValid()) {
        qDebug() << "Local cam auto resolution" << max_fit << "to fit in" << window_size;
        this->local_cam_settings.setResolution(max_fit);
    }
    else
        qDebug() << "No suitable resolutions found to fit in" << window_size;

    if (this->config->get_cam_local_format_override() > 0) {
      this->local_cam_settings.setPixelFormat(this->config->get_cam_local_format_override());
      qDebug() << "Overriding local cam format to" << this->config->get_cam_local_format_override();
    }
    this->local_cam->setViewfinderSettings(this->local_cam_settings);
}

bool CameraPage::local_cam_available(const QString &device)
{
    if (device.isEmpty())
        return false;

    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cameras) {
        if (cameraInfo.deviceName() == device)
            return true;
    }
    return false;
}

void CameraPage::update_local_status(QCamera::Status status)
{
    qDebug() << "Local camera" << this->config->get_cam_local_device() << "changed status to" << status;

    switch (status) {
      case QCamera::ActiveStatus:
        qDebug() << "Local camera format:" << this->local_cam->viewfinderSettings().pixelFormat();
        emit connected_local();
        break;
      case QCamera::LoadedStatus:
        this->choose_video_resolution();
        // fall-through
      case QCamera::LoadingStatus:
      case QCamera::StartingStatus:
        this->status->setText("connecting..."); 
        break;
      case QCamera::UnloadedStatus:
        emit disconnected();
        break;
      default:
        break;
    }

    if (this->local_cam != nullptr && !this->local_cam->error() == QCamera::NoError) {
        qCritical() << "Local camera" << this->local_cam << "got error" << this->local_cam->error();
        this->status->setText("Error connecting to local camera at '" + this->config->get_cam_local_device() + "'");
    }
}
