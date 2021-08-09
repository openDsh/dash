#pragma once

#include <QPair>
#include <QtWidgets>
#include <QPluginLoader>

#include "canbus/socketcanbus.hpp"
#include "obd/message.hpp"
#include "obd/conversions.hpp"

#include "app/widgets/selector.hpp"
#include "app/widgets/dialog.hpp"
#include "app/pages/page.hpp"

class Arbiter;

typedef QPair<QString, QString> units_t;
struct font_size_t {
    int label;
    int value;
    int unit;
};
typedef std::function<double(double, bool)> unit_converter_t;

struct GaugeConfig {
    QString id;
    QString description;
    units_t units;
    font_size_t font_size;
    int precision;
    unit_converter_t converter;
};

// typedef QList<Gauge> Gauges;
struct GaugesConfig {
    GaugeConfig LOAD;
    GaugeConfig COOLANT_TEMP;
    GaugeConfig RPM;
    GaugeConfig SPEED;
    GaugeConfig INTAKE_TEMP;
    GaugeConfig MAF;
};

class Gauge : public QWidget {
    Q_OBJECT

   public:
    enum Orientation { BOTTOM, RIGHT };

    Gauge(GaugeConfig cfg, QFont value_font, QFont unit_font, Orientation orientation, QWidget *parent = nullptr);

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
    QWidget *vehicle_data_widget(GaugeConfig cfg);

    std::vector<Gauge *> gauges;
};

