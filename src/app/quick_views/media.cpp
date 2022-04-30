#include <QFrame>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QDebug>

#include "app/arbiter.hpp"
#include "app/widgets/dialog.hpp"
#include "app/quick_views/media.hpp"
#include "app/utilities/icon_engine.hpp"

MediaQuickView::MediaQuickView(Arbiter &arbiter)
    : QFrame()
    , QuickView(arbiter, "Media", this)
{

}

void MediaQuickView::init(){

        AAHandler *aa_handler = arbiter.android_auto().handler;
        mediaWidget = new MediaWidget(this->arbiter,this);

        horizontalLayout = new QHBoxLayout(this);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        mediaWidget = new MediaWidget(this->arbiter, this);
        horizontalLayout->addWidget(mediaWidget);
       
        connect(aa_handler, &AAHandler::aa_media_playback_update, [this](const aasdk::proto::messages::MediaInfoChannelPlaybackData& playback){
            mediaWidget->updatePlayback(playback);
        });

        connect(aa_handler, &AAHandler::aa_media_metadata_update, [this](const aasdk::proto::messages::MediaInfoChannelMetadataData& metadata){
           mediaWidget->updateMetadata(metadata);
        });

}

MediaWidget::MediaWidget(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    ,arbiter(arbiter)
{

    scroll_timer = new QTimer(this);
    scroll_timer->start(33);//30fps
    connect(scroll_timer, &QTimer::timeout, [this](){
        
        if(metadata_.has_track_name() && metadata_.has_artist_name()){
            QString track_info(QString::fromStdString(metadata_.artist_name()) + 
            " - " +
            QString::fromStdString(metadata_.track_name()));
            QFont myFont("arial", 10);
            QFontMetrics fm(myFont);
            int text_width=fm.width(track_info);
            
            int text_rect_width = (width() / 2) - (height() / 2 + (height() * 2));

            if(text_width > text_rect_width){

                if(scroll_text_left){
                    title_pos.setX(title_pos.x() - 1);
                    if((title_pos.x() + text_width) < height() + text_rect_width){
                        scroll_text_left = false;
                    }

                } else{
                    title_pos.setX(title_pos.x() + 1);
                    if(title_pos.x() > height()){
                        scroll_text_left = true;
                    }
                }

            }
            
        }
        
        this->update();

    });

    connect(&arbiter, &Arbiter::openauto_connection_changed, [this](bool connected){

        this->connected = connected;
        
    });

}

void MediaWidget::paintEvent(QPaintEvent *event)
{

    // Setup
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    if(arbiter.theme().mode == Session::Theme::Mode::Dark){
        painter.setBackground(QBrush(QColor(48,48,48)));
        painter.setPen(QPen(Qt::white,1));
    } else {
        painter.setBackground(QBrush(QColor(250,250,250)));
        painter.setPen(QPen(Qt::black,1));
    }

    painter.eraseRect(0,0,width(),height()); 

    /*** ALBUM ART ***/
    if(this->metadata_.has_album_art() && connected){
        
          QImage art;
          art.loadFromData(QByteArray::fromStdString(metadata_.album_art()));
          QSize size = QSize(height(), height());
          auto scaled = art.scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation);
          painter.drawImage(QPoint(0,0),scaled);

    }

    /*** TRACK TITLE ***/
    int text_rect_width = (width() / 2) - (height() / 2 + (height() * 2));
    if(metadata_.has_track_name() && metadata_.has_artist_name() && connected){
        QString track_info(QString::fromStdString(metadata_.artist_name()) + 
        " - " +
        QString::fromStdString(metadata_.track_name()));
        QFont myFont("arial", 10);
        painter.setFont(myFont);
        painter.setClipRect(height(),0, text_rect_width ,height());
        painter.drawText(title_pos, track_info);
        painter.setClipping(false);
    }

    /*** PREV BUTTON ***/
    QIcon prev_icon(new IconEngine(this->arbiter, QString("://icons/skip_previous.svg"), true));
    QPixmap prev_pixmap = prev_icon.pixmap(QSize(128,128));
    painter.drawPixmap(prev_rect,prev_pixmap);

    /*** PLAY/PAUSE BUTTON ***/
    QPen progressBGPen = QPen(QColor(70,70,70),4);
    painter.setPen(progressBGPen);
    painter.drawArc(play_rect,1440,5760);

    if(this->playback_.has_track_progress() && connected){
        if(metadata_.has_track_length() && metadata_.track_length() != 0){

            QPen progressPen = QPen(arbiter.theme().color(),4);
            painter.setPen(progressPen);  
            int step =  5760 / metadata_.track_length();
            int val = step * playback_.track_progress();
            painter.drawArc(play_rect,1440,val);
        }
    }

    if(playback_.has_playback_state() && playback_.playback_state() == aasdk::proto::messages::MediaInfoChannelPlaybackData_PlaybackState_PLAY){
        QIcon icon(new IconEngine(this->arbiter, QString("://icons/pause.svg"), true));
        QPixmap pause = icon.pixmap(QSize(128,128));
        painter.drawPixmap(play_rect,pause);
       

    } else {
        QIcon icon(new IconEngine(this->arbiter, QString("://icons/play.svg"), true));
        QPixmap play = icon.pixmap(QSize(128,128));
        painter.drawPixmap(play_rect,play);
    }

    /**** NEXT BUTTON ****/
    QIcon next_icon(new IconEngine(this->arbiter, QString("://icons/skip_next.svg"), true));
    QPixmap next_pixmap = next_icon.pixmap(QSize(128,128));
    painter.drawPixmap(next_rect,next_pixmap);

}

void MediaWidget::resizeEvent(QResizeEvent *event)
{
    prev_rect = QRect((width() / 2) - (height() / 2 + height()), 8,height() - 16,height() - 16);
    play_rect = QRect((width() / 2) - (height() / 2), 8,height() - 16,height() - 16);
    next_rect = QRect((width() / 2) + (height() / 2), 8,height() - 16,height() - 16);

}

void MediaWidget::mousePressEvent(QMouseEvent *event)
{

    if(event->button()==Qt::LeftButton)
    {

        AAHandler *aa_handler = arbiter.android_auto().handler;
        if(prev_rect.contains(event->pos())){
            aa_handler->injectButtonPress(aasdk::proto::enums::ButtonCode::PREV,openauto::projection::ButtonEventType::RELEASE);
        }
        if(play_rect.contains(event->pos())){
            aa_handler->injectButtonPress(aasdk::proto::enums::ButtonCode::TOGGLE_PLAY,openauto::projection::ButtonEventType::PRESS);
            aa_handler->injectButtonPress(aasdk::proto::enums::ButtonCode::TOGGLE_PLAY,openauto::projection::ButtonEventType::RELEASE);
        }
        if(next_rect.contains(event->pos())){
            aa_handler->injectButtonPress(aasdk::proto::enums::ButtonCode::NEXT,openauto::projection::ButtonEventType::RELEASE);
        }

    }
}

void MediaWidget::updatePlayback(const aasdk::proto::messages::MediaInfoChannelPlaybackData& playback)
{

    this->playback_ = playback;
    this->update();

}

void MediaWidget::updateMetadata(const aasdk::proto::messages::MediaInfoChannelMetadataData& metadata)
{

    title_pos = QPoint(height() + 8, (height() / 2) + 5);//reset
    this->metadata_ = metadata;
    this->update();

}


