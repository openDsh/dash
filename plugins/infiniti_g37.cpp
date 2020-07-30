#include "infiniti_g37.hpp"

bool InfinitiG37::init(SocketCANBus* canbus, Theme* theme){
    std::cout<<"INIT OF G37"<<std::endl;
    std::function<void(QByteArray)> headlightCallback = monitorHeadlightStatus;
    canbus->registerFrameHandler(0x60D, headlightCallback);
    std::function<void(QByteArray)> climateControlCallback = updateClimateDisplay;
    this->theme = theme;
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
    std::cout<<"G37 Got Headlight Status"<<std::endl;
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


// 542  00 5A 00 00 00 00 00 01 | 90F
// 542  00 3C 00 00 00 00 00 01 | 60F
// SECOND BYTE:
//     temperature goal:
//         1:1 to temperature in F

void InfinitiG37::updateClimateDisplay(QByteArray payload){
    //theme->temperature = payload.at(1);
}

