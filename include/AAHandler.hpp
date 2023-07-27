#pragma once

#include "openauto/Service/IAndroidAutoInterface.hpp"
#include "app/action.hpp"

class AAHandler : public QObject, public openauto::service::IAndroidAutoInterface {
    Q_OBJECT

   public:
    AAHandler();
    ~AAHandler();
    void mediaPlaybackUpdate(const aasdk::proto::messages::MediaInfoChannelPlaybackData& playback) override;
    void mediaMetadataUpdate(const aasdk::proto::messages::MediaInfoChannelMetadataData& metadata) override;
    void navigationStatusUpdate(const aasdk::proto::messages::NavigationStatus& navStatus) override;
    void navigationTurnEvent(const aasdk::proto::messages::NavigationTurnEvent& turnEvent) override;
    void navigationDistanceEvent(const aasdk::proto::messages::NavigationDistanceEvent& distanceEvent) override;
    void injectButtonPressHelper(aasdk::proto::enums::ButtonCode::Enum buttonCode, Action::ActionState actionState);

   private:

   signals:
    void aa_media_metadata_update(const aasdk::proto::messages::MediaInfoChannelMetadataData& metadata);
    void aa_media_playback_update(const aasdk::proto::messages::MediaInfoChannelPlaybackData& playback);
    void aa_navigation_status_update(const aasdk::proto::messages::NavigationStatus& navStatus);
    void aa_navigation_turn_event(const aasdk::proto::messages::NavigationTurnEvent& turnEvent);
    void aa_navigation_distance_event(const aasdk::proto::messages::NavigationDistanceEvent& distanceEvent);
};