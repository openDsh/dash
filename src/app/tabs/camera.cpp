#include <QLineEdit>

#include "app/tabs/camera.hpp"
#include "app/window.hpp"

CameraTab::CameraTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->player = new QMediaPlayer(this);

    this->config = Config::get_instance();
    this->url = this->config->get_cam_stream_url();

    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->connect_widget());
    layout->addWidget(this->camera_widget());

    connect(this, &CameraTab::connected, [layout]() { layout->setCurrentIndex(1); });
    connect(this, &CameraTab::disconnected, [layout]() { layout->setCurrentIndex(0); });
}

QWidget *CameraTab::camera_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *button = new QPushButton(widget);
    button->setFlat(true);
    button->setIconSize(Theme::icon_16);
    connect(button, &QPushButton::clicked, [this]() {
        this->player->setMedia(QUrl());
        this->status->setText(QString());
        this->player->stop();
    });
    this->theme->add_button_icon("close", button);
    layout->addWidget(button, 0, Qt::AlignRight);

    QVideoWidget *video = new QVideoWidget(widget);
    this->player->setVideoOutput(video);
    layout->addWidget(video);

    return widget;
}

QWidget *CameraTab::connect_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("connect camera stream", widget);
    label->setFont(Theme::font_16);

    this->status = new QLabel(widget);
    this->status->setFont(Theme::font_16);

    QPushButton *button = new QPushButton("connect", widget);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    button->setFont(Theme::font_14);
    button->setFlat(true);
    button->setIconSize(Theme::icon_36);
    this->theme->add_button_icon("wifi", button);
    connect(button, &QPushButton::clicked, [this]() {
        this->connect_stream();
        this->config->set_cam_stream_url(this->url);
    });

    layout->addStretch();
    layout->addWidget(label, 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(this->input_widget());
    layout->addWidget(this->status, 0, Qt::AlignCenter);
    layout->addWidget(button, 0, Qt::AlignCenter);
    layout->addStretch();

    return widget;
}

QWidget *CameraTab::input_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLineEdit *input = new QLineEdit(this->url, widget);
    input->setContextMenuPolicy(Qt::NoContextMenu);
    input->setFont(Theme::font_18);
    input->setAlignment(Qt::AlignCenter);
    connect(input, &QLineEdit::textEdited, [this](QString text) { this->url = text; });
    connect(input, &QLineEdit::returnPressed, [this]() {
        this->connect_stream();
        this->config->set_cam_stream_url(this->url);
    });

    layout->addStretch(1);
    layout->addWidget(input, 4);
    layout->addStretch(1);

    return widget;
}

void CameraTab::update_status(QMediaPlayer::MediaStatus media_status)
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

void CameraTab::connect_stream()
{
    connect(this->player, &QMediaPlayer::mediaStatusChanged,
            [this](QMediaPlayer::MediaStatus media_status) { this->update_status(media_status); });
    connect(this->player, QOverload<>::of(&QMediaPlayer::metaDataChanged), [this]() { emit connected(); });

    QString pipeline = QString(
                           "gst-pipeline: rtspsrc location=%1 ! decodebin ! video/x-raw ! videoconvert ! videoscale ! "
                           "xvimagesink sync=false force-aspect-ratio=false name=\"qtvideosink\"")
                           .arg(this->url);

    qInfo() << "playing stream pipeline: " << pipeline;
    this->player->setMedia(QUrl(pipeline));
    this->player->play();
}
