#include "obd2.hpp"

bool Obd2::init(ICANBus* canbus){
    if (this->arbiter) {
        OBD2_LOG(info)<<"loading plugin, totoal commands: "<<cmds.size();

        for (int i = 0; i < cmds.size(); ++i){
            canbus->registerFrameHandler(cmds.at(i).frame.frameId()+0x9, [this,i](QByteArray payload){this->readObd2(cmds.at(i), payload);});
            cmds[i].timer = new QTimer(this);
            connect(cmds.at(i).timer, &QTimer::timeout, [this, canbus, i]() {
                canbus->writeFrame(cmds.at(i).frame);
            });
            cmds.at(i).timer->start(cmds.at(i).rate);
            OBD2_LOG(info)<<"loaded "<< QString((cmds.at(i).id)).toStdString();
        }
        OBD2_LOG(info)<<"loaded successfully";
        return true;
    }
    else{
        OBD2_LOG(error)<<"Failed to get arbiter";
        return false;
    }
}

QList<QWidget *> Obd2::widgets()
{
    QList<QWidget *> tabs;
    return tabs;
}

void Obd2::readObd2(Command cmd, QByteArray payload){
    Response resp = Response(payload);
    if(cmd.frame.payload().at(2) == resp.PID){
        double value = cmd.decoder(resp);
        OBD2_LOG(info)<<QString(cmd.id).toStdString()<<": "<<value;
        if (cmd.id == "maf_rate") {
            this->maf_rate = Conversion::gps_to_lph(value);
        } else if (cmd.id == "speed") {
            const double mpg = this->maf_rate == 0 ? 0 : value / this->maf_rate;
            this->arbiter->vehicle_update_data("mpg", mpg);    
        }
        this->arbiter->vehicle_update_data(cmd.id, value);
    }
}
