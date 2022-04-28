#include <QTime>

#include "app/services/clock.hpp"

Clock::Clock()
    : QObject()
    , timer()
{
    connect(&this->timer, &QTimer::timeout, [this]{ emit ticked(QTime::currentTime()); });
    this->timer.start(100);
}
