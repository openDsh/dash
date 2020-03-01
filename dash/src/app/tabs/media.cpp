#include <BluezQt/PendingCall>

#include <app/tabs/media.hpp>
#include <app/window.hpp>

MediaTab::MediaTab(QWidget *parent) : QTabWidget(parent)
{
    this->tabBar()->setFont(Theme::font_18);

    this->addTab(new RadioPlayerSubTab(this), "Radio");
    this->addTab(new BluetoothPlayerSubTab(this), "Bluetooth");
}

BluetoothPlayerSubTab::BluetoothPlayerSubTab(QWidget *parent) : QWidget(parent)
{
    this->bluetooth = Bluetooth::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    layout->addStretch();
    layout->addWidget(this->track_widget());
    layout->addStretch();
    layout->addWidget(this->controls_widget());
    layout->addStretch();
}

QWidget *BluetoothPlayerSubTab::track_widget()
{
    BluezQt::MediaPlayerPtr media_player = this->bluetooth->get_media_player().second;

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *artist_hdr = new QLabel("Artist", widget);
    artist_hdr->setFont(Theme::font_16);
    QLabel *artist = new QLabel((media_player != nullptr) ? media_player->track().artist() : QString(), widget);
    artist->setFont(Theme::font_14);
    artist->setStyleSheet("padding-left: 16px;");
    layout->addWidget(artist_hdr);
    layout->addWidget(artist);

    QLabel *album_hdr = new QLabel("Album", widget);
    album_hdr->setFont(Theme::font_16);
    QLabel *album = new QLabel((media_player != nullptr) ? media_player->track().album() : QString(), widget);
    album->setFont(Theme::font_14);
    album->setStyleSheet("padding-left: 16px;");
    layout->addWidget(album_hdr);
    layout->addWidget(album);

    QLabel *title_hdr = new QLabel("Title", widget);
    title_hdr->setFont(Theme::font_16);
    QLabel *title = new QLabel((media_player != nullptr) ? media_player->track().title() : QString(), widget);
    title->setFont(Theme::font_14);
    title->setStyleSheet("padding-left: 16px;");
    layout->addWidget(title_hdr);
    layout->addWidget(title);

    connect(this->bluetooth, &Bluetooth::media_player_track_changed,
            [artist, album, title](BluezQt::MediaPlayerTrack track) {
                artist->setText(track.artist());
                album->setText(track.album());
                title->setText(track.title());
            });

    return widget;
}

QWidget *BluetoothPlayerSubTab::controls_widget()
{
    BluezQt::MediaPlayerPtr media_player = this->bluetooth->get_media_player().second;
    Theme *theme = Theme::get_instance();

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *previous_button = new QPushButton(widget);
    previous_button->setFlat(true);
    previous_button->setIconSize(Theme::icon_84);
    connect(previous_button, &QPushButton::clicked, [bluetooth = this->bluetooth]() {
        BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
        if (media_player != nullptr) media_player->previous()->waitForFinished();
    });
    theme->add_button_icon("skip_previous", previous_button);
    layout->addStretch();
    layout->addWidget(previous_button);

    QPushButton *play_button = new QPushButton(widget);
    play_button->setFlat(true);
    play_button->setCheckable(true);
    bool status = (media_player != nullptr) ? media_player->status() == BluezQt::MediaPlayer::Status::Playing : false;
    play_button->setChecked(status);
    play_button->setIconSize(Theme::icon_84);
    connect(play_button, &QPushButton::clicked, [bluetooth = this->bluetooth, play_button](bool checked = false) {
        play_button->setChecked(!checked);

        BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
        if (media_player != nullptr) {
            if (checked)
                media_player->play()->waitForFinished();
            else
                media_player->pause()->waitForFinished();
        }
    });
    connect(this->bluetooth, &Bluetooth::media_player_status_changed,
            [play_button](BluezQt::MediaPlayer::Status status) {
                play_button->setChecked(status == BluezQt::MediaPlayer::Status::Playing);
            });
    theme->add_button_icon("pause", play_button, "play");
    layout->addStretch();
    layout->addWidget(play_button);

    QPushButton *forward_button = new QPushButton(widget);
    forward_button->setFlat(true);
    forward_button->setIconSize(Theme::icon_84);
    connect(forward_button, &QPushButton::clicked, [bluetooth = this->bluetooth]() {
        BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
        if (media_player != nullptr) media_player->next()->waitForFinished();
    });
    theme->add_button_icon("skip_next", forward_button);
    layout->addStretch();
    layout->addWidget(forward_button);
    layout->addStretch();

    return widget;
}

