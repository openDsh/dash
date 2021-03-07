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

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
#include <QGlib/Error>
#include <QGlib/Connect>
#include <QGst/Init>
#include <QGst/Bus>
#include <QGst/Pipeline>
#include <QGst/Parse>
#include <QGst/Message>
#include <QGst/Utils/ApplicationSink>
#include <QGst/Utils/ApplicationSource>
#include <QGst/Ui/VideoWidget>
#include <QGst/ElementFactory>
#include <QGst/Quick/VideoSurface>
#include <QtQml/QQmlContext>
#include <QtQuickWidgets/QQuickWidget>

#include "app/config.hpp"
#include "DashLog.hpp"

#include "app/pages/page.hpp"

class Arbiter;

class CameraPage : public QWidget, public Page {
    Q_OBJECT

   public:
    CameraPage(Arbiter &arbiter, QWidget *parent = nullptr);

    void init() override;

   private:
    class VideoContainer : public QWidget {
       public:
        VideoContainer(QWidget *parent = nullptr, CameraPage *page = nullptr);

       private:
        void resizeEvent(QResizeEvent *event);

        CameraPage *page;
    };

    class Settings : public QWidget {
       public:
        Settings(Arbiter &arbiter, QWidget *parent = nullptr);
        QSize sizeHint() const override;

       private:
        QWidget *settings_widget();
        QWidget *camera_overlay_row_widget();
        QWidget *camera_overlay_width_row_widget();
        QWidget *camera_overlay_height_row_widget();
        QWidget *camera_overlay_width_widget();
        QWidget *camera_overlay_height_widget();

        Arbiter &arbiter;
        Config *config;
    };

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
    QSize choose_video_resolution();
    void count_down();
    void connect_cam();

    Config *config;
    QLabel *status;
    QMediaPlayer *player;
    QList<QPair<QString, QString>> local_cams;
    QComboBox *cams_dropdown;
    QWidget *local_video_widget;
    QWidget *remote_video_widget;
    QCameraViewfinderSettings local_cam_settings;
    QCamera *local_cam;
    QTimer *reconnect_timer;
    int reconnect_in_secs;
    QString reconnect_message;
    int local_index;

    bool connected = false;

    void init_gstreamer_pipeline(std::string vidLaunchStr_, bool sync = false);
    void disconnect_stream();

    static GstPadProbeReturn convertProbe(GstPad *pad, GstPadProbeInfo *info, void *);
    static gboolean busCallback(GstBus *, GstMessage *message, gpointer *);
    void showEvent(QShowEvent *event);

    QGst::ElementPtr videoSink_;
    QQuickWidget *videoWidget_;
    GstElement *vidPipeline_;
    GstAppSrc *vidSrc_;
    QWidget *videoContainer_;
    QGst::Quick::VideoSurface *surface_;

   signals:
    void connected_network();
    void connected_local();
    void disconnected();
    void autoconnect_disabled();
    void next_cam();
    void prev_cam();
};
