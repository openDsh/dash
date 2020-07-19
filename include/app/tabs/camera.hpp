#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <QLabel>
#include <QMediaPlayer>
#include <QStackedLayout>
#include <QString>
#include <QVideoWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QCamera>
#include <QCheckBox>
#include <QCameraViewfinder>
#include <QCameraViewfinderSettings>

#include <app/config.hpp>
#include <app/theme.hpp>

class CameraTab : public QWidget {
    Q_OBJECT

   public:
    CameraTab(QWidget *parent = nullptr);

   private:
    QWidget *connect_widget();
    QWidget *network_camera_widget();
    QWidget *local_camera_widget();
    QWidget *camera_selector();
    QPushButton *connect_button();
    QWidget *input_widget();
    QWidget *selector_widget(QWidget *selection);
    bool populate_local_cams();
    void connect_network_stream();
    void connect_local_stream();
    bool local_cam_available(const QString& device);
    void update_network_status(QMediaPlayer::MediaStatus media_status);
    void update_local_status(QCamera::Status status);
    void choose_video_resolution();

    Theme *theme;
    Config *config;
    QLabel *status;
    QMediaPlayer *player;
    QList<QPair<QString,QString>> local_cams;
    QComboBox *cams_dropdown;
    QCameraViewfinder *local_video_widget;
    QCameraViewfinderSettings local_cam_settings;
    QCamera *local_cam;
    int local_index;

   signals:
    void connected_network();
    void connected_local();
    void disconnected();
    void next_cam();
    void prev_cam();
};

#endif
