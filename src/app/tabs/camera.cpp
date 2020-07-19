#include <QLineEdit>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraImageCapture>

#include "app/tabs/camera.hpp"
#include "app/window.hpp"

CameraTab::CameraTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->player = new QMediaPlayer(this);
    this->local_cam = nullptr;
    this->local_index = 0;

    this->config = Config::get_instance();

    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->connect_widget());
    layout->addWidget(this->local_camera_widget());
    layout->addWidget(this->network_camera_widget());

    connect(this, &CameraTab::disconnected, [layout]() { layout->setCurrentIndex(0); });
    connect(this, &CameraTab::connected_local, [layout]() { layout->setCurrentIndex(1); });
    connect(this, &CameraTab::connected_network, [layout]() { layout->setCurrentIndex(2); });
}

QWidget *CameraTab::connect_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("connect camera", widget);
    label->setFont(Theme::font_16);

    this->status = new QLabel(widget);
    this->status->setFont(Theme::font_16);

    QWidget *cam_stack_widget = new QWidget(widget);
    QStackedLayout *cam_stack = new QStackedLayout(cam_stack_widget);
    cam_stack->addWidget(this->local_cam_selector());
    cam_stack->addWidget(this->network_cam_selector());
    QCheckBox *network_toggle = new QCheckBox("Network", this);
    network_toggle->setFont(Theme::font_14);
    connect(network_toggle, &QCheckBox::toggled, [this, cam_stack](bool checked) {
	cam_stack->setCurrentIndex(checked? 1 : 0);
        this->status->setText("");
        this->config->set_cam_is_network(checked);
    });
    network_toggle->setChecked(this->config->get_cam_is_network());

    layout->addStretch();
    layout->addWidget(label, 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(cam_stack_widget);
    layout->addWidget(this->status, 0, Qt::AlignCenter);
    layout->addWidget(this->connect_button(), 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(network_toggle, 1, Qt::AlignLeft);

    return widget;
}

QWidget *CameraTab::local_camera_widget()
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
        this->local_cam->stop();
        this->local_cam->unload();
        this->local_cam->deleteLater();
        this->local_cam = nullptr;
    });
    this->theme->add_button_icon("close", disconnect);
    layout->addWidget(disconnect, 0, Qt::AlignRight);

    this->local_video_widget = new QCameraViewfinder(widget);
    layout->addWidget(this->local_video_widget);

    return widget;
}

QWidget *CameraTab::network_camera_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *disconnect = new QPushButton(widget);
    disconnect->setFlat(true);
    disconnect->setIconSize(Theme::icon_16);
    connect(disconnect, &QPushButton::clicked, [this]() {
        this->player->setMedia(QUrl());
        this->status->setText(QString());
        this->player->stop();
    });
    this->theme->add_button_icon("close", disconnect);
    layout->addWidget(disconnect, 0, Qt::AlignRight);

    QVideoWidget *video = new QVideoWidget(widget);
    this->player->setVideoOutput(video);
    layout->addWidget(video);

    return widget;
}


QPushButton *CameraTab::connect_button()
{
    QPushButton *connect_button = new QPushButton("connect", this);
    connect_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect_button->setFont(Theme::font_14);
    connect_button->setFlat(true);
    connect_button->setIconSize(Theme::icon_36);
    connect(connect_button, &QPushButton::clicked, [this]() {
        this->status->setText("");
        if (this->config->get_cam_is_network())
            this->connect_network_stream();
        else
            this->connect_local_stream();
    });

    return connect_button;
}

QWidget *CameraTab::local_cam_selector()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel(widget);
    label->setFont(Theme::font_16);
    label->setAlignment(Qt::AlignCenter);
    QWidget *selector = this->selector_widget(label);
    this->populate_local_cams();
    connect(this, &CameraTab::prev_cam, [this, label]() {
        this->local_index = (this->local_index - 1 + this->local_cams.size()) % this->local_cams.size();
        auto cam = this->local_cams.at(local_index);
        label->setText(cam.first);
        this->config->set_cam_local_device(cam.second);
    });
    connect(this, &CameraTab::next_cam, [this, label]() {
        this->local_index = (this->local_index + 1) % this->local_cams.size();
        auto cam = this->local_cams.at(this->local_index);
        label->setText(cam.first);
        this->config->set_cam_local_device(cam.second);
    });
    label->setText(this->local_cams.at(local_index).first);
    layout->addWidget(selector);

    QHBoxLayout *refresh_row = new QHBoxLayout();
    QPushButton *refresh_button = new QPushButton(widget);
    refresh_button->setFont(Theme::font_14);
    refresh_button->setFlat(true);
    this->theme->add_button_icon("refresh", refresh_button);

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

QWidget *CameraTab::selector_widget(QWidget *selection)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_left", left_button);
    connect(left_button, &QPushButton::clicked, this, &CameraTab::prev_cam);

    QPushButton *right_button = new QPushButton(this);
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_right", right_button);
    connect(right_button, &QPushButton::clicked, this, &CameraTab::next_cam);

    layout->addStretch(1);
    layout->addWidget(left_button);
    layout->addWidget(selection, 2);
    layout->addWidget(right_button);
    layout->addStretch(1);

    return widget;
}

bool CameraTab::populate_local_cams()
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
        return false;
    }
    return true;
}

QWidget *CameraTab::network_cam_selector()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLineEdit *input = new QLineEdit(this->config->get_cam_network_url(), widget);
    input->setContextMenuPolicy(Qt::NoContextMenu);
    input->setFont(Theme::font_18);
    input->setAlignment(Qt::AlignCenter);
    connect(input, &QLineEdit::textEdited, [this](QString text) { this->config->set_cam_network_url(text); });
    connect(input, &QLineEdit::returnPressed, this, &CameraTab::connect_network_stream);

    layout->addStretch(1);
    layout->addWidget(input, 4);
    layout->addStretch(1);

    return widget;
}

void CameraTab::update_network_status(QMediaPlayer::MediaStatus media_status)
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

void CameraTab::connect_network_stream()
{
    connect(this->player, &QMediaPlayer::mediaStatusChanged,
            [this](QMediaPlayer::MediaStatus media_status) { this->update_network_status(media_status); });
    connect(this->player, QOverload<>::of(&QMediaPlayer::metaDataChanged), [this]() { emit connected_network(); });
    qInfo() << "playing stream: " << this->config->get_cam_network_url();
    this->player->setMedia(QUrl(this->config->get_cam_network_url()));
    this->player->play();
}

void CameraTab::connect_local_stream()
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
    connect(this->local_cam, &QCamera::statusChanged, this, &CameraTab::update_local_status);
    this->local_cam->start();
}

void CameraTab::choose_video_resolution()
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
    //    this->local_cam_settings.setPixelFormat(QVideoFrame::Format_YV12);
    this->local_cam->setViewfinderSettings(this->local_cam_settings);
}

bool CameraTab::local_cam_available(const QString &device)
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

void CameraTab::update_local_status(QCamera::Status status)
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
