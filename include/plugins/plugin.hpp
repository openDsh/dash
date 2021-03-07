#pragma once

#include <QApplication>
#include <QList>
#include <QSettings>
#include <QWidget>

class Arbiter;

class Plugin {
   public:
    Plugin() : settings(qApp->organizationName(), "plugins") {}
    virtual ~Plugin() = default;

    inline void dashize(Arbiter *arbiter)
    {
        if (!this->arbiter)
            this->arbiter = arbiter;
    }
    virtual QList<QWidget *> widgets() { return QList<QWidget *>{}; };

   protected:
    QSettings settings;
    Arbiter *arbiter = nullptr;
};

#define Plugin_iid "openDsh.plugins.Plugin"

Q_DECLARE_INTERFACE(Plugin, Plugin_iid)
