#include <QPalette>
#include <QSerialPortInfo>

#include "app/config.hpp"
#include "app/pages/vehicle.hpp"
#include "app/window.hpp"
#include "plugins/vehicle_plugin.hpp"

GaugesConfig gauges_cfg =
    {
        {"autonomia", "Autonomia", {"Miles", "Km"}, {10, 16, 12}, 0, [](double x, bool _)
         { return x; }},
        {"coolant_temp", "Temperatura Refrigerante Motore", {"°F", "°C"}, {10, 16, 12}, 0, [](double x, bool si)
         { return si ? x : Conversion::c_to_f(x); }},
        {"rpm", "Giri al Minuto (RPM)", {"rpm", "rpm"}, {0, 24, 12}, 0, [](double x, bool _)
         { return x; }},
        {"speed", "Velocità", {"mph", "km/h"}, {0, 36, 16}, 0, [](double x, bool si)
         { return si ? x : Conversion::kph_to_mph(x); }},
        {"intake_temp", "Aspirazione", {"°F", "°C"}, {8, 18, 10}, 1, [](double x, bool si)
         { return si ? x : Conversion::c_to_f(x); }},
        {"ext_temp", "Temperatura Esterna", {"°F", "°C"}, {10, 16, 12}, 1, [](double x, bool si)
         { return si ? x : Conversion::c_to_f(x); }},
        {"volt", "Batteria", {"V", "V"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"maf", "Debimetro", {"g/s", "g/s"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"map", "Pressione Aspirazione (MAP)", {"kPa", "kPa"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"app", "Acceleratore", {"%", "%"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"tpapwm", "Corpo Farfallato", {"%", "%"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"inj", "Iniezione", {"ms", "ms"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"tvent", "Pompa Benzina", {"%", "%"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"tank", "Serbatoio Benzina", {"L", "L"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"ignangle", "A. Accensione", {"°CA", "°CA"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"batt", "Sensore di Detonazione", {"V", "V"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"ritcyl1", "R. Scoppio C1", {"°CA", "°CA"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"ritcyl2", "R. Scoppio C2", {"°CA", "°CA"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"ritcyl3", "R. Scoppio C3", {"°CA", "°CA"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"ritcyl4", "R. Scoppio C4", {"°CA", "°CA"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"sft", "Taglio Benzina (Carburazione)", {"%", "%"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lmb1", "Sonda Lambda 1", {"V", "V"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lmb2", "Sonda Lambda 2", {"V", "V"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lscoolant", "Antigelo", {"°C", "°C"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lsoil", "Olio motore", {"lvl", "lvl"}, {8, 18, 10}, 0, [](double x, bool si)
         { return x; }},
        {"lslvlcoolant", "Livello Antigelo", {"lvl", "lvl"}, {8, 18, 10}, 0, [](double x, bool si)
         { return x; }},
        {"lstempext", "Temperatura Esterna", {"°C", "°C"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lsbenz", "Benzina", {"L", "L"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lsvolt", "Tensione Batteria", {"V", "V"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lsiniezs", "Cons. inst.", {"ml/s", "ml/s"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lsiniezh", "Cons. orari", {"L", "L"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }},
        {"lsiniezkm", "Cons. ogni 100km", {"L", "L"}, {8, 18, 10}, 1, [](double x, bool si)
         { return x; }}
         };

Obd1Tab::Obd1Tab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent), arbiter(arbiter)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    // QWidget *driving_data = this->speedo_tach_widget();
    // layout->addWidget(driving_data);
    // layout->addWidget(Session::Forge::br(true));

    QWidget *obd_data1 = this->obd_data_widget(1);
    layout->addWidget(obd_data1);
    layout->addWidget(Session::Forge::br(true));
    QWidget *obd_data2 = this->obd_data_widget(2);
    layout->addWidget(obd_data2);
    layout->addWidget(Session::Forge::br(true));
    QWidget *obd_data3 = this->obd_data_widget(3);
    layout->addWidget(obd_data3);

    // QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // sp_left.setHorizontalStretch(5);
    // driving_data->setSizePolicy(sp_left);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(2);
    obd_data1->setSizePolicy(sp_right);
    obd_data2->setSizePolicy(sp_right);
    obd_data3->setSizePolicy(sp_right);

    connect(&this->arbiter, &Arbiter::vehicle_update_data, [this](QString gauge_id, double value)
            {
        // DASH_LOG(info)<<"[Gauges] arbiter update: "<<qPrintable(gauge_id)<<" to "<< std::to_string(value);
        for (auto &gauge : this->gauges) {
            if(gauge->get_id() == gauge_id){
                // DASH_LOG(info)<<"[Gauges] Found: "<<gauge->get_id();
                gauge->set_value(value);
            }
        } });
}

Obd2Tab::Obd2Tab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent), arbiter(arbiter)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    // QWidget *driving_data = this->speedo_tach_widget();
    // layout->addWidget(driving_data);
    // layout->addWidget(Session::Forge::br(true));

    QWidget *obd_data1 = this->obd_data_widget(1);
    layout->addWidget(obd_data1);
    layout->addWidget(Session::Forge::br(true));
    QWidget *obd_data2 = this->obd_data_widget(2);
    layout->addWidget(obd_data2);
    layout->addWidget(Session::Forge::br(true));
    QWidget *obd_data3 = this->obd_data_widget(3);
    layout->addWidget(obd_data3);

    // QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // sp_left.setHorizontalStretch(5);
    // driving_data->setSizePolicy(sp_left);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(2);
    obd_data1->setSizePolicy(sp_right);
    obd_data2->setSizePolicy(sp_right);
    obd_data3->setSizePolicy(sp_right);

    connect(&this->arbiter, &Arbiter::vehicle_update_data, [this](QString gauge_id, double value)
            {
        // DASH_LOG(info)<<"[Gauges] arbiter update: "<<qPrintable(gauge_id)<<" to "<< std::to_string(value);
        for (auto &gauge : this->gauges) {
            if(gauge->get_id() == gauge_id){
                // DASH_LOG(info)<<"[Gauges] Found: "<<gauge->get_id();
                gauge->set_value(value);
            }
        } });
}

LSTab::LSTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent), arbiter(arbiter)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    // QWidget *driving_data = this->speedo_tach_widget();
    // layout->addWidget(driving_data);
    // layout->addWidget(Session::Forge::br(true));

    QWidget *ls_data1 = this->ls_data_widget(1);
    layout->addWidget(ls_data1);
    layout->addWidget(Session::Forge::br(true));
    QWidget *ls_data2 = this->ls_data_widget(2);
    layout->addWidget(ls_data2);
    layout->addWidget(Session::Forge::br(true));
    QWidget *ls_data3 = this->ls_data_widget(3);
    layout->addWidget(ls_data3);

    // QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // sp_left.setHorizontalStretch(5);
    // driving_data->setSizePolicy(sp_left);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(2);
    ls_data1->setSizePolicy(sp_right);
    ls_data2->setSizePolicy(sp_right);
    ls_data3->setSizePolicy(sp_right);

    connect(&this->arbiter, &Arbiter::vehicle_update_data, [this](QString gauge_id, double value)
            {
        // DASH_LOG(info)<<"[Gauges] arbiter update: "<<qPrintable(gauge_id)<<" to "<< std::to_string(value);
        for (auto &gauge : this->gauges) {
            if(gauge->get_id() == gauge_id){
                // DASH_LOG(info)<<"[Gauges] Found: "<<gauge->get_id();
                gauge->set_value(value);
            }
        } });
}

Gauge::Gauge(GaugeConfig cfg, QFont value_font, QFont unit_font, Gauge::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
    Config *config = Config::get_instance();

    this->id = cfg.id;
    this->si = config->get_si_units();

    this->precision = cfg.precision;

    this->units = cfg.units;
    this->converter = cfg.converter;

    QBoxLayout *layout;
    if (orientation == BOTTOM)
        layout = new QVBoxLayout(this);
    else
        layout = new QHBoxLayout(this);

    value_label = new QLabel(this->null_value(), this);
    value_label->setFont(value_font);
    value_label->setAlignment(Qt::AlignCenter);

    QLabel *unit_label = new QLabel(this->si ? this->units.second : this->units.first, this);
    unit_label->setFont(unit_font);
    unit_label->setAlignment(Qt::AlignCenter);

    connect(config, &Config::si_units_changed, [this, unit_label](bool si)
            {
        this->si = si;
        unit_label->setText(this->si ? this->units.second : this->units.first);
        value_label->setText(this->null_value()); });

    layout->addStretch(6);
    layout->addWidget(value_label);
    layout->addStretch(1);
    layout->addWidget(unit_label);
    layout->addStretch(4);
}

void Gauge::set_value(double value)
{
    DASH_LOG(debug) << "[Gauges] set_value: " << std::to_string(value);
    value_label->setText(this->format_value(this->converter(value, this->si)));
}

QString Gauge::format_value(double value)
{
    if (this->precision == 0)
        return QString::number((int)value);
    else
        return QString::number(value, 'f', this->precision);
}

QString Gauge::null_value()
{
    QString null_str = "-";
    if (this->precision > 0)
        null_str += ".-";
    else
        null_str += '-';

    return null_str;
}

VehiclePage::VehiclePage(Arbiter &arbiter, QWidget *parent)
    : QTabWidget(parent), Page(arbiter, "Vehicle", "directions_car", true, this)
{
}

void VehiclePage::init()
{
    this->addTab(new DataTab(this->arbiter, this), "CAN1");
    this->addTab(new LSTab(this->arbiter, this), "CAN2");
    this->addTab(new Obd1Tab(this->arbiter, this), "OBD1");
    this->addTab(new Obd2Tab(this->arbiter, this), "OBD2");
    this->config = Config::get_instance();

    for (auto device : QCanBus::instance()->availableDevices("socketcan"))
        this->can_devices.append(device.name());

    for (auto port : QSerialPortInfo::availablePorts())
        this->serial_devices.append(port.systemLocation());

    this->get_plugins();
    this->active_plugin = new QPluginLoader(this);
    Dialog *dialog = new Dialog(this->arbiter, true, this->window());
    dialog->set_body(this->dialog_body());
    QPushButton *load_button = new QPushButton("load");
    connect(load_button, &QPushButton::clicked, [this]()
            { this->load_plugin(); });
    dialog->set_button(load_button);

    QPushButton *settings_button = new QPushButton(this);
    settings_button->setFlat(true);
    this->arbiter.forge().iconize("settings", settings_button, 24);
    connect(settings_button, &QPushButton::clicked, [dialog]()
            { dialog->open(); });
    this->setCornerWidget(settings_button);

    this->load_plugin();
}

QWidget *VehiclePage::dialog_body()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QStringList plugins = this->plugins.keys();
    this->plugin_selector = new Selector(plugins, this->config->get_vehicle_plugin(), this->arbiter.forge().font(14), this->arbiter, widget, "unloader");

    layout->addWidget(this->si_units_row_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(this->can_bus_toggle_row(), 1);

    QStringList devices = this->config->get_vehicle_can_bus() ? this->can_devices : this->serial_devices;
    Selector *interface_selector = new Selector(devices, this->config->get_vehicle_interface(), this->arbiter.forge().font(14), this->arbiter, widget, "disabled");
    interface_selector->setVisible((this->can_devices.size() > 0) || (this->serial_devices.size() > 0));
    connect(interface_selector, &Selector::item_changed, [config = this->config](QString item)
            { config->set_vehicle_interface(item); });
    connect(this->config, &Config::vehicle_can_bus_changed, [this, interface_selector](bool state)
            { interface_selector->set_options(state ? this->can_devices : this->serial_devices); });
    layout->addWidget(interface_selector, 1);

    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(this->plugin_selector, 1);

    return widget;
}

QWidget *VehiclePage::can_bus_toggle_row()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Interface", widget);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox();
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QRadioButton *socketcan_button = new QRadioButton("SocketCAN", group);
    socketcan_button->setChecked(this->config->get_vehicle_can_bus());
    socketcan_button->setEnabled(this->can_devices.size() > 0);
    connect(socketcan_button, &QRadioButton::clicked, [config = this->config]
            { config->set_vehicle_can_bus(true); });
    group_layout->addWidget(socketcan_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *VehiclePage::si_units_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("SI Units", widget);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->config->get_si_units());
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state)
            { config->set_si_units(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

void VehiclePage::get_plugins()
{
    for (const QFileInfo &plugin : Session::plugin_dir("vehicle").entryInfoList(QDir::Files))
    {
        if (QLibrary::isLibrary(plugin.absoluteFilePath()))
            this->plugins[Session::fmt_plugin(plugin.baseName())] = plugin;
    }
}

void VehiclePage::load_plugin()
{
    if (this->active_plugin->isLoaded())
        this->active_plugin->unload();

    QString key = this->plugin_selector->get_current();
    if (!key.isNull())
    {
        this->active_plugin->setFileName(this->plugins[key].absoluteFilePath());

        if (VehiclePlugin *plugin = qobject_cast<VehiclePlugin *>(this->active_plugin->instance()))
        {
            plugin->dashize(&this->arbiter);
            plugin->init(((SocketCANBus *)SocketCANBus::get_instance()));
            for (QWidget *tab : plugin->widgets())
                this->addTab(tab, tab->objectName());
        }
    }
    this->config->set_vehicle_plugin(key);
}

DataTab::DataTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent), arbiter(arbiter)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    QWidget *driving_data = this->speedo_tach_widget();
    layout->addWidget(driving_data);
    layout->addWidget(Session::Forge::br(true));

    QWidget *engine_data = this->engine_data_widget();
    layout->addWidget(engine_data);

    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch(5);
    driving_data->setSizePolicy(sp_left);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(2);
    engine_data->setSizePolicy(sp_right);

    connect(&this->arbiter, &Arbiter::vehicle_update_data, [this](QString gauge_id, double value)
            {
        // DASH_LOG(info)<<"[Gauges] arbiter update: "<<qPrintable(gauge_id)<<" to "<< std::to_string(value);
        for (auto &gauge : this->gauges) {
            if(gauge->get_id() == gauge_id){
                // DASH_LOG(info)<<"[Gauges] Found: "<<gauge->get_id();
                gauge->set_value(value);
            }
        } });
}

QWidget *DataTab::speedo_tach_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addStretch(3);
    layout->addWidget(this->vehicle_data_widget(gauges_cfg.SPEED));
    layout->addStretch(2);
    layout->addWidget(this->vehicle_data_widget(gauges_cfg.RPM));
    layout->addStretch(1);

    return widget;
}

QWidget *DataTab::engine_data_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(this->vehicle_data_widget(gauges_cfg.COOLANT_TEMP));
    layout->addStretch();
    layout->addWidget(Session::Forge::br());
    layout->addStretch();
    layout->addWidget(this->vehicle_data_widget(gauges_cfg.AUTONOMIA));
    layout->addStretch();
    layout->addWidget(Session::Forge::br());
    layout->addStretch();
    layout->addWidget(this->vehicle_data_widget(gauges_cfg.EXT_TEMP));
    layout->addStretch();

    return widget;
}

QWidget *Obd1Tab::obd_data_widget(int colonna)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    switch (colonna)
    {
    case 1:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.INTAKE_TEMP));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.VOLT));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.MAF));
        layout->addStretch();
        break;
    case 2:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.MAP));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.APP));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.TPAPWM));
        layout->addStretch();
        break;
    case 3:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.INJ));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.TVENT));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.TANK));
        layout->addStretch();
        break;
    }

    return widget;
}

