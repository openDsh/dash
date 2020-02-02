#ifndef DATA_HPP_
#define DATA_HPP_

#include <QPair>
#include <QtWidgets>

#include <obd/obd.hpp>

typedef std::function<double(std::vector<double>, bool)> obd_decoder_t;
typedef QPair<QString, QString> units_t;

class Gauge : public QWidget {
    Q_OBJECT

   public:
    enum Orientation { BOTTOM, RIGHT };

    Gauge(units_t units, QFont value_font, QFont unit_font, Orientation orientation, int rate,
          std::vector<Command> cmds, int precision, obd_decoder_t decoder, QWidget *parent = nullptr);

    inline void start() { this->timer->start(this->rate); }
    inline void stop() { this->timer->stop(); }

   signals:
    void toggle_unit(bool si);

   private:
    QString format_value(double value);
    QString null_value();

    bool si;
    int rate;
    int precision;

    QTimer *timer;
};

class DataTab : public QWidget {
    Q_OBJECT

   public:
    DataTab(QWidget *parent = nullptr);

    void convert_gauges(bool);

   private:
    QWidget *cluster_widget();
    QWidget *driving_data_widget();
    QWidget *speedo_tach_widget();
    QWidget *mileage_data_widget();
    QWidget *engine_data_widget();
    QWidget *coolant_temp_widget();
    QWidget *engine_load_widget();

    std::vector<Gauge *> gauges;
};

#endif
