#pragma once

#include <QList>
#include <QWidget>

#include <app/theme.hpp>
#include <app/config.hpp>
#include <app/shortcuts.hpp>

class Plugin {
   public:
    virtual ~Plugin() = default;

    virtual QList<QWidget *> widgets() { return QList<QWidget *>{}; };

   private:
    Theme *theme = Theme::get_instance();
    Config *config = Config::get_instance();
    Shortcuts *shortcuts = Shortcuts::get_instance();
};

#define Plugin_iid "org.dash.Plugin"

Q_DECLARE_INTERFACE(Plugin, Plugin_iid)
