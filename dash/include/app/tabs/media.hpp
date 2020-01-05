#include <QtWidgets>

class MediaTab : public QWidget {
    Q_OBJECT

   public:
    MediaTab(QWidget *parent = 0);

   private slots:
    void toggle_play(bool checked = false);
    void back();
    void forward();
    void update_media_player_status(BluezQt::MediaPlayer::Status);
    void update_media_player_track(BluezQt::MediaPlayerTrack);

   private:
    Bluetooth *bluetooth;
    QPushButton *play_button;
    QLabel *artist;
    QLabel *album;
    QLabel *title;

    void set_media_stats();
};
