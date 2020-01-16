#include <BluezQt/PendingCall>

#include <app/bluetooth.hpp>
#include <app/config.hpp>
#include <app/tabs/media.hpp>
#include <app/theme.hpp>
#include <app/tuner.hpp>
#include <app/window.hpp>

MediaTab::MediaTab(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    QTabWidget *widget = new QTabWidget(this);
    widget->tabBar()->setFont(Theme::font_18);
    widget->addTab(new RadioPlayerTab(widget), "Radio");
    widget->addTab(new BluetoothPlayerTab(widget), "Bluetooth");

    layout->addWidget(widget);
}

BluetoothPlayerTab::BluetoothPlayerTab(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    layout->addStretch(1);
    layout->addWidget(this->track_widget());
    layout->addStretch(1);
    layout->addWidget(this->controls_widget());
    layout->addStretch(1);
}

QWidget *BluetoothPlayerTab::track_widget()
{
    Bluetooth *bluetooth = Bluetooth::get_instance();
    BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *artist_label = new QLabel("Artist", widget);
    artist_label->setFont(Theme::font_18);
    QLabel *artist = new QLabel((media_player != nullptr) ? media_player->track().artist() : QString(), widget);
    artist->setFont(Theme::font_14);
    artist->setStyleSheet("padding-left: 16px;");
    layout->addWidget(artist_label);
    layout->addWidget(artist);

    QLabel *album_label = new QLabel("Album", widget);
    album_label->setFont(Theme::font_18);
    QLabel *album = new QLabel((media_player != nullptr) ? media_player->track().album() : QString(), widget);
    album->setFont(Theme::font_14);
    album->setStyleSheet("padding-left: 16px;");
    layout->addWidget(album_label);
    layout->addWidget(album);

    QLabel *title_label = new QLabel("Title", widget);
    title_label->setFont(Theme::font_18);
    QLabel *title = new QLabel((media_player != nullptr) ? media_player->track().title() : QString(), widget);
    title->setFont(Theme::font_14);
    title->setStyleSheet("padding-left: 16px;");
    layout->addWidget(title_label);
    layout->addWidget(title);

    connect(bluetooth, &Bluetooth::media_player_track_changed, [artist, album, title](BluezQt::MediaPlayerTrack track) {
        artist->setText(track.artist());
        album->setText(track.album());
        title->setText(track.title());
    });

    return widget;
}

QWidget *BluetoothPlayerTab::controls_widget()
{
    Bluetooth *bluetooth = Bluetooth::get_instance();
    BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
    Theme *theme = Theme::get_instance();

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *previous_button = new QPushButton(widget);
    previous_button->setFlat(true);
    previous_button->setIconSize(Theme::icon_96);
    connect(previous_button, &QPushButton::clicked, [bluetooth]() {
        BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
        if (media_player != nullptr) media_player->previous()->waitForFinished();
    });
    theme->add_button_icon("skip_previous", previous_button);
    layout->addStretch(1);
    layout->addWidget(previous_button);

    QPushButton *play_button = new QPushButton(widget);
    play_button->setFlat(true);
    play_button->setCheckable(true);
    bool status = (media_player != nullptr) ? media_player->status() == BluezQt::MediaPlayer::Status::Playing : false;
    play_button->setChecked(status);
    play_button->setIconSize(Theme::icon_96);
    connect(play_button, &QPushButton::clicked, [play_button, bluetooth](bool checked = false) {
        play_button->setChecked(!checked);

        BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
        if (media_player != nullptr) {
            if (checked)
                media_player->play()->waitForFinished();
            else
                media_player->pause()->waitForFinished();
        }
    });
    connect(bluetooth, &Bluetooth::media_player_status_changed, [play_button](BluezQt::MediaPlayer::Status status) {
        play_button->setChecked(status == BluezQt::MediaPlayer::Status::Playing);
    });
    theme->add_button_icon("pause", play_button, "play");
    layout->addStretch(1);
    layout->addWidget(play_button);

    QPushButton *forward_button = new QPushButton(widget);
    forward_button->setFlat(true);
    forward_button->setIconSize(Theme::icon_96);
    connect(forward_button, &QPushButton::clicked, [bluetooth]() {
        BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
        if (media_player != nullptr) media_player->next()->waitForFinished();
    });
    theme->add_button_icon("skip_next", forward_button);
    layout->addStretch(1);
    layout->addWidget(forward_button);
    layout->addStretch(1);

    return widget;
}

RadioPlayerTab::RadioPlayerTab(QWidget *parent) : QWidget(parent)
{
    Theme *theme = Theme::get_instance();
    Config *config = Config::get_instance();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(24, 24, 24, 24);
    Tuner *tuner = new Tuner(config->get_radio_station());
    tuner->setFont(Theme::font_14);

    QLabel *tuner_value = new QLabel(QString::number(tuner->sliderPosition() / 10.0, 'f', 1));
    tuner_value->setAlignment(Qt::AlignCenter);
    tuner_value->setFont(Theme::font_36);

    connect(tuner, &Tuner::station_updated, [tuner_value, config](int station) {
        tuner_value->setText(QString::number(station / 10.0, 'f', 1));
        config->set_radio_station(station);
    });

    layout->addStretch(1);
    layout->addWidget(tuner_value);
    layout->addStretch(1);
    layout->addWidget(tuner);
    layout->addStretch(1);

    QHBoxLayout *controls = new QHBoxLayout;
    QPushButton *scan_reverse = new QPushButton;
    scan_reverse->setFlat(true);
    scan_reverse->setIconSize(Theme::icon_96);
    theme->add_button_icon("fast_rewind", scan_reverse);
    connect(scan_reverse, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 5); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(scan_reverse);
    QPushButton *prev_station = new QPushButton;
    prev_station->setFlat(true);
    prev_station->setIconSize(Theme::icon_96);
    theme->add_button_icon("skip_previous", prev_station);
    connect(prev_station, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 1); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(prev_station);
    QPushButton *next_station = new QPushButton;
    next_station->setFlat(true);
    next_station->setIconSize(Theme::icon_96);
    theme->add_button_icon("skip_next", next_station);
    connect(next_station, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() + 1); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(next_station);
    QPushButton *scan_forward = new QPushButton;
    scan_forward->setFlat(true);
    scan_forward->setIconSize(Theme::icon_96);
    theme->add_button_icon("fast_forward", scan_forward);
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
    mute->setIconSize(Theme::icon_56);
    connect(mute, &QPushButton::clicked, [config](bool checked = false) { config->set_radio_muted(checked); });
    theme->add_button_icon("volume_off", mute);
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(mute);
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

    layout->addLayout(controls);
    layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    this->setLayout(layout);
}
