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
#include "app/theme.hpp"
#include "DashLog.hpp"


class CameraPage : public QWidget {
    Q_OBJECT

   public:
    CameraPage(QWidget *parent = nullptr);

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
    QSize choose_video_resolution();
    void count_down();
    void connect_cam();

    Theme *theme;
    Config *config;
    QLabel *status;
    QMediaPlayer *player;
    QList<QPair<QString,QString>> local_cams;
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

    static GstPadProbeReturn convertProbe(GstPad* pad, GstPadProbeInfo* info, void*);
    static gboolean busCallback(GstBus*, GstMessage* message, gpointer*);
    void showEvent(QShowEvent *event);

    QStackedLayout* cam_stack;
    QGst::ElementPtr videoSink_;
    QQuickWidget* videoWidget_;
    GstElement* vidPipeline_;
    GstAppSrc* vidSrc_;
    QWidget* videoContainer_;
    QGst::Quick::VideoSurface* surface_;
    class VideoContainer : public QWidget {
        public:
         VideoContainer(QWidget *parent = nullptr, CameraPage *page = nullptr);
        private:
         void resizeEvent(QResizeEvent *event);
         CameraPage * page;
    };
    class Settings : public QWidget {
       public:
        Settings(QStackedLayout *cam_stack, QTimer *reconnect_timer, QWidget *parent = nullptr);

       private:
        QStackedLayout* cam_stack;
        QTimer* reconnect_timer;
        QWidget *settings_widget();
        QBoxLayout *camera_overlay_row_widget();
        QBoxLayout *camera_overlay_width_row_widget();
        QBoxLayout *camera_overlay_height_row_widget();
        QBoxLayout *camera_overlay_width_widget();
        QBoxLayout *camera_overlay_height_widget();
        QBoxLayout *camera_network_toggle_row_widget();
        QBoxLayout *camera_autoconnect_toggle_row_widget();

        Config *config;
    };

   signals:
    void connected_network();
    void connected_local();
    void disconnected();
    void autoconnect_disabled();
    void next_cam();
    void prev_cam();
};
