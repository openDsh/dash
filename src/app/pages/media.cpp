#include <fileref.h>
#include <math.h>
#include <tag.h>
#include <tpropertymap.h>
#include <BluezQt/PendingCall>
#include <QDirIterator>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMediaPlaylist>

#include "app/pages/media.hpp"
#include "app/window.hpp"

MediaPage::MediaPage(QWidget *parent) : QTabWidget(parent)
{
    // this->addTab(new RadioPlayerTab(this), "Radio");
    this->addTab(new BluetoothPlayerTab(this), "Bluetooth");
    this->addTab(new LocalPlayerTab(this), "Local");
}

BluetoothPlayerTab::BluetoothPlayerTab(QWidget *parent) : QWidget(parent)
{
    this->bluetooth = Bluetooth::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(this->track_widget());
    layout->addWidget(this->controls_widget());
}

QWidget *BluetoothPlayerTab::track_widget()
{
    BluezQt::MediaPlayerPtr media_player = this->bluetooth->get_media_player().second;

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *artist_hdr = new QLabel("Artist", widget);
    QLabel *artist = new QLabel((media_player != nullptr) ? media_player->track().artist() : QString(), widget);
    artist->setIndent(16);
    layout->addWidget(artist_hdr);
    layout->addWidget(artist);

    QLabel *album_hdr = new QLabel("Album", widget);
    QLabel *album = new QLabel((media_player != nullptr) ? media_player->track().album() : QString(), widget);
    album->setIndent(16);
    layout->addWidget(album_hdr);
    layout->addWidget(album);

    QLabel *title_hdr = new QLabel("Title", widget);
    QLabel *title = new QLabel((media_player != nullptr) ? media_player->track().title() : QString(), widget);
    title->setIndent(16);
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

QWidget *BluetoothPlayerTab::controls_widget()
{
    BluezQt::MediaPlayerPtr media_player = this->bluetooth->get_media_player().second;
    Theme *theme = Theme::get_instance();

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *previous_button = new QPushButton(widget);
    previous_button->setFlat(true);
    previous_button->setIconSize(Theme::icon_56);
    previous_button->setIcon(theme->make_button_icon("skip_previous", previous_button));
    connect(previous_button, &QPushButton::clicked, [bluetooth = this->bluetooth]() {
        BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
        if (media_player != nullptr) media_player->previous()->waitForFinished();
    });
    layout->addWidget(previous_button);

    QPushButton *play_button = new QPushButton(widget);
    play_button->setFlat(true);
    play_button->setCheckable(true);
    bool status = (media_player != nullptr) ? media_player->status() == BluezQt::MediaPlayer::Status::Playing : false;
    play_button->setChecked(status);
    play_button->setIconSize(Theme::icon_56);
    play_button->setIcon(theme->make_button_icon("play", play_button, "pause"));
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
    layout->addWidget(play_button);

    QPushButton *forward_button = new QPushButton(widget);
    forward_button->setFlat(true);
    forward_button->setIconSize(Theme::icon_56);
    forward_button->setIcon(theme->make_button_icon("skip_next", forward_button));
    connect(forward_button, &QPushButton::clicked, [bluetooth = this->bluetooth]() {
        BluezQt::MediaPlayerPtr media_player = bluetooth->get_media_player().second;
        if (media_player != nullptr) media_player->next()->waitForFinished();
    });
    layout->addWidget(forward_button);

    return widget;
}

RadioPlayerTab::RadioPlayerTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();
    this->tuner = new Tuner();

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addStretch();
    layout->addWidget(this->tuner_widget());
    layout->addStretch();
    layout->addWidget(this->controls_widget());
    layout->addStretch();
}

QWidget *RadioPlayerTab::tuner_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    this->tuner->setParent(widget);

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

