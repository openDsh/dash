#pragma once

#include <QPair>
#include <QtWidgets>
#include <QPluginLoader>

#include "obd/obd.hpp"
#include "app/widgets/selector.hpp"
#include "app/widgets/dialog.hpp"

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

   private:
    QString format_value(double value);
    QString null_value();

    bool si;
    int rate;
    int precision;
    QTimer *timer;

   signals:
    void toggle_unit(bool si);
};

class VehiclePage : public QTabWidget {
    Q_OBJECT

   public:
    VehiclePage(QWidget *parent = nullptr);

   private:
    void get_plugins();
    void load_plugin();
    QWidget *dialog_body();
    QWidget *can_bus_toggle_row();
    QWidget *interface_selector_row();

    QMap<QString, int> capabilities;
    QMap<QString, QFileInfo> plugins;
    QStringList can_devices;
    QStringList serial_devices;
    QPluginLoader *active_plugin;
    Selector *plugin_selector;
    Config *config;
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

