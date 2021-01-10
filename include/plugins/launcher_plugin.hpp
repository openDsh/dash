#pragma once

#include "plugins/plugin.hpp"

class LauncherPlugin : public Plugin {
   public:
    LauncherPlugin() { this->settings.beginGroup("Launcher"); }
    virtual ~LauncherPlugin() = default;
    virtual void remove_widget(int idx) { this->loaded_widgets.removeAt(idx); }

   protected:
    QList<QWidget *> loaded_widgets;
};

#define LauncherPlugin_iid "openDsh.plugins.LauncherPlugin"

Q_DECLARE_INTERFACE(LauncherPlugin, LauncherPlugin_iid)
