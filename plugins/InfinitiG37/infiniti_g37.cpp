#include "infiniti_g37.hpp"

Theme* InfinitiG37::theme = nullptr;

bool InfinitiG37::init(ICANBus* canbus){
    std::function<void(QByteArray)> headlightCallback = monitorHeadlightStatus;
    canbus->registerFrameHandler(0x60D, headlightCallback);
    std::function<void(QByteArray)> climateControlCallback = updateClimateDisplay;
    this->theme = Theme::get_instance();
    G37_LOG(info)<<"loaded successfully";
    return true;
}

// 60D
// FIRST BYTE:
//     headlights:
//         08 = OFF
//         0E = AUTO (ON)
//         0C = SIDES
//         0E = ON
// SECOND BYTE:
//     foglights:
//         06 = OFF
//         07 = ON

void InfinitiG37::monitorHeadlightStatus(QByteArray payload){
    if(payload.at(0) == 0x0E){
        //headlights are ON - turn to dark mode
        if(theme->get_mode() != true){
            theme->set_mode(true);
        }
    }
    else{
        //headlights are off or not fully on - make sure is light mode
        if(theme->get_mode() != false){
            theme->set_mode(false);
        }
    }
}

// 54B
// 54B  03 08 80 02 04 00 00 01 


// 1st byte 
//  AC status
//  0x03: OFF
//  0x02 ON - cold?
// 3rd byte:
//   mode:
//    A0 defrost+leg
//    88 head
//    90 head+feet
//    98 feet
//    A8 defrost
// 4th byte:
//  duel climate:
//      61: on & recirculation?
//      01: off & recirculation
//      62: on
//      02: off
//      
// 5th byte:
//    fan:
//     04 off
//     0C 1
//     14 2
//     1C 3
//     24 4
//     2C 5
//     34 6
//     3C 7
//



// 542  00 5A 00 00 00 00 00 01 | 90F
// 542  00 3C 00 00 00 00 00 01 | 60F
// SECOND BYTE:
//     temperature goal:
//         1:1 to temperature in F
// THIRD BYTE:
//      duel climate temp goal

void InfinitiG37::updateClimateDisplay(QByteArray payload){
    //theme->temperature = payload.at(1);
}

