#include <QPalette>

#include "app/config.hpp"
#include "app/pages/vehicle.hpp"
#include "app/window.hpp"
#include "obd/conversions.hpp"

#include "plugins/vehicle_plugin.hpp"

Gauge::Gauge(units_t units, QFont value_font, QFont unit_font, Gauge::Orientation orientation, int rate,
             std::vector<Command> cmds, int precision, obd_decoder_t decoder, QWidget *parent)
    : QWidget(parent)
{
    Config *config = Config::get_instance();
    this->si = config->get_si_units();

    this->rate = rate;
    this->precision = precision;

    QBoxLayout *layout;
    if (orientation == BOTTOM)
        layout = new QVBoxLayout(this);
    else
        layout = new QHBoxLayout(this);

    QLabel *value_label = new QLabel(this->null_value(), this);
    value_label->setFont(value_font);
    value_label->setAlignment(Qt::AlignCenter);

    QLabel *unit_label = new QLabel(this->si ? units.second : units.first, this);
    unit_label->setFont(unit_font);
    unit_label->setAlignment(Qt::AlignCenter);

    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, [this, cmds, decoder, value_label]() {
        std::vector<double> results;
        for (auto cmd : cmds) {
            double result = 0;
            if (!OBD::get_instance()->query(cmd, result)) return;
            results.push_back(result);
        }

        value_label->setText(this->format_value(decoder(results, this->si)));
    });

    connect(config, &Config::si_units_changed, [this, units, unit_label, value_label](bool si) {
        this->si = si;
        unit_label->setText(this->si ? units.second : units.first);
        value_label->setText(this->null_value());
    });

    layout->addStretch();
    layout->addWidget(value_label);
    layout->addWidget(unit_label);
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

VehiclePage::VehiclePage(QWidget *parent) : QTabWidget(parent)
{
    this->tabBar()->setFont(Theme::font_16);
    this->addTab(new DataTab(this), "Data");
    this->config = Config::get_instance();

    // for (auto device : QCanBus::instance()->availableDevices("socketcan"))
    //     this->can_devices.append(device.name());

    // for (auto port : QSerialPortInfo::availablePorts())
    //     this->serial_devices.append(port.portName());

    this->get_plugins();
    this->active_plugin = new QPluginLoader(this);
    this->dialog = new Dialog(true, this->window());
    this->dialog->set_body(this->dialog_body());
    QPushButton *load_button = new QPushButton("load");
    connect(load_button, &QPushButton::clicked, [this]() { this->load_plugin(); });
    this->dialog->set_button(load_button);

    QPushButton *settings_button = new QPushButton(this);
    settings_button->setFlat(true);
    settings_button->setIconSize(Theme::icon_24);
    settings_button->setIcon(Theme::get_instance()->make_button_icon("settings", settings_button));
    connect(settings_button, &QPushButton::clicked, [this]() { this->dialog->open(); });
    this->setCornerWidget(settings_button);

    this->load_plugin();
}

QWidget *VehiclePage::dialog_body()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QStringList plugins = this->plugins.keys();
    this->plugin_selector = new Selector(plugins, this->config->get_vehicle_plugin(), Theme::font_14, widget, true);

    layout->addWidget(this->can_bus_toggle_row(), 1);
    layout->addWidget(this->interface_selector_row(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->plugin_selector, 1);

    return widget;
}

QWidget *VehiclePage::can_bus_toggle_row()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("CAN Bus", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->get_vehicle_can_bus());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [this](bool state) {
        this->config->set_vehicle_can_bus(state);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *VehiclePage::interface_selector_row()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Interface", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    QStringList devices = this->config->get_vehicle_can_bus() ? this->can_devices : this->serial_devices;
    Selector *selector = new Selector(devices, this->config->get_vehicle_interface(), Theme::font_14, widget);
    connect(selector, &Selector::item_changed, [config = this->config](QString item) {
        config->set_vehicle_interface(item);
    });
    connect(this->config, &Config::vehicle_can_bus_changed, [this, selector](bool state) {
        selector->set_options(state ? this->can_devices : this->serial_devices);
    });
    layout->addWidget(selector, 1);

    return widget;
}

void VehiclePage::get_plugins()
{
    for (const QFileInfo &plugin : Config::plugin_dir("vehicle").entryInfoList(QDir::Files)) {
        if (QLibrary::isLibrary(plugin.absoluteFilePath()))
            this->plugins[Config::fmt_plugin(plugin.baseName())] = plugin;
    }
}

