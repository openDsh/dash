#include <QPalette>
#include <QPainter>
#include <QPen>
#include <QSerialPortInfo>

//For speed Correction - Eric KG6WXC
#include <QByteArray>
#include <QtGlobal>

#include <algorithm>
#include <cmath>

#include "app/config.hpp"
#include "app/pages/vehicle.hpp"
#include "app/window.hpp"
#include "obd/conversions.hpp"
#include "canbus/elm327.hpp"
#include "plugins/vehicle_plugin.hpp"

namespace {

//For speed Correction - Eric KG6WXC
double configured_speed_correction()
{
    const QByteArray raw_value = qgetenv("DASH_SPEED_CORRECTION");

    if (raw_value.isEmpty())
        return 1.0;

    bool valid = false;
    const double correction = raw_value.toDouble(&valid);

    // Invalid, zero, or negative values fall back to no correction.
    if (!valid || correction <= 0.0)
        return 1.0;

    return correction;
}

class RadialGaugeLabel : public QLabel
{
public:
    explicit RadialGaugeLabel(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        // The previous RPM gauge requested a 190x190 size.  A fixed 200x200
        // canvas makes every gauge identical and is approximately 5% larger.
        constexpr int gauge_size = 200;

        this->setAlignment(Qt::AlignCenter);
        this->setAutoFillBackground(false);
        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->setFixedSize(gauge_size, gauge_size);
    }

    QSize sizeHint() const override
    {
        return QSize(200, 200);
    }

    QSize minimumSizeHint() const override
    {
        return QSize(200, 200);
    }

    void set_gauge_fonts(const QFont &value_font, const QFont &unit_font)
    {
        this->value_font = value_font;
        this->unit_font = unit_font;
        this->update();
    }

    void set_gauge_unit(const QString &unit)
    {
        this->unit = unit;
        this->update();
    }

    void set_gauge_value(const QString &text, double value, bool valid)
    {
        this->value_text = text;
        this->value = value;
        this->valid = valid;
        this->update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const qreal side = std::max<qreal>(20.0, std::min(this->width(), this->height()) - 8.0);
        const QPointF center(this->width() / 2.0, this->height() / 2.0);
        const qreal ring_width = std::max<qreal>(5.0, side * 0.045);
        const QRectF ring_rect(center.x() - side / 2.0 + ring_width,
                               center.y() - side / 2.0 + ring_width,
                               side - ring_width * 2.0,
                               side - ring_width * 2.0);
        const QRectF face_rect(center.x() - side / 2.0 + ring_width / 2.0,
                               center.y() - side / 2.0 + ring_width / 2.0,
                               side - ring_width,
                               side - ring_width);

        const QColor face(16, 16, 16);
        const QColor outer_ring(95, 95, 95);
        const QColor green(0, 210, 32);
        const QColor red(255, 35, 35);

        painter.setPen(Qt::NoPen);
        painter.setBrush(face);
        painter.drawEllipse(face_rect);

        constexpr qreal start_angle = 225.0;
        constexpr qreal span_angle = -270.0;

        QPen outer_pen(outer_ring, ring_width, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(outer_pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawArc(ring_rect, static_cast<int>(start_angle * 16.0), static_cast<int>(span_angle * 16.0));

        // Draw the warning zone at the high end of the gauge.
        QPen warning_pen(red, ring_width * 0.82, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(warning_pen);
        painter.drawArc(ring_rect,
                        static_cast<int>((start_angle + span_angle * 0.84) * 16.0),
                        static_cast<int>((span_angle * 0.16) * 16.0));

        const double max_value = this->max_for_unit(this->unit);
        double fraction = this->valid ? (this->value / max_value) : 0.0;
        fraction = std::max(0.0, std::min(1.0, fraction));
        constexpr int segments = 28;
        const int lit_segments = static_cast<int>(std::round(fraction * segments));
        const qreal segment_span = span_angle / segments;

        QPen progress_pen(green, ring_width * 0.82, Qt::SolidLine, Qt::FlatCap);
        painter.setPen(progress_pen);
        for (int i = 0; i < lit_segments; ++i) {
            const qreal segment_start = start_angle + segment_span * i;
            painter.drawArc(ring_rect,
                            static_cast<int>(segment_start * 16.0),
                            static_cast<int>((segment_span * 0.62) * 16.0));
        }

        QFont display_value_font = this->value_font;
        if (display_value_font.family().isEmpty())
            display_value_font = this->font();
        display_value_font.setBold(true);

        QFont display_unit_font = this->unit_font;
        if (display_unit_font.family().isEmpty())
            display_unit_font = this->font();
        display_unit_font.setBold(true);

        painter.setPen(green);
        painter.setFont(display_value_font);
        QRectF value_rect(center.x() - side * 0.38,
                          center.y() - side * 0.20,
                          side * 0.76,
                          side * 0.30);
        painter.drawText(value_rect, Qt::AlignCenter, this->value_text);

        painter.setFont(display_unit_font);
        QRectF unit_rect(center.x() - side * 0.38,
                         center.y() + side * 0.08,
                         side * 0.76,
                         side * 0.20);
        painter.drawText(unit_rect, Qt::AlignCenter, this->unit);
    }

private:
    double max_for_unit(const QString &unit) const
    {
        if (unit == QStringLiteral("km/h"))
            return 220.0;
        if (unit == QStringLiteral("mph"))
            return 140.0;
        if (unit.contains(QStringLiteral("rpm"), Qt::CaseInsensitive))
            return 8.0;
        if (unit == QStringLiteral("%"))
            return 100.0;
        if (unit == QStringLiteral("°C"))
            return 120.0;
        if (unit == QStringLiteral("°F"))
            return 250.0;
        if (unit == QStringLiteral("g/s"))
            return 200.0;
        return 100.0;
    }

    bool valid = false;
    double value = 0.0;
    QString value_text = QStringLiteral("--");
    QString unit;
    QFont value_font;
    QFont unit_font;
};

} // namespace

Gauge::Gauge(units_t units, QFont value_font, QFont unit_font, Gauge::Orientation orientation, int rate,
             std::vector<Command> cmds, int precision, obd_decoder_t decoder, QWidget *parent)
: QWidget(parent)
{
    Config *config = Config::get_instance();
    ICANBus *bus;
    switch(config->get_vehicle_can_bus()){
        //ELM327 USB
        case ICANBus::VehicleBusType::ELM327USB:
            bus = (ICANBus *)elm327::get_usb_instance();
            break;
        //ELM327 Bluetooth
        case ICANBus::VehicleBusType::ELM327BT:
            bus = (ICANBus *)elm327::get_bt_instance();
            break;
        //SocketCAN
        case ICANBus::VehicleBusType::SocketCAN:
        default:
            bus = (ICANBus *)SocketCANBus::get_instance();
            break;
    }

    using namespace std::placeholders;
    std::function<void(QByteArray)> callback = std::bind(&Gauge::can_callback, this, std::placeholders::_1);

    bus->registerFrameHandler(cmds[0].frame.frameId()+0x9, callback);
    DASH_LOG(info)<<"[Gauges] Registered frame handler for id "<<(cmds[0].frame.frameId()+0x9);

    this->si = config->get_si_units();

    this->rate = rate;
    this->precision = precision;

    this->cmds = cmds;
    this->decoder = decoder;

    QBoxLayout *layout;
    if (orientation == BOTTOM)
        layout = new QVBoxLayout(this);
    else
        layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    value_label = new RadialGaugeLabel(this);
    auto radial_gauge = static_cast<RadialGaugeLabel *>(value_label);
    radial_gauge->set_gauge_fonts(value_font, unit_font);
    radial_gauge->set_gauge_unit(this->si ? units.second : units.first);
    radial_gauge->set_gauge_value(this->null_value(), 0.0, false);

    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, [this, bus, cmds]() {
        for (auto cmd : cmds) {
            bus->writeFrame(cmd.frame);
        }
    });

    connect(config, &Config::si_units_changed, [this, units](bool si) {
        this->si = si;
        auto radial_gauge = static_cast<RadialGaugeLabel *>(value_label);
        radial_gauge->set_gauge_unit(this->si ? units.second : units.first);
        radial_gauge->set_gauge_value(this->null_value(), 0.0, false);
    });

    layout->addStretch(1);
    layout->addWidget(value_label, 0, Qt::AlignCenter);
    layout->addStretch(1);
}

void Gauge::can_callback(QByteArray payload){
    Response resp = Response(payload);
    for(auto cmd : cmds){
        if(cmd.frame.payload().at(2) == resp.PID){
            double value = this->decoder(cmd.decoder(resp), this->si);
            if (!std::isfinite(value)) {
                const QByteArray payload_hex = payload.toHex();
                DASH_LOG(warning)
                << "[Gauges] Invalid response for PID "
                << static_cast<int>(resp.PID)
                << ", payload "
                << payload_hex.constData();
                return;
            }
            static_cast<RadialGaugeLabel *>(value_label)->set_gauge_value(this->format_value(value), value, true);
        }
    }
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
    : QTabWidget(parent)
    , Page(arbiter, "Vehicle", "directions_car", true, this)
{
}

void VehiclePage::init()
{
    this->addTab(new DataTab(this->arbiter, this), "Data");
    this->config = Config::get_instance();

    for (auto device : QCanBus::instance()->availableDevices("socketcan"))
        this->can_devices.append(device.name());

    for (auto port : QSerialPortInfo::availablePorts())
        this->serial_devices.append(port.systemLocation());

    connect(&this->arbiter.system().bluetooth, &Bluetooth::init, [this]{
        for (auto device: this->arbiter.system().bluetooth.get_devices())
        {
            if(device->isPaired()){
                this->paired_bt_devices.insert(device->name(), device->address());
            }
        }
    });

    this->get_plugins();
    this->active_plugin = new QPluginLoader(this);
    Dialog *dialog = new Dialog(this->arbiter, true, this->window());
    dialog->set_body(this->dialog_body());
    QPushButton *load_button = new QPushButton("load");
    connect(load_button, &QPushButton::clicked, [this]() { this->load_plugin(); });
    dialog->set_button(load_button);

    QPushButton *settings_button = new QPushButton(this);
    settings_button->setFlat(true);
    this->arbiter.forge().iconize("settings", settings_button, 24);
    connect(settings_button, &QPushButton::clicked, [dialog]() { dialog->open(); });
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

    QStringList devices;
    switch(config->get_vehicle_can_bus()){
        //ELM327 USB
        case ICANBus::VehicleBusType::ELM327USB:
            devices = this->serial_devices;
            break;
        //ELM327 Bluetooth
        case ICANBus::VehicleBusType::ELM327BT:
            
            break;
        //SocketCAN
        case ICANBus::VehicleBusType::SocketCAN:
        default:
            devices = this->can_devices;
            break;
    }
   
    Selector *interface_selector = new Selector(devices, this->config->get_vehicle_interface(), this->arbiter.forge().font(14), this->arbiter, widget, "disabled");
    interface_selector->setVisible((this->can_devices.size() > 0) || (this->serial_devices.size() > 0) || (this->paired_bt_devices.size() > 0));
    connect(interface_selector, &Selector::item_changed, [this](QString item){
        if(this->config->get_vehicle_can_bus()==ICANBus::VehicleBusType::ELM327BT && item != QString("disabled"))
        {
            this->config->set_vehicle_interface(this->paired_bt_devices[item]);
        }
        else
        {
            this->config->set_vehicle_interface(item);
        }
    });
    connect(this->config, &Config::vehicle_can_bus_changed, [this, interface_selector](int state){
        switch(state){
            //ELM327 USB
            case ICANBus::VehicleBusType::ELM327USB:
                interface_selector->set_options(this->serial_devices);
                break;
            //ELM327 Bluetooth
            case ICANBus::VehicleBusType::ELM327BT:
                interface_selector->set_options(this->paired_bt_devices.keys());
                break;
            //SocketCAN
            case ICANBus::VehicleBusType::SocketCAN:
            default:
                interface_selector->set_options(this->can_devices);
                break;
        }
    });
    connect(&this->arbiter.system().bluetooth, &Bluetooth::init, [this, interface_selector]{
        interface_selector->setVisible((this->can_devices.size() > 0) || (this->serial_devices.size() > 0) || (this->paired_bt_devices.size() > 0));
        if(this->config->get_vehicle_can_bus()==ICANBus::VehicleBusType::ELM327BT){
            QString current = this->config->get_vehicle_interface();
            interface_selector->set_options(this->paired_bt_devices.keys());
            if(current != "disabled")
                interface_selector->set_current(this->paired_bt_devices.key(current));

        }
    });
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

    ICANBus::VehicleBusType can_bus_selected = this->config->get_vehicle_can_bus();
    QRadioButton *socketcan_button = new QRadioButton("SocketCAN", group);
    socketcan_button->setChecked(can_bus_selected==ICANBus::VehicleBusType::SocketCAN);
    socketcan_button->setEnabled(this->can_devices.size() > 0);
    connect(socketcan_button, &QRadioButton::clicked, [config = this->config]{
        config->set_vehicle_can_bus(ICANBus::VehicleBusType::SocketCAN);
    });
    group_layout->addWidget(socketcan_button);

    QRadioButton *elm_usb_button = new QRadioButton("ELM327 (USB)", group);
    elm_usb_button->setChecked(can_bus_selected==ICANBus::VehicleBusType::ELM327USB);
    elm_usb_button->setEnabled(this->serial_devices.size() > 0);
    connect(elm_usb_button, &QRadioButton::clicked, [config = this->config]{
        config->set_vehicle_can_bus(ICANBus::VehicleBusType::ELM327USB);
    });
    group_layout->addWidget(elm_usb_button);

    QRadioButton *elm_bt_button = new QRadioButton("ELM327 (Bluetooth)", group);
    elm_bt_button->setChecked(can_bus_selected==ICANBus::VehicleBusType::ELM327BT);
    elm_bt_button->setEnabled(false);
    connect(elm_bt_button, &QRadioButton::clicked, [config = this->config]{
        config->set_vehicle_can_bus(ICANBus::VehicleBusType::ELM327BT);
    });
    connect(&this->arbiter.system().bluetooth, &Bluetooth::init, [this, elm_bt_button]{
            elm_bt_button->setEnabled(this->paired_bt_devices.size() > 0);
    });
    group_layout->addWidget(elm_bt_button);

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
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) { config->set_si_units(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

void VehiclePage::get_plugins()
{
    for (const QFileInfo &plugin : Session::plugin_dir("vehicle").entryInfoList(QDir::Files)) {
        if (QLibrary::isLibrary(plugin.absoluteFilePath()))
            this->plugins[Session::fmt_plugin(plugin.baseName())] = plugin;
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
            plugin->dashize(&this->arbiter);
            switch(config->get_vehicle_can_bus()){
                //ELM327 USB
                case ICANBus::VehicleBusType::ELM327USB:
                    plugin->init((ICANBus *)elm327::get_usb_instance());
                    break;
                //ELM327 Bluetooth
                case ICANBus::VehicleBusType::ELM327BT:
                    plugin->init((ICANBus *)elm327::get_bt_instance());
                    break;
                //SocketCAN
                case ICANBus::VehicleBusType::SocketCAN:
                default:
                    plugin->init((ICANBus *)SocketCANBus::get_instance());
                    break;
            }
            for (QWidget *tab : plugin->widgets())
                this->addTab(tab, tab->objectName());
        }
    }
    this->config->set_vehicle_plugin(key);
}

DataTab::DataTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
    // Use one grid for the entire Vehicle page so every gauge shares the
    // same row and column geometry.  This also removes the old vertical
    // divider between the driving and engine-data sections.
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(16, 8, 16, 8);
    layout->setHorizontalSpacing(20);
    layout->setVerticalSpacing(12);

    layout->addWidget(this->speed_widget(),        0, 0, Qt::AlignCenter);
    layout->addWidget(this->coolant_temp_widget(), 0, 1, Qt::AlignCenter);
    layout->addWidget(this->intake_temp_widget(),  0, 2, Qt::AlignCenter);
    layout->addWidget(this->rpm_widget(),          1, 0, Qt::AlignCenter);
    layout->addWidget(this->engine_load_widget(),  1, 1, Qt::AlignCenter);
    layout->addWidget(this->maf_widget(),          1, 2, Qt::AlignCenter);

    for (int column = 0; column < 3; ++column)
        layout->setColumnStretch(column, 1);
    for (int row = 0; row < 2; ++row)
        layout->setRowStretch(row, 1);

    for (auto &gauge : this->gauges)
        gauge->start();
}

QWidget *DataTab::speed_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QFont value_font(this->arbiter.forge().font(38, true));
    value_font.setBold(true);

    QFont unit_font(this->arbiter.forge().font(16));
    unit_font.setBold(true);
    unit_font.setItalic(false);

    //For speed Correction - Eric KG6WXC
    const double speed_correction = configured_speed_correction();

    //Change below decoder For speed Correction - Eric KG6WXC
    Gauge *speed = new Gauge({"mph", "km/h"}, value_font, unit_font,
                             Gauge::BOTTOM, 100, {cmds.SPEED}, 0,
                             [speed_correction](double x, bool si) {
                                 const double corrected_kph = x * speed_correction;
                                 return si ? corrected_kph : kph_to_mph(corrected_kph);
                             },
                             widget);

    layout->addWidget(speed, 0, Qt::AlignHCenter);
    this->gauges.push_back(speed);

    QFont label_font(this->arbiter.forge().font(14));
    label_font.setBold(true);

    QLabel *speed_label = new QLabel("Speed", widget);
    speed_label->setFont(label_font);
    speed_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(speed_label);

    return widget;
}

QWidget *DataTab::rpm_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QFont value_font(this->arbiter.forge().font(38, true));
    value_font.setBold(true);

    QFont unit_font(this->arbiter.forge().font(16));
    unit_font.setBold(true);
    unit_font.setItalic(false);

    Gauge *rpm = new Gauge({"x1000rpm", "x1000rpm"}, value_font, unit_font,
                           Gauge::BOTTOM, 100, {cmds.RPM}, 1,
                           [](double x, bool _) { return x / 1000.0; }, widget);
    layout->addWidget(rpm, 0, Qt::AlignHCenter);
    this->gauges.push_back(rpm);

    QFont label_font(this->arbiter.forge().font(14));
    label_font.setBold(true);

    QLabel *rpm_label = new QLabel("RPM", widget);
    rpm_label->setFont(label_font);
    rpm_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(rpm_label);

    return widget;
}

/*  socketcan/elm327 rewrite right now only has support for one PID per gauge, so we can't calculate milage at this point.
    This is because gauges act more as event handlers now for each PID.
    Multi-PID gauges could feasibly be reimplemented if there was a helper method that stored received values, and only calls
    the gauge update once all values have been updated since last gauge update.

*/

// QWidget *DataTab::mileage_data_widget()
// {
//     QWidget *widget = new QWidget(this);
//     QHBoxLayout *layout = new QHBoxLayout(widget);
//
//     QFont value_font(Theme::font_36);
//     value_font.setFamily("Titillium Web");
//
//     QFont unit_font(Theme::font_14);
//     unit_font.setWeight(QFont::Light);
//     unit_font.setItalic(true);
//
//     Gauge *mileage = new Gauge({"mpg", "km/L"}, value_font, unit_font,
//                                Gauge::BOTTOM, 100, {cmds.SPEED, cmds.MAF}, 1,
//                                [](std::vector<double> x, bool si) {
//                                    return (si ? x[0] : kph_to_mph(x[0])) / (si ? gps_to_lph(x[1]) : gps_to_gph(x[1]));
//                                },
//                                widget);
//     layout->addWidget(mileage);
//     this->gauges.push_back(mileage);
//
//     return widget;
// }

QWidget *DataTab::coolant_temp_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QFont value_font(this->arbiter.forge().font(38, true));
    value_font.setBold(true);

    QFont unit_font(this->arbiter.forge().font(16));
    unit_font.setBold(true);
    unit_font.setItalic(false);

    Gauge *coolant_temp = new Gauge(
        {"°F", "°C"}, value_font, unit_font, Gauge::BOTTOM, 5000,
        {cmds.COOLANT_TEMP}, 1, [](double x, bool si) { return si ? x : c_to_f(x); }, widget);
    layout->addWidget(coolant_temp, 0, Qt::AlignHCenter);
    this->gauges.push_back(coolant_temp);

    QFont label_font(this->arbiter.forge().font(14));
    label_font.setBold(true);

    QLabel *coolant_temp_label = new QLabel("Coolant", widget);
    coolant_temp_label->setFont(label_font);
    coolant_temp_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(coolant_temp_label);

    return widget;
}

QWidget *DataTab::engine_load_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QFont value_font(this->arbiter.forge().font(38, true));
    value_font.setBold(true);

    QFont unit_font(this->arbiter.forge().font(16));
    unit_font.setBold(true);
    unit_font.setItalic(false);

    Gauge *engine_load =
        new Gauge({"%", "%"}, value_font, unit_font, Gauge::BOTTOM,
                  500, {cmds.LOAD}, 1, [](double x, bool _) { return x; }, widget);
    layout->addWidget(engine_load, 0, Qt::AlignHCenter);
    this->gauges.push_back(engine_load);

    QFont label_font(this->arbiter.forge().font(14));
    label_font.setBold(true);

    QLabel *engine_load_label = new QLabel("Load", widget);
    engine_load_label->setFont(label_font);
    engine_load_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(engine_load_label);

    return widget;
}

QWidget *DataTab::intake_temp_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QFont value_font(this->arbiter.forge().font(38, true));
    value_font.setBold(true);

    QFont unit_font(this->arbiter.forge().font(16));
    unit_font.setBold(true);
    unit_font.setItalic(false);

    Gauge *intake_temp = new Gauge(
        {"°F", "°C"}, value_font, unit_font, Gauge::BOTTOM, 5000,
        {cmds.INTAKE_TEMP}, 1, [](double x, bool si) { return si ? x : c_to_f(x); }, widget);
    layout->addWidget(intake_temp, 0, Qt::AlignHCenter);
    this->gauges.push_back(intake_temp);

    QFont label_font(this->arbiter.forge().font(14));
    label_font.setBold(true);

    QLabel *intake_temp_label = new QLabel("Intake Air", widget);
    intake_temp_label->setFont(label_font);
    intake_temp_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(intake_temp_label);

    return widget;
}

QWidget *DataTab::maf_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QFont value_font(this->arbiter.forge().font(38, true));
    value_font.setBold(true);

    QFont unit_font(this->arbiter.forge().font(16));
    unit_font.setBold(true);
    unit_font.setItalic(false);

    // OBD-II PID 0x10 is conventionally reported in grams per second,
    // including on many non-SI scan tools, so both unit modes use g/s.
    Gauge *maf = new Gauge(
        {"g/s", "g/s"}, value_font, unit_font, Gauge::BOTTOM, 500,
        {cmds.MAF}, 1, [](double x, bool _) { return x; }, widget);
    layout->addWidget(maf, 0, Qt::AlignHCenter);
    this->gauges.push_back(maf);

    QFont label_font(this->arbiter.forge().font(14));
    label_font.setBold(true);

    QLabel *maf_label = new QLabel("MAF", widget);
    maf_label->setFont(label_font);
    maf_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(maf_label);

    return widget;
}
