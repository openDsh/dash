#include <app/config.hpp>
#include <app/tabs/camera.hpp>
#include <app/window.hpp>
#include <app/theme.hpp>

CameraTab::CameraTab(QWidget *parent) : QWidget(parent)
{
    QStackedLayout *stack = new QStackedLayout(this);
    stack->addWidget(this->connect_widget());
    stack->addWidget(this->cam_widget());

    connect(this, &CameraTab::stream_connected, this, [stack]() {stack->setCurrentIndex(1);});
    connect(this, &CameraTab::stream_disconnected, this, [stack]() {stack->setCurrentIndex(0);});

    this->player = nullptr;
    // connect_stream(); TODO: offer auto-connect option
}

QWidget *CameraTab::cam_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QHBoxLayout *connection = new QHBoxLayout;
    this->status_cam_view = new QLabel("Disconnected", widget);
    this->status_cam_view->setFont(Theme::font_16);
    QPushButton *disconnect = new QPushButton("disconnect");
    disconnect->setFont(Theme::font_14);
    disconnect->setFlat(true);
    disconnect->setIconSize(Theme::icon_36);
    Theme::get_instance()->add_button_icon("wifi", disconnect);
    connection->addWidget(this->status_cam_view);
    connection->addStretch();
    connection->addWidget(disconnect);

    connect(disconnect, &QPushButton::clicked, this, &CameraTab::disconnect_stream);

    this->video_widget = new QVideoWidget;

    layout->addWidget(this->video_widget);
    layout->addLayout(connection);

    return widget;
}

QWidget *CameraTab::connect_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("connect camera stream for camera view", widget);
    label->setFont(Theme::font_16);
    label->setAlignment(Qt::AlignCenter);

    Config *config = Config::get_instance();
    QLineEdit *stream_url_input = new QLineEdit(config->get_cam_stream_url(), widget);
    stream_url_input->setFixedWidth(18 * 30);
    stream_url_input->setFont(Theme::font_18);
    stream_url_input->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(stream_url_input, &QLineEdit::editingFinished, this, [config,stream_url_input]() {
        config->set_cam_stream_url(stream_url_input->text());
    });

    this->status_connection_overlay = new QLabel("", widget);
    this->status_connection_overlay->setFont(Theme::font_16);
    this->status_connection_overlay->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QPushButton *button = new QPushButton("connect", widget);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    button->setFont(Theme::font_14);
    button->setFlat(true);
    button->setIconSize(Theme::icon_36);
    Theme::get_instance()->add_button_icon("wifi", button);
    connect(button, &QPushButton::clicked, this, [this, stream_url_input]() {
        this->connect_stream(stream_url_input->text());
    });

    connect(this, &CameraTab::stream_connecting, this, [button, stream_url_input]() {
        button->setEnabled(false); stream_url_input->setEnabled(false);
    });
    connect(this, &CameraTab::stream_disconnected, this, [button, stream_url_input]() {
        button->setEnabled(true); stream_url_input->setEnabled(true);
    });

    layout->addStretch();
    layout->addWidget(label, 1);
    layout->addStretch();
    layout->addWidget(stream_url_input, 0, Qt::AlignCenter);
    layout->addWidget(this->status_connection_overlay, 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(button, 1, Qt::AlignCenter);

    return widget;
}

void CameraTab::changed_status(QMediaPlayer::MediaStatus media_status)
{
    qInfo() << "Camera status changed to: " << media_status;

    switch(media_status) {
        case QMediaPlayer::InvalidMedia:
            this->status_connection_overlay->setText("unable to connect");
            emit stream_disconnected();
            break;
         case QMediaPlayer::LoadingMedia:
         case QMediaPlayer::LoadedMedia:
         case QMediaPlayer::BufferedMedia:
            this->status_connection_overlay->setText("connecting...");
            emit stream_connecting();
            break;
         case QMediaPlayer::EndOfMedia:
         case QMediaPlayer::StalledMedia:
         case QMediaPlayer::NoMedia:
        default:
            this->status_connection_overlay->setText("disconnected");
            emit stream_disconnected();
            break;
    }
}

void CameraTab::new_metadata(const QString &key, const QVariant &value)
{
    this->status_cam_view->setText("connected");
    emit stream_connected();
}

void CameraTab::disconnect_stream()
{
    this->player->setMedia(QUrl());
    this->player->stop();
}

void CameraTab::connect_stream(QString stream_url)
{
    if (this->player != nullptr)
        delete this->player;

    this->player = new QMediaPlayer;
    this->player->setVideoOutput(this->video_widget);

    connect(this->player, &QMediaPlayer::mediaStatusChanged, this, &CameraTab::changed_status);
    connect(this->player, qOverload<const QString&,const QVariant&>(&QMediaPlayer::metaDataChanged), this, &CameraTab::new_metadata);

    // e.g. "rtsp://10.0.0.185:8554/unicast"
    QString stream_pipeline = QString("gst-pipeline: rtspsrc location=%1 ! decodebin ! video/x-raw ! videoconvert ! videoscale ! xvimagesink sync=false force-aspect-ratio=false name=\"qtvideosink\"").arg(stream_url);
    qInfo() << "Playing stream pipeline: " << stream_pipeline;
    this->player->setMedia(QUrl(stream_pipeline));
    this->player->play();
}
