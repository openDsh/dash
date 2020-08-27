#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QString>

#include "app/theme.hpp"
#include "app/tabs/climate.hpp"
#include "app/widgets/step_meter.hpp"

// https://dribbble.com/shots/5991865-Car-Control-App
// https://torrancehu.com/smart-ac-mobile-application/
QWidget *popup(Climate &climate)
{
    QWidget *widget = new QWidget(climate.parentWidget());
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    if (climate.driver_temp > 0) {
        QLabel *temp = new QLabel(QString::number(climate.driver_temp), widget);
        temp->setAlignment(Qt::AlignLeft);
        temp->setFont(Theme::font_14);
        temp->setAlignment(Qt::AlignCenter);
        layout->addWidget(temp);
    }

    StepMeter *fan = new StepMeter(climate.max_speed, climate.speed, widget);

    layout->addSpacing(fan->width() / 2);

    QPushButton *low = new QPushButton(widget);
    low->setFocusPolicy(Qt::NoFocus);
    low->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    low->setFlat(true);
    low->setIconSize(Theme::icon_24);
    low->setIcon(Theme::get_instance()->make_button_icon("expand_more", low));
    layout->addWidget(low);

    layout->addWidget(fan);

    QPushButton *high = new QPushButton(widget);
    high->setFocusPolicy(Qt::NoFocus);
    high->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    high->setFlat(true);
    high->setIconSize(Theme::icon_24);
    high->setIcon(Theme::get_instance()->make_button_icon("expand_less", high));
    layout->addWidget(high);

    layout->addSpacing(fan->width() / 2);

    if (climate.passenger_temp > 0) {
        QLabel *temp = new QLabel(QString::number(climate.passenger_temp), widget);
        temp->setAlignment(Qt::AlignRight);
        temp->setFont(Theme::font_14);
        temp->setAlignment(Qt::AlignCenter);
        layout->addWidget(temp);
    }

    return widget;
}
