#pragma once

#include <QList>
#include <QApplication>
#include <QSettings>
#include <QWidget>

class Theme;
class Config;
class Shortcuts;

class Plugin {
   public:
    Plugin() : settings(qApp->organizationName(), "plugins") {}
    virtual ~Plugin() = default;

    virtual QList<QWidget *> widgets() { return QList<QWidget *>{}; };

   protected:
    static const Theme *theme;
    static const Config *config;
    static const Shortcuts *shortcuts;

    QSettings settings;
};

#define Plugin_iid "openDsh.plugins.Plugin"

Q_DECLARE_INTERFACE(Plugin, Plugin_iid)
