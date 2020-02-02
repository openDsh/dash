#include <QPalette>

#include <app/config.hpp>
#include <app/tabs/data.hpp>
#include <app/window.hpp>
#include <obd/conversions.hpp>

Gauge::Gauge(units_t units, QFont value_font, QFont unit_font, Gauge::Orientation orientation, int rate,
             std::vector<Command> cmds, int precision, obd_decoder_t decoder, QWidget *parent)
    : QWidget(parent)
{
    this->si = Config::get_instance()->get_si_units();

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

    connect(this, &Gauge::toggle_unit, [this, units, unit_label, value_label](bool si) {
        this->si = si;
        unit_label->setText(this->si ? units.second : units.first);
        value_label->setText(this->null_value());
    });

    layout->addStretch(1);
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

DataTab::DataTab(QWidget *parent) : QWidget(parent)
{
    MainWindow *app = qobject_cast<MainWindow *>(parent);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QFrame *status_indicator = new QFrame(this);
    status_indicator->setFixedHeight(6 * RESOLUTION);
    status_indicator->setAutoFillBackground(true);
    connect(app, &MainWindow::data_tab_toggle, [this, status_indicator](bool toggled) {
        if (toggled) {
            QPalette p(palette());
            if (OBD::get_instance()->is_connected()) {
                p.setColor(QPalette::Window, Theme::success_color);
                for (auto &gauge : this->gauges) gauge->start();
            }
            else
                p.setColor(QPalette::Window, Theme::danger_color);
            status_indicator->setPalette(p);
        }
        else
            for (auto &gauge : this->gauges) gauge->stop();
    });
    layout->addWidget(status_indicator);
    layout->addWidget(this->cluster_widget());
}

QWidget *DataTab::cluster_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(24, 24, 24, 24);

    QWidget *driving_data = this->driving_data_widget();
    layout->addWidget(driving_data);

    QFrame *vert_break = new QFrame(widget);
    vert_break->setLineWidth(1);
    vert_break->setFrameShape(QFrame::VLine);
    vert_break->setFrameShadow(QFrame::Plain);
    layout->addWidget(vert_break);

    QWidget *engine_data = this->engine_data_widget();
    layout->addWidget(engine_data);

    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch(5);
    driving_data->setSizePolicy(sp_left);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(2);
    engine_data->setSizePolicy(sp_right);

    return widget;
}

QWidget *DataTab::driving_data_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    layout->addWidget(this->speedo_tach_widget());

    QFrame *horiz_break = new QFrame(widget);
    horiz_break->setLineWidth(1);
    horiz_break->setFrameShape(QFrame::HLine);
    horiz_break->setFrameShadow(QFrame::Plain);
    layout->addWidget(horiz_break);

    layout->addWidget(this->mileage_data_widget());

    return widget;
}

QWidget *DataTab::speedo_tach_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    Gauge *speed = new Gauge({"mph", "km/h"}, QFont("Titillium Web", 92), QFont("Montserrat", 24, QFont::Light, true),
                             Gauge::BOTTOM, 100, {cmds.SPEED}, 0,
                             [](std::vector<double> x, bool si) { return si ? x[0] : kph_to_mph(x[0]); }, widget);
    layout->addWidget(speed);
    this->gauges.push_back(speed);

    Gauge *rpm = new Gauge({"x1000rpm", "x1000rpm"}, QFont("Titillium Web", 92),
                           QFont("Montserrat", 24, QFont::Light, true), Gauge::BOTTOM, 100, {cmds.RPM}, 1,
                           [](std::vector<double> x, bool _) { return x[0] / 1000.0; }, widget);
    layout->addWidget(rpm);
    this->gauges.push_back(rpm);

    return widget;
}

QWidget *DataTab::mileage_data_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    Gauge *mileage = new Gauge({"mpg", "km/L"}, QFont("Titillium Web", 48), QFont("Montserrat", 16, QFont::Light, true),
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

    layout->addStretch(1);
    layout->addWidget(this->coolant_temp_widget());
    layout->addStretch(1);

    QFrame *horiz_break = new QFrame(widget);
    horiz_break->setLineWidth(1);
    horiz_break->setFrameShape(QFrame::HLine);
    horiz_break->setFrameShadow(QFrame::Plain);
    layout->addWidget(horiz_break);

    layout->addStretch(1);
    layout->addWidget(this->engine_load_widget());
    layout->addStretch(1);

    return widget;
}

QWidget *DataTab::coolant_temp_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    Gauge *coolant_temp = new Gauge(
        {"°F", "°C"}, QFont("Titillium Web", 48), QFont("Montserrat", 16, QFont::Light, true), Gauge::RIGHT, 5000,
        {cmds.COOLANT_TEMP}, 1, [](std::vector<double> x, bool si) { return si ? x[0] : c_to_f(x[0]); }, widget);
    layout->addWidget(coolant_temp);
    this->gauges.push_back(coolant_temp);

    QLabel *coolant_temp_label = new QLabel("coolant", widget);
    coolant_temp_label->setFont(QFont("Montserrat", 16, QFont::Light));
    coolant_temp_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(coolant_temp_label);

    return widget;
}

QWidget *DataTab::engine_load_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    Gauge *engine_load =
        new Gauge({"%", "%"}, QFont("Titillium Web", 48), QFont("Montserrat", 16, QFont::Light, true), Gauge::RIGHT,
                  500, {cmds.LOAD}, 1, [](std::vector<double> x, bool _) { return x[0]; }, widget);
    layout->addWidget(engine_load);
    this->gauges.push_back(engine_load);

    QLabel *engine_load_label = new QLabel("load", widget);
    engine_load_label->setFont(QFont("Montserrat", 16, QFont::Light));
    engine_load_label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(engine_load_label);
    return widget;
}

void DataTab::convert_gauges(bool si)
{
    for (auto &x : this->gauges) x->toggle_unit(si);
}
