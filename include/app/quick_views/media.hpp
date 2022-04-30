#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QFrame>
#include "app/quick_views/quick_view.hpp"
#include "AAHandler.hpp"

class Arbiter;
class MediaWidget;

class MediaQuickView : public QFrame, public QuickView {
    Q_OBJECT

   public:
    MediaQuickView(Arbiter &arbiter);
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QLabel *album_art_label;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *prev_button;
    QPushButton *play_button;
    QPushButton *next_button;
    QSpacerItem *horizontalSpacer_6;
    QLabel *track_progress_label;
    QLabel *title_label;
    QProgressBar *progressBar;
    MediaWidget *mediaWidget;

    void init() override;

};

class MediaWidget : public QWidget
{
    Q_OBJECT

public:
    MediaWidget(Arbiter &arbiter, QWidget *parent = 0);
    void clear();

public slots:
    void updateMetadata(const aasdk::proto::messages::MediaInfoChannelMetadataData& metadata);
    void updatePlayback(const aasdk::proto::messages::MediaInfoChannelPlaybackData& playback);

protected: 
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    Arbiter &arbiter;
    aasdk::proto::messages::MediaInfoChannelMetadataData metadata_;
    aasdk::proto::messages::MediaInfoChannelPlaybackData playback_;
    bool connected = false;
    QPoint title_pos;
    bool scroll_text_left = true;
    QTimer *scroll_timer;
    QRect play_rect;
    QRect prev_rect;
    QRect next_rect;
};




