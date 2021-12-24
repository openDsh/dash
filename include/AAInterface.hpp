#pragma once

#include "openauto/Service/IAndroidAutoInterface.hpp"
#include "app/action.hpp"



class AAInterface : public QObject, public openauto::service::IAndroidAutoInterface {
    Q_OBJECT

   public:
    AAInterface();
    ~AAInterface();
    void mediaPlaybackUpdate(const aasdk::proto::messages::MediaInfoChannelPlaybackData& playback) override;
    void mediaMetadataUpdate(const aasdk::proto::messages::MediaInfoChannelMetadataData& metadata) override;
    void injectButtonPressHelper(aasdk::proto::enums::ButtonCode::Enum buttonCode, Action::ActionState actionState);
    static AAInterface *get_instance();

   

   private:
    

   signals:
    void aa_media_metadata_update(const aasdk::proto::messages::MediaInfoChannelMetadataData& metadata);
    void aa_media_playback_update(const aasdk::proto::messages::MediaInfoChannelPlaybackData& playback);

};
