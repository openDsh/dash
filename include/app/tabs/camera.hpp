#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <QLabel>
#include <QMediaPlayer>
#include <QStackedLayout>
#include <QVideoWidget>

class CameraTab : public QWidget {
    Q_OBJECT

   public:
    CameraTab(QWidget *parent = nullptr);

   signals:
    void stream_connecting();
    void stream_connected();
    void stream_disconnected();

   public slots:
    void connect_stream(QString stream_url);
    void disconnect_stream();

   private slots:
    void changed_status(QMediaPlayer::MediaStatus);
    void new_metadata(const QString &key, const QVariant &value);

   private:
     QWidget *connect_widget();
     QWidget *cam_widget();

     QLabel *status_cam_view;
     QLabel *status_connection_overlay;
     QVideoWidget *video_widget;
     QMediaPlayer *player;
};

#endif