QWidget *Obd2Tab::obd_data_widget(int colonna)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    switch (colonna)
    {
    case 1:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.IGNANGLE));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.BATT));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.RITCYL1));
        layout->addStretch();
        break;
    case 2:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.RITCYL2));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.RITCYL3));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.RITCYL4));
        layout->addStretch();
        break;
    case 3:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.SFT));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LMB1));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LMB2));
        layout->addStretch();
        break;
    }

    return widget;
}

QWidget *LSTab::ls_data_widget(int colonna)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    switch (colonna)
    {
    case 1:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSCOOLANT));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSOIL));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSLVLCOOLANT));
        layout->addStretch();
        break;
    case 2:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSTEMPEXT));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSBENZ));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSVOLT));
        layout->addStretch();
        break;
    case 3:
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSINIEZS));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSINIEZH));
        layout->addStretch();
        layout->addWidget(Session::Forge::br());
        layout->addStretch();
        layout->addWidget(this->vehicle_data_widget(gauges_cfg.LSINIEZKM));
        layout->addStretch();
        break;
    }

    return widget;
}

QWidget *DataTab::vehicle_data_widget(GaugeConfig cfg)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QFont value_font(this->arbiter.forge().font(cfg.font_size.value, true));

    QFont unit_font(this->arbiter.forge().font(cfg.font_size.unit));
    unit_font.setWeight(QFont::Light);
    unit_font.setItalic(true);

    Gauge *gauge = new Gauge(cfg,
                             value_font, unit_font, Gauge::RIGHT, widget);
    layout->addWidget(gauge);
    this->gauges.push_back(gauge);

    if (cfg.font_size.label > 0)
    {
        QFont label_font(this->arbiter.forge().font(cfg.font_size.label));
        label_font.setWeight(QFont::Light);

        QLabel *gauge_label = new QLabel(cfg.description, widget);
        gauge_label->setFont(label_font);
        gauge_label->setAlignment(Qt::AlignHCenter);
        layout->addWidget(gauge_label);
    }

    return widget;
}