void VehiclePage::load_plugin()
{
    if (this->active_plugin->isLoaded())
        this->active_plugin->unload();

    QString key = this->plugin_selector->get_current();
    if (!key.isNull()) {
        this->active_plugin->setFileName(this->plugins[key].absoluteFilePath());

        if (VehiclePlugin *plugin = qobject_cast<VehiclePlugin *>(this->active_plugin->instance())) {
            plugin->init();
            for (QWidget *tab : plugin->widgets())
                this->addTab(tab, tab->objectName());
        }
    }
    this->config->set_vehicle_plugin(key);
}

DataTab::DataTab(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    QWidget *driving_data = this->driving_data_widget();
    layout->addWidget(driving_data);
    layout->addWidget(Theme::br(this, true));

    QWidget *engine_data = this->engine_data_widget();
    layout->addWidget(engine_data);

    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch(5);
    driving_data->setSizePolicy(sp_left);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(2);
    engine_data->setSizePolicy(sp_right);
}

QWidget *DataTab::driving_data_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    layout->addStretch();
    layout->addWidget(this->speedo_tach_widget());
    layout->addStretch();
    layout->addWidget(Theme::br(widget));
    layout->addWidget(this->mileage_data_widget());

    return widget;
}

QWidget *DataTab::speedo_tach_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    Gauge *speed = new Gauge({"mph", "km/h"}, QFont("Titillium Web", 72), QFont("Montserrat", 16, QFont::Light, true),
                             Gauge::BOTTOM, 100, {cmds.SPEED}, 0,
                             [](std::vector<double> x, bool si) { return si ? x[0] : kph_to_mph(x[0]); }, widget);
    layout->addWidget(speed);
    this->gauges.push_back(speed);

    Gauge *rpm = new Gauge({"x1000rpm", "x1000rpm"}, QFont("Titillium Web", 72),
                           QFont("Montserrat", 16, QFont::Light, true), Gauge::BOTTOM, 100, {cmds.RPM}, 1,
                           [](std::vector<double> x, bool _) { return x[0] / 1000.0; }, widget);
    layout->addWidget(rpm);
    this->gauges.push_back(rpm);

    return widget;
}

QWidget *DataTab::mileage_data_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    Gauge *mileage = new Gauge({"mpg", "km/L"}, QFont("Titillium Web", 36), QFont("Montserrat", 14, QFont::Light, true),
                               Gauge::BOTTOM, 100, {cmds.SPEED, cmds.MAF}, 1,
                               [](std::vector<double> x, bool si) {
                                   return (si ? x[0] : kph_to_mph(x[0])) / (si ? gps_to_lph(x[1]) : gps_to_gph(x[1]));
                               },
                               widget);
    layout->addWidget(mileage);
    this->gauges.push_back(mileage);

    return widget;
}

QWidget *DataTab::engine_data_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    layout->addStretch();
    layout->addWidget(this->coolant_temp_widget());
    layout->addStretch();
    layout->addWidget(Theme::br(widget));
    layout->addStretch();
    layout->addWidget(this->engine_load_widget());
    layout->addStretch();

    return widget;
}

QWidget *DataTab::coolant_temp_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    Gauge *coolant_temp = new Gauge(
        {"°F", "°C"}, QFont("Titillium Web", 36), QFont("Montserrat", 14, QFont::Light, true), Gauge::RIGHT, 5000,
        {cmds.COOLANT_TEMP}, 1, [](std::vector<double> x, bool si) { return si ? x[0] : c_to_f(x[0]); }, widget);
    layout->addWidget(coolant_temp);
    this->gauges.push_back(coolant_temp);

    QLabel *coolant_temp_label = new QLabel("coolant", widget);
    coolant_temp_label->setFont(QFont("Montserrat", 14, QFont::Light));
    coolant_temp_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(coolant_temp_label);

    return widget;
}

QWidget *DataTab::engine_load_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    Gauge *engine_load =
        new Gauge({"%", "%"}, QFont("Titillium Web", 36), QFont("Montserrat", 14, QFont::Light, true), Gauge::RIGHT,
                  500, {cmds.LOAD}, 1, [](std::vector<double> x, bool _) { return x[0]; }, widget);
    layout->addWidget(engine_load);
    this->gauges.push_back(engine_load);

    QLabel *engine_load_label = new QLabel("load", widget);
    engine_load_label->setFont(QFont("Montserrat", 14, QFont::Light));
    engine_load_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(engine_load_label);
    return widget;
}