QWidget *RadioPlayerTab::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *scan_reverse_button = new QPushButton(widget);
    scan_reverse_button->setFlat(true);
    scan_reverse_button->setIconSize(Theme::icon_56);
    scan_reverse_button->setIcon(this->theme->make_button_icon("fast_rewind", scan_reverse_button));
    connect(scan_reverse_button, &QPushButton::clicked,
            [tuner = this->tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 5); });
    layout->addStretch();
    layout->addWidget(scan_reverse_button);

    QPushButton *prev_station_button = new QPushButton(widget);
    prev_station_button->setFlat(true);
    prev_station_button->setIconSize(Theme::icon_56);
    prev_station_button->setIcon(this->theme->make_button_icon("skip_previous", prev_station_button));
    connect(prev_station_button, &QPushButton::clicked,
            [tuner = this->tuner]() { tuner->setSliderPosition(tuner->sliderPosition() - 1); });
    layout->addStretch();
    layout->addWidget(prev_station_button);

    QPushButton *next_station_button = new QPushButton(widget);
    next_station_button->setFlat(true);
    next_station_button->setIconSize(Theme::icon_56);
    next_station_button->setIcon(this->theme->make_button_icon("skip_next", next_station_button));
    connect(next_station_button, &QPushButton::clicked,
            [tuner = this->tuner]() { tuner->setSliderPosition(tuner->sliderPosition() + 1); });
    layout->addStretch();
    layout->addWidget(next_station_button);

    QPushButton *scan_forward_button = new QPushButton(widget);
    scan_forward_button->setFlat(true);
    scan_forward_button->setIconSize(Theme::icon_56);
    scan_forward_button->setIcon(this->theme->make_button_icon("fast_forward", scan_forward_button));
    connect(scan_forward_button, &QPushButton::clicked,
            [tuner = this->tuner]() { tuner->setSliderPosition(tuner->sliderPosition() + 5); });
    layout->addStretch();
    layout->addWidget(scan_forward_button);

    layout->addStretch();
    layout->addWidget(Theme::br(widget, true));

    QPushButton *mute_button = new QPushButton(widget);
    mute_button->setFlat(true);
    mute_button->setCheckable(true);
    mute_button->setChecked(this->config->get_radio_muted());
    mute_button->setIconSize(Theme::icon_42);
    mute_button->setIcon(this->theme->make_button_icon("volume_off", mute_button));
    connect(mute_button, &QPushButton::clicked,
            [config = this->config](bool checked = false) { config->set_radio_muted(checked); });
    layout->addStretch();
    layout->addWidget(mute_button);
    layout->addStretch();

    return widget;
}

LocalPlayerTab::LocalPlayerTab(QWidget *parent) : QWidget(parent)
{
    this->config = Config::get_instance();

    QMediaPlaylist *playlist = new QMediaPlaylist(this);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    this->player = new QMediaPlayer(this);
    this->player->setPlaylist(playlist);

    this->path_label = new QLabel(this->config->get_media_home(), this);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(this->path_label);
    layout->addWidget(this->playlist_widget());
    layout->addWidget(this->seek_widget());
    layout->addWidget(this->controls_widget());
}

QWidget *LocalPlayerTab::playlist_widget()
{
    Theme *theme = Theme::get_instance();

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QString root_path(config->get_media_home());

    QPushButton *home_button = new QPushButton(widget);
    home_button->setFlat(true);
    home_button->setCheckable(true);
    home_button->setChecked(this->config->get_media_home() == root_path);
    home_button->setIconSize(Theme::icon_32);
    home_button->setIcon(theme->make_button_icon("playlist_add", home_button, "playlist_add_check"));
    connect(home_button, &QPushButton::clicked, [this](bool checked = false) {
        this->config->set_media_home(checked ? this->path_label->text() : QDir().absolutePath());
    });
    layout->addWidget(home_button, 0, Qt::AlignTop);

    QListWidget *folders = new QListWidget(widget);
    Theme::to_touch_scroller(folders);
    this->populate_dirs(root_path, folders);
    layout->addWidget(folders, 1);

    QListWidget *tracks = new QListWidget(widget);
    Theme::to_touch_scroller(tracks);
    this->populate_tracks(root_path, tracks);
    connect(tracks, &QListWidget::itemClicked, [tracks, player = this->player](QListWidgetItem *item) {
        player->playlist()->setCurrentIndex(tracks->row(item));
        player->play();
    });
    connect(this->player->playlist(), &QMediaPlaylist::currentIndexChanged, [tracks](int idx) {
        if (idx < 0) return;
        tracks->setCurrentRow(idx);
    });
    connect(folders, &QListWidget::itemClicked, [this, folders, tracks, home_button](QListWidgetItem *item) {
        if (!item->isSelected()) return;

        tracks->clear();
        this->player->playlist()->clear();
        QString current_path(item->data(Qt::UserRole).toString());
        this->path_label->setText(current_path);
        this->populate_tracks(current_path, tracks);
        this->populate_dirs(current_path, folders);

        home_button->setChecked(this->config->get_media_home() == current_path);
    });
    layout->addWidget(tracks, 2);

    return widget;
}