RadioPlayerSubTab::RadioPlayerSubTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();
    this->tuner = new Tuner();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    layout->addStretch();
    layout->addWidget(this->tuner_widget());
    layout->addStretch();
    layout->addWidget(this->controls_widget());
    layout->addStretch();
}

QWidget *RadioPlayerSubTab::tuner_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    this->tuner->setParent(widget);
    this->tuner->setFont(Theme::font_14);

    QLabel *station = new QLabel(QString::number(this->tuner->sliderPosition() / 10.0, 'f', 1), widget);
    station->setAlignment(Qt::AlignCenter);
    station->setFont(Theme::font_36);
    connect(this->tuner, &Tuner::station_updated,
            [station](int freq) { station->setText(QString::number(freq / 10.0, 'f', 1)); });
    layout->addWidget(station);
    layout->addStretch();
    layout->addWidget(this->tuner);

    return widget;
}

QWidget *RadioPlayerSubTab::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *scan_reverse_button = new QPushButton(widget);
    scan_reverse_button->setFlat(true);
    scan_reverse_button->setIconSize(Theme::icon_84);
    this->theme->add_button_icon("fast_rewind", scan_reverse_button);
    connect(scan_reverse_button, &QPushButton::clicked,
            [tuner = this->tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 5); });
    layout->addStretch();
    layout->addWidget(scan_reverse_button);

    QPushButton *prev_station_button = new QPushButton(widget);
    prev_station_button->setFlat(true);
    prev_station_button->setIconSize(Theme::icon_84);
    this->theme->add_button_icon("skip_previous", prev_station_button);
    connect(prev_station_button, &QPushButton::clicked,
            [tuner = this->tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 1); });
    layout->addStretch();
    layout->addWidget(prev_station_button);

    QPushButton *next_station_button = new QPushButton(widget);
    next_station_button->setFlat(true);
    next_station_button->setIconSize(Theme::icon_84);
    this->theme->add_button_icon("skip_next", next_station_button);
    connect(next_station_button, &QPushButton::clicked,
            [tuner = this->tuner]() { tuner->setSliderPosition(tuner->sliderPosition() + 1); });
    layout->addStretch();
    layout->addWidget(next_station_button);

    QPushButton *scan_forward_button = new QPushButton(widget);
    scan_forward_button->setFlat(true);
    scan_forward_button->setIconSize(Theme::icon_84);
    this->theme->add_button_icon("fast_forward", scan_forward_button);
    connect(scan_forward_button, &QPushButton::clicked,
            [tuner = this->tuner]() { tuner->setSliderPosition(tuner->sliderPosition() + 5); });
    layout->addStretch();
    layout->addWidget(scan_forward_button);

    QFrame *vert_break = new QFrame(widget);
    vert_break->setLineWidth(1);
    vert_break->setFrameShape(QFrame::VLine);
    vert_break->setFrameShadow(QFrame::Plain);
    layout->addStretch();
    layout->addWidget(vert_break);

    QPushButton *mute_button = new QPushButton(widget);
    mute_button->setFlat(true);
    mute_button->setCheckable(true);
    mute_button->setChecked(this->config->get_radio_muted());
    mute_button->setIconSize(Theme::icon_48);
    connect(mute_button, &QPushButton::clicked,
            [config = this->config](bool checked = false) { config->set_radio_muted(checked); });
    this->theme->add_button_icon("volume_off", mute_button);
    layout->addStretch();
    layout->addWidget(mute_button);
    layout->addStretch();

    return widget;
}
