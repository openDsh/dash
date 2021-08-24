#pragma once

#include <QWidget>
#include "plugins/plugin.hpp"

class LauncherPlugin : public Plugin {
    Q_OBJECT

   public:
    LauncherPlugin() { this->settings.beginGroup("Launcher"); }
    virtual ~LauncherPlugin() = default;
    virtual void remove_widget(int idx) { this->loaded_widgets.removeAt(idx); }

public slots:

    signals:
     void widget_added(QWidget *widget);

   protected:
    QList<QWidget *> loaded_widgets;
};

#define LauncherPlugin_iid "openDsh.plugins.LauncherPlugin"

Q_DECLARE_INTERFACE(LauncherPlugin, LauncherPlugin_iid)
