#pragma once

#include <QList>
#include <QWidget>

class Theme;
class Config;
class Shortcuts;

class Plugin {
   public:
    virtual ~Plugin() = default;

    virtual QList<QWidget *> widgets() { return QList<QWidget *>{}; };

   protected:
    static const Theme *theme;
    static const Config *config;
    static const Shortcuts *shortcuts;
};

#define Plugin_iid "org.dash.Plugin"

Q_DECLARE_INTERFACE(Plugin, Plugin_iid)
