#pragma once

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

#include "app/config.hpp"
#include "app/theme.hpp"

#include "app/pages/page.hpp"

class Arbiter;

class CameraPage : public QWidget, public Page {
    Q_OBJECT

   public:
    CameraPage(Arbiter &arbiter, QWidget *parent = nullptr);

   private:
    QWidget *connect_widget();
    QWidget *network_camera_widget();
    QWidget *local_camera_widget();
    QWidget *local_cam_selector();
    QPushButton *connect_button();
    QWidget *network_cam_selector();
    QWidget *selector_widget(QWidget *selection);
    void populate_local_cams();
    void connect_network_stream();
    void connect_local_stream();
    bool local_cam_available(const QString &device);
    void update_network_status(QMediaPlayer::MediaStatus media_status);
    void update_local_status(QCamera::Status status);
    void choose_video_resolution();
    void count_down();
    void connect_cam();

    Theme *theme;
    Config *config;
    QLabel *status;
    QMediaPlayer *player;
    QList<QPair<QString,QString>> local_cams;
    QComboBox *cams_dropdown;
    QCameraViewfinder *local_video_widget;
    QCameraViewfinderSettings local_cam_settings;
    QCamera *local_cam;
    QTimer *reconnect_timer;
    int reconnect_in_secs;
    QString reconnect_message;
    int local_index;

   signals:
    void connected_network();
    void connected_local();
    void disconnected();
    void autoconnect_disabled();
    void next_cam();
    void prev_cam();
};
