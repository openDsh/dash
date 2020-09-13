#pragma once

#include <QList>
#include <QWidget>

class Plugin {
   public:
    virtual ~Plugin() = default;

    virtual QList<QWidget *> widgets() = 0;
};

#define Plugin_iid "org.dash.Plugin"

Q_DECLARE_INTERFACE(Plugin, Plugin_iid)
