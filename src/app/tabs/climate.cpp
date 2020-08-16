#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QString>

#include "app/theme.hpp"
#include "app/tabs/climate.hpp"

QWidget *popup(Climate &climate)
{
    QWidget *widget = new QWidget(climate.parentWidget());
    QHBoxLayout *layout = new QHBoxLayout(widget);

    if (climate.driver_temp > 0) {
        QLabel *driver_temp = new QLabel(QString::number(climate.driver_temp), widget);
        driver_temp->setFont(Theme::font_14);
        driver_temp->setAlignment(Qt::AlignCenter);
        layout->addWidget(driver_temp);
    }

    layout->addStretch();

    QPushButton *symbol = new QPushButton(widget);
    QSizePolicy symbol_policy = symbol->sizePolicy();
    symbol_policy.setRetainSizeWhenHidden(true);
    symbol->setSizePolicy(symbol_policy);
    symbol->setFocusPolicy(Qt::NoFocus);
    symbol->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    symbol->setFlat(true);
    symbol->setIconSize(Theme::icon_24);
    symbol->setIcon(Theme::get_instance()->make_button_icon("play", symbol));
    layout->addWidget(symbol);

    QProgressBar *speed = new QProgressBar(widget);
    speed->setMaximum(climate.max_speed);
    speed->setFormat("%v");
    speed->setFont(Theme::font_14);
    speed->setValue(climate.speed);
    layout->addWidget(speed);

    layout->addStretch();

    if (climate.passenger_temp > 0) {
        QLabel *passenger_temp = new QLabel(QString::number(climate.passenger_temp), widget);
        passenger_temp->setFont(Theme::font_14);
        passenger_temp->setAlignment(Qt::AlignCenter);
        layout->addWidget(passenger_temp);
    }

    return widget;
}
