#pragma once

#include <QtWidgets>
#include <QPluginLoader>

#include "app/widgets/selector.hpp"
#include "app/widgets/dialog.hpp"

class LauncherPage;

class LauncherPlugins : public QTabWidget {
    Q_OBJECT

   public:
    LauncherPlugins(QWidget *parent = nullptr);

   private:
    void get_plugins();

    QMap<QString, QFileInfo> plugins;
    QSet<QPluginLoader *> active_plugins; // keep track of widgets not plugins
    QListWidget *active_plugins_list;
    Selector *selector;
    Dialog *dialog;

    QWidget *dialog_body();

    friend class LauncherPage;
};

class LauncherPage : public QStackedWidget {
    Q_OBJECT

   public:
    LauncherPage(QWidget *parent = nullptr);

   private:
    Theme *theme;
    LauncherPlugins *plugin_tabs;

    QWidget *load_msg();
};
