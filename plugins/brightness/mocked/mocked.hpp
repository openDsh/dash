#pragma once

#include <QObject>
#include <QMainWindow>

#include "plugins/brightness_plugin.hpp"

class Mocked : public QObject, BrightnessPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID BrightnessPlugin_iid FILE "mocked.json")
    Q_INTERFACES(BrightnessPlugin)

   public:
    Mocked();
    void set(int brightness) override;

   private:
    QMainWindow *window;
};
