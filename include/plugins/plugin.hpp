#pragma once

#include <QString>

class Plugin
{
public:
    virtual ~Plugin() = default;

    virtual QList<QWidget *> tabs() = 0;
};


#define Plugin_iid "org.dash.Plugin"

Q_DECLARE_INTERFACE(Plugin, Plugin_iid)
