#pragma once

#include <QPair>
#include <QtWidgets>
#include <QPluginLoader>

#include "canbus/socketcanbus.hpp"
#include "obd/message.hpp"
#include "obd/command.hpp"
#include "app/widgets/selector.hpp"
#include "app/widgets/dialog.hpp"

#include "app/pages/page.hpp"

class Arbiter;

typedef QPair<QString, QString> units_t;

class Gauge : public QWidget {
    Q_OBJECT

   public:
    enum Orientation { BOTTOM, RIGHT };

    Gauge(Command cmd, QFont value_font, QFont unit_font, Orientation orientation, QWidget *parent = nullptr);

    inline QString get_id() { return this->id; };
    void set_value(int value);

   private:
    QString format_value(double value);
    QString null_value();
    QLabel *value_label;

    unit_converter_t converter;

    QString id;
    bool si;
    int precision;
    units_t units;

   signals:
    void toggle_unit(bool si);
};

class VehiclePage : public QTabWidget, public Page {
    Q_OBJECT

   public:
    VehiclePage(Arbiter &arbiter, QWidget *parent = nullptr);

    void init() override;

   private:
    void get_plugins();
    void load_plugin();
    QWidget *dialog_body();
    QWidget *can_bus_toggle_row();
    QWidget *si_units_row_widget();

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
    DataTab(Arbiter &arbiter, QWidget *parent = nullptr);

   private:
    Arbiter &arbiter;
    QWidget *speedo_tach_widget();
    // QWidget *mileage_data_widget();
    QWidget *engine_data_widget();
    QWidget *vehicle_data_widget(Command cfg);

    std::vector<Gauge *> gauges;
};

