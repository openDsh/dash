#pragma once

#include <QObject>
#include <QtPlugin>
#include "plugins/plugin.hpp"
#include "app/tabs/climate.hpp"
#include "canbus/ICANBus.hpp"

class Test : public QObject, Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "test.json")
    Q_INTERFACES(Plugin)

   public:
    Test();
    QList<QWidget *> tabs() override;
    bool init(ICANBus* canbus) override;


   private:
    Climate *climate;
};
