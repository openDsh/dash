#pragma once

#include <QString>
#include "canbus/ICANBus.hpp"

class Plugin
{
public:
    virtual ~Plugin() = default;
    virtual bool init(ICANBus* canbus) = 0;
    virtual QList<QWidget *> tabs() = 0;
};


#define Plugin_iid "org.dash.Plugin"

Q_DECLARE_INTERFACE(Plugin, Plugin_iid)
