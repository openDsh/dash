#pragma once

#include <QString>

class Plugin
{
public:
    virtual ~Plugin() = default;
    virtual void init(QString message) = 0;

    // maybe have a get_tab_* where you load the tab youre looking for?
    // and then some will be prepopulated (like climate etc)
};


#define Plugin_iid "org.dash.Plugin"

Q_DECLARE_INTERFACE(Plugin, Plugin_iid)
