#pragma once

#include "plugins/plugin.hpp"

class LauncherPlugin : public Plugin {
   public:
    virtual ~LauncherPlugin() = default;
};

#define LauncherPlugin_iid "org.dash.LauncherPlugin"

Q_DECLARE_INTERFACE(LauncherPlugin, LauncherPlugin_iid)
