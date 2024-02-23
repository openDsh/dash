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
struct font_size_t
{
    int label;
    int value;
    int unit;
};
typedef std::function<double(double, bool)> unit_converter_t;

struct GaugeConfig
{
    QString id;
    QString description;
    units_t units;
    font_size_t font_size;
    int precision;
    unit_converter_t converter;
};

// typedef QList<Gauge> Gauges;
struct GaugesConfig
{
    GaugeConfig AUTONOMIA;
    GaugeConfig COOLANT_TEMP;
    GaugeConfig RPM;
    GaugeConfig SPEED;
    GaugeConfig INTAKE_TEMP;
    GaugeConfig EXT_TEMP;
    GaugeConfig VOLT;
    GaugeConfig MAF;
    GaugeConfig MAP;
    GaugeConfig APP;
    GaugeConfig TPAPWM;
    GaugeConfig INJ;
    GaugeConfig TVENT;
    GaugeConfig TANK;
    GaugeConfig IGNANGLE;
    GaugeConfig BATT;
    GaugeConfig RITCYL1;
    GaugeConfig RITCYL2;
    GaugeConfig RITCYL3;
    GaugeConfig RITCYL4;
    GaugeConfig SFT;
    GaugeConfig LMB1;
    GaugeConfig LMB2;
    GaugeConfig LSOIL;
    GaugeConfig LSLVLCOOLANT;
    GaugeConfig LSBENZ;
    GaugeConfig LSVOLT;
    GaugeConfig LSINIEZS;
    GaugeConfig LSINIEZH;
    GaugeConfig LSINIEZKM;
};

class Gauge : public QWidget
{
    Q_OBJECT

public:
    enum Orientation
    {
        BOTTOM,
        RIGHT
    };

    Gauge(GaugeConfig cfg, QFont value_font, QFont unit_font, Orientation orientation, QWidget *parent = nullptr);

    inline QString get_id() { return this->id; };
    void set_value(double value);

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

class VehiclePage : public QTabWidget, public Page
{
    Q_OBJECT

public:
    VehiclePage(Arbiter &arbiter, QWidget *parent = nullptr);
    QWidget *obd;

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

class DataTab : public QWidget
{
    Q_OBJECT

public:
    DataTab(Arbiter &arbiter, QWidget *parent = nullptr);

private:
    Arbiter &arbiter;
    QWidget *speedo_tach_widget();
    QWidget *engine_data_widget();
    QWidget *vehicle_data_widget(GaugeConfig cfg);

    std::vector<Gauge *> gauges;
};

class Obd1Tab : public QWidget
{
    Q_OBJECT

public:
    Obd1Tab(Arbiter &arbiter, QWidget *parent = nullptr);

private:
    Arbiter &arbiter;
    QWidget *obd_data_widget(int colonna);
    QWidget *vehicle_data_widget(GaugeConfig cfg);

    std::vector<Gauge *> gauges;
};

class Obd2Tab : public QWidget
{
    Q_OBJECT

public:
    Obd2Tab(Arbiter &arbiter, QWidget *parent = nullptr);

private:
    Arbiter &arbiter;
    QWidget *obd_data_widget(int colonna);
    QWidget *vehicle_data_widget(GaugeConfig cfg);

    std::vector<Gauge *> gauges;
};

class LSTab : public QWidget
{
    Q_OBJECT

public:
    LSTab(Arbiter &arbiter, QWidget *parent = nullptr);

private:
    Arbiter &arbiter;
    QWidget *ls_data_widget(int colonna);
    QWidget *vehicle_data_widget(GaugeConfig cfg);

    std::vector<Gauge *> gauges;
};

class ACTab : public QWidget
{
    Q_OBJECT

public:
    ACTab(Arbiter &arbiter, QWidget *parent = nullptr);

private:
    Arbiter &arbiter;
    QWidget *aq_row_widget();
    QWidget *aq_selector_widget();
    QWidget *ac_row_widget();
    QWidget *ac_switch_widget();
};
