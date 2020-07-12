#pragma once

#include <QLabel>
#include <QMediaPlayer>
#include <QStackedLayout>
#include <QString>
#include <QVideoWidget>

#include "app/config.hpp"
#include "app/theme.hpp"

class CameraTab : public QWidget {
    Q_OBJECT

   public:
    CameraTab(QWidget *parent = nullptr);

   private:
    QWidget *connect_widget();
    QWidget *camera_widget();
    QWidget *input_widget();
    void connect_stream();
    void update_status(QMediaPlayer::MediaStatus media_status);

    Theme *theme;
    Config *config;
    QLabel *status;
    QMediaPlayer *player;
    QString url;

   signals:
    void connected();
    void disconnected();
};
