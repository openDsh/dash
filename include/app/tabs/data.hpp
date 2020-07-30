#pragma once

#include <QPair>
#include <QtWidgets>

#include "canbus/socketcanbus.hpp"
#include "canbus/vehicleinterface.hpp"
#include "obd/message.hpp"
#include "obd/command.hpp"

typedef std::function<double(double, bool)> obd_decoder_t;
typedef QPair<QString, QString> units_t;

class Gauge : public QWidget {
    Q_OBJECT

   public:
    enum Orientation { BOTTOM, RIGHT };

    Gauge(units_t units, QFont value_font, QFont unit_font, Orientation orientation, int rate,
          std::vector<Command> cmds, int precision, obd_decoder_t decoder, QWidget *parent = nullptr);

    inline void start() { this->timer->start(this->rate); }
    inline void stop() { this->timer->stop(); }
    void can_callback(QByteArray payload);

   private:
    QString format_value(double value);
    QString null_value();
    QLabel *value_label;

    obd_decoder_t decoder;
    std::vector<Command> cmds;

    bool si;
    int rate;
    int precision;
    QTimer *timer;

   signals:
    void toggle_unit(bool si);
};

class DataTab : public QWidget {
    Q_OBJECT

   public:
    DataTab(QWidget *parent = nullptr);

   private:
    QWidget *driving_data_widget();
    QWidget *speedo_tach_widget();
    QWidget *mileage_data_widget();
    QWidget *engine_data_widget();
    QWidget *coolant_temp_widget();
    QWidget *engine_load_widget();

    std::vector<Gauge *> gauges;
};