QWidget *Obd1Tab::vehicle_data_widget(GaugeConfig cfg)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QFont value_font(this->arbiter.forge().font(cfg.font_size.value, true));

    QFont unit_font(this->arbiter.forge().font(cfg.font_size.unit));
    unit_font.setWeight(QFont::Light);
    unit_font.setItalic(true);

    Gauge *gauge = new Gauge(cfg,
                             value_font, unit_font, Gauge::RIGHT, widget);
    layout->addWidget(gauge);
    this->gauges.push_back(gauge);

    if (cfg.font_size.label > 0)
    {
        QFont label_font(this->arbiter.forge().font(cfg.font_size.label));
        label_font.setWeight(QFont::Light);

        QLabel *gauge_label = new QLabel(cfg.description, widget);
        gauge_label->setFont(label_font);
        gauge_label->setAlignment(Qt::AlignHCenter);
        layout->addWidget(gauge_label);
    }

    return widget;
}

QWidget *Obd2Tab::vehicle_data_widget(GaugeConfig cfg)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QFont value_font(this->arbiter.forge().font(cfg.font_size.value, true));

    QFont unit_font(this->arbiter.forge().font(cfg.font_size.unit));
    unit_font.setWeight(QFont::Light);
    unit_font.setItalic(true);

    Gauge *gauge = new Gauge(cfg,
                             value_font, unit_font, Gauge::RIGHT, widget);
    layout->addWidget(gauge);
    this->gauges.push_back(gauge);

    if (cfg.font_size.label > 0)
    {
        QFont label_font(this->arbiter.forge().font(cfg.font_size.label));
        label_font.setWeight(QFont::Light);

        QLabel *gauge_label = new QLabel(cfg.description, widget);
        gauge_label->setFont(label_font);
        gauge_label->setAlignment(Qt::AlignHCenter);
        layout->addWidget(gauge_label);
    }

    return widget;
}

QWidget *LSTab::vehicle_data_widget(GaugeConfig cfg)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QFont value_font(this->arbiter.forge().font(cfg.font_size.value, true));

    QFont unit_font(this->arbiter.forge().font(cfg.font_size.unit));
    unit_font.setWeight(QFont::Light);
    unit_font.setItalic(true);

    Gauge *gauge = new Gauge(cfg,
                             value_font, unit_font, Gauge::RIGHT, widget);
    layout->addWidget(gauge);
    this->gauges.push_back(gauge);

    if (cfg.font_size.label > 0)
    {
        QFont label_font(this->arbiter.forge().font(cfg.font_size.label));
        label_font.setWeight(QFont::Light);

        QLabel *gauge_label = new QLabel(cfg.description, widget);
        gauge_label->setFont(label_font);
        gauge_label->setAlignment(Qt::AlignHCenter);
        layout->addWidget(gauge_label);
    }

    return widget;
}