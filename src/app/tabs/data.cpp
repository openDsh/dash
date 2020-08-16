#include <QPalette>

#include "app/config.hpp"
#include "app/tabs/data.hpp"
#include "app/tabs/climate.hpp"
#include "app/window.hpp"
#include "obd/conversions.hpp"

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

QWidget *VehicleTab::plugins()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    const QStringList views = {"foo", "bar", "baz"};

    QLabel *label = new QLabel("foo", widget);
    label->setAlignment(Qt::AlignCenter);
    label->setFont(Theme::font_16);

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    left_button->setIcon(Theme::get_instance()->make_button_icon("arrow_left", left_button));
    connect(left_button, &QPushButton::clicked, [this, label, views]() {
        int total_views = views.size();
        QString view = views[((views.indexOf(label->text()) - 1) % total_views + total_views) % total_views];
        label->setText(view);
    });

    QPushButton *right_button = new QPushButton(widget);
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    right_button->setIcon(Theme::get_instance()->make_button_icon("arrow_right", right_button));
    connect(right_button, &QPushButton::clicked, [this, label, views]() {
        QString view = views[(views.indexOf(label->text()) + 1) % views.size()];
        label->setText(view);
    });

    layout->addStretch(1);
    layout->addWidget(left_button);
    layout->addWidget(label, 2);
    layout->addWidget(right_button);
    layout->addStretch(1);

    return widget;
}

VehicleTab::VehicleTab(QWidget *parent) : QTabWidget(parent)
{
    QPushButton *settings_button = new QPushButton(this);
    settings_button->setFlat(true);
    settings_button->setIconSize(Theme::icon_24);
    settings_button->setIcon(Theme::get_instance()->make_button_icon("settings", settings_button));
    connect(settings_button, &QPushButton::clicked, [this]() {
        Dialog *dialog = new Dialog(true, this->window());
        dialog->set_body(this->plugins());
        QPushButton *save_button = new QPushButton("load");
        dialog->set_button(save_button);
        dialog->open();
    });

    this->tabBar()->setFont(Theme::font_18);
    this->setCornerWidget(settings_button);

    this->addTab(new DataTab(this), "Data");
    Climate *climate = new Climate(this);
    climate->set_max_speed(6);
    climate->set_driver_temp(66);
    climate->set_passenger_temp(55);
    int idx = this->addTab(climate, "Climate");
    this->capabilities["climate"] = idx;

    this->setTabEnabled(idx, false);

    // QTimer *timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, [climate]() {
    //     // popup should be triggered anytime climate class receives new data!!!
    //     climate->set_speed(rand() % 6);
    //     QWidget *parent = nullptr;
    //     for (QWidget *widget : qApp->allWidgets()) {
    //         if (widget->objectName() == "controls_bar") {
    //             parent = widget;
    //             break;
    //         }
    //     }

    //     Dialog *dialog = new Dialog(false, parent);
    //     dialog->set_body(popup(*climate));
    //     dialog->setFocusPolicy(Qt::NoFocus);
    //     dialog->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    //     dialog->open(1000);
    // });


    // timer->start(5000);
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
