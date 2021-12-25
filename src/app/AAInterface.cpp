#include "AAInterface.hpp"


AAInterface::AAInterface() :
QObject()
{

}

AAInterface::~AAInterface()
{

}

void AAInterface::injectButtonPressHelper(aasdk::proto::enums::ButtonCode::Enum buttonCode, Action::ActionState actionState)
{
    switch(actionState)
    {
        case(Action::ActionState::Activated):
            this->injectButtonPress(buttonCode, openauto::projection::ButtonEventType::PRESS);
            break;
        case(Action::ActionState::Deactivated):
            this->injectButtonPress(buttonCode, openauto::projection::ButtonEventType::RELEASE);
            break;
        case(Action::ActionState::Triggered):
        default:
            this->injectButtonPress(buttonCode, openauto::projection::ButtonEventType::NONE);
            break;   
    }
}

void AAInterface::mediaPlaybackUpdate(const aasdk::proto::messages::MediaInfoChannelPlaybackData& playback)
{
    emit aa_media_playback_update(playback);
}

void AAInterface::mediaMetadataUpdate(const aasdk::proto::messages::MediaInfoChannelMetadataData& metadata)
{
    emit aa_media_metadata_update(metadata);
}


void AAInterface::navigationStatusUpdate(const aasdk::proto::messages::NavigationStatus& navStatus)
{
    emit aa_navigation_status_update(navStatus);
}
void AAInterface::navigationTurnEvent(const aasdk::proto::messages::NavigationTurnEvent& turnEvent)
{
    emit aa_navigation_turn_event(turnEvent);
}
void AAInterface::navigationDistanceEvent(const aasdk::proto::messages::NavigationDistanceEvent& distanceEvent)
{
    emit aa_navigation_distance_event(distanceEvent);
}

AAInterface *AAInterface::get_instance()
{
    static AAInterface aa_interface;
    return &aa_interface;
}
