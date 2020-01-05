#include <app/bluetooth.hpp>
#include <app/config.hpp>
#include <app/tabs/media.hpp>
#include <app/theme.hpp>
#include <app/tuner.hpp>
#include <app/window.hpp>

MediaTab::MediaTab(QWidget *parent) : QWidget(parent)
{
    this->bluetooth = Bluetooth::get_instance();

    connect(this->bluetooth, SIGNAL(media_player_status_changed(BluezQt::MediaPlayer::Status)), this,
            SLOT(update_media_player_status(BluezQt::MediaPlayer::Status)));
    connect(this->bluetooth, SIGNAL(media_player_track_changed(BluezQt::MediaPlayerTrack)), this,
            SLOT(update_media_player_track(BluezQt::MediaPlayerTrack)));

    auto media_player = this->bluetooth->get_media_player().second;

    Theme *theme = Theme::get_instance();

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    QTabWidget *tab_widget = new QTabWidget;
    QWidget *bluetooth_tab = new QWidget;
    tab_widget->tabBar()->setFont(f);

    connect(this->bluetooth, &Bluetooth::media_player_changed,
            [tab_widget](QString name, BluezQt::MediaPlayerPtr player) {
                bool active = player != nullptr;
                tab_widget->setTabEnabled(1, active);
                tab_widget->setCurrentIndex(active ? 1 : 0);
            });

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(24, 24, 24, 24);
    QHBoxLayout *layout2 = new QHBoxLayout;
    QPushButton *back_button = new QPushButton;
    back_button->setFlat(true);
    back_button->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    theme->add_button_icon("skip_previous", back_button);
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
    theme->add_button_icon("pause", play_button, "play");
    connect(this->play_button, SIGNAL(clicked(bool)), this, SLOT(toggle_play(bool)));
    layout2->addWidget(this->play_button);
    QPushButton *forward_button = new QPushButton;
    forward_button->setFlat(true);
    forward_button->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    theme->add_button_icon("skip_next", forward_button);

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
    theme->add_button_icon("fast_rewind", scan_reverse);
    connect(scan_reverse, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 5); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(scan_reverse);
    QPushButton *prev_station = new QPushButton;
    prev_station->setFlat(true);
    prev_station->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    theme->add_button_icon("skip_previous", prev_station);
    connect(prev_station, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 1); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(prev_station);
    QPushButton *next_station = new QPushButton;
    next_station->setFlat(true);
    next_station->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
    theme->add_button_icon("skip_next", next_station);
    connect(next_station, &QPushButton::clicked, [tuner]() { tuner->setSliderPosition(tuner->sliderPosition() + 1); });
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(next_station);
    QPushButton *scan_forward = new QPushButton;
    scan_forward->setFlat(true);
    scan_forward->setIconSize(QSize(96 * RESOLUTION, 96 * RESOLUTION));
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
    mute->setIconSize(QSize(56 * RESOLUTION, 56 * RESOLUTION));
    connect(mute, &QPushButton::clicked, [config](bool checked = false) { config->set_radio_muted(checked); });
    theme->add_button_icon("volume_off", mute);
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    controls->addWidget(mute);
    controls->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

    radio_layout->addLayout(controls);
    radio_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    radio->setLayout(radio_layout);

    tab_widget->addTab(radio, "Radio");

    bluetooth_tab->setLayout(layout);
    tab_widget->addTab(bluetooth_tab, "Bluetooth");

    bool active = media_player != nullptr;
    tab_widget->setTabEnabled(1, active);
    tab_widget->setCurrentIndex(active ? 1 : 0);

    main_layout->addWidget(tab_widget);
    setLayout(main_layout);
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
