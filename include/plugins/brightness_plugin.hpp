#pragma once

#include <QObject>
#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>

#include "plugins/plugin.hpp"

class BrightnessPlugin : public Plugin {
   public:
    virtual ~BrightnessPlugin() = default;
    virtual void set(int brightness) = 0;

    static QWidget *slider(bool buttons, QWidget *parent = nullptr);
};

#define BrightnessPlugin_iid "org.dash.BrightnessPlugin"

Q_DECLARE_INTERFACE(BrightnessPlugin, BrightnessPlugin_iid)