QWidget *LocalPlayerTab::seek_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(0, 0);
    QLabel *value = new QLabel(LocalPlayerTab::durationFmt(slider->value()), widget);
    connect(slider, &QSlider::sliderReleased,
            [player = this->player, slider]() { player->setPosition(slider->sliderPosition()); });
    connect(slider, &QSlider::sliderMoved,
            [value](int position) { value->setText(LocalPlayerTab::durationFmt(position)); });
    connect(this->player, &QMediaPlayer::durationChanged, [slider](qint64 duration) {
        slider->setValue(0);
        slider->setRange(0, duration);
    });
    connect(this->player, &QMediaPlayer::positionChanged, [slider, value](qint64 position) {
        if (!slider->isSliderDown()) {
            slider->setValue(position);
            value->setText(LocalPlayerTab::durationFmt(position));
        }
    });

    layout->addStretch(4);
    layout->addWidget(slider, 28);
    layout->addWidget(value, 3);
    layout->addStretch(1);

    return widget;
}

QWidget *LocalPlayerTab::controls_widget()
{
    Theme *theme = Theme::get_instance();

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *previous_button = new QPushButton(widget);
    previous_button->setFlat(true);
    previous_button->setIconSize(Theme::icon_56);
    previous_button->setIcon(theme->make_button_icon("skip_previous", previous_button));
    connect(previous_button, &QPushButton::clicked, [player = this->player]() {
        if (player->playlist()->currentIndex() < 0) player->playlist()->setCurrentIndex(0);
        player->playlist()->previous();
        player->play();
    });
    layout->addWidget(previous_button);

    QPushButton *play_button = new QPushButton(widget);
    play_button->setFlat(true);
    play_button->setCheckable(true);
    play_button->setChecked(false);
    play_button->setIconSize(Theme::icon_56);
    play_button->setIcon(theme->make_button_icon("play", play_button, "pause"));
    connect(play_button, &QPushButton::clicked, [player = this->player, play_button](bool checked = false) {
        play_button->setChecked(!checked);
        if (checked)
            player->play();
        else
            player->pause();
    });
    connect(this->player, &QMediaPlayer::stateChanged,
            [play_button](QMediaPlayer::State state) { play_button->setChecked(state == QMediaPlayer::PlayingState); });
    layout->addWidget(play_button);

    QPushButton *forward_button = new QPushButton(widget);
    forward_button->setFlat(true);
    forward_button->setIconSize(Theme::icon_56);
    forward_button->setIcon(theme->make_button_icon("skip_next", forward_button));
    connect(forward_button, &QPushButton::clicked, [player = this->player]() {
        player->playlist()->next();
        player->play();
    });
    layout->addWidget(forward_button);

    return widget;
}

QString LocalPlayerTab::durationFmt(int total_ms)
{
    int mins = (total_ms / (1000 * 60)) % 60;
    int secs = (total_ms / 1000) % 60;

    return QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}

void LocalPlayerTab::populate_dirs(QString path, QListWidget *dirs_widget)
{
    dirs_widget->clear();
    QDir current_dir(path);
    QFileInfoList dirs = current_dir.entryInfoList(QDir::AllDirs | QDir::Readable);
    for (QFileInfo dir : dirs) {
        if (dir.fileName() == ".") continue;

        QListWidgetItem *item = new QListWidgetItem(dir.fileName(), dirs_widget);
        if (dir.fileName() == "..") {
            item->setText("↲");

            if (current_dir.isRoot()) item->setFlags(Qt::NoItemFlags);
        }
        else {
            item->setText(dir.fileName());
        }
        item->setData(Qt::UserRole, QVariant(dir.absoluteFilePath()));
    }
}

void LocalPlayerTab::populate_tracks(QString path, QListWidget *tracks_widget)
{
    QStringList tracks = QDir(path).entryList(QStringList() << "*.mp3", QDir::Files | QDir::Readable);
    for (QString track : tracks) {
        if (this->player->playlist()->addMedia(QMediaContent(QUrl::fromLocalFile(path + '/' + track)))) {
            TagLib::FileRef f(std::string(path.toStdString() + "/" + track.toStdString()).c_str());
            if (!f.isNull() && f.tag()) {
                TagLib::Tag *tag = f.tag();
                tag->title();
            }
            int lastPoint = track.lastIndexOf(".");
            QString fileNameNoExt = track.left(lastPoint);
            new QListWidgetItem(fileNameNoExt, tracks_widget);
        }
    }
}
