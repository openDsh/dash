#pragma once

#include <QObject>
#include <QtPlugin>
#include "plugins/plugin.hpp"
#include "app/tabs/climate.hpp"

class Test : public QObject, Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid FILE "test.json")
    Q_INTERFACES(Plugin)

   public:
    Test();
    QList<QWidget *> tabs() override;

   private:
    Climate *climate;
};
