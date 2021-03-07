#pragma once

#include <QtWidgets>
#include <QPluginLoader>

#include "app/widgets/selector.hpp"
#include "app/widgets/dialog.hpp"

#include "app/pages/page.hpp"

class Arbiter;
class LauncherPage;

class LauncherPlugins : public QTabWidget {
    Q_OBJECT

   public:
    LauncherPlugins(Arbiter &arbiter, QWidget *parent = nullptr);

   private:
    void get_plugins();

    Arbiter &arbiter;
    Config *config;

    QMap<QString, QFileInfo> plugins;
    QList<QPluginLoader *> active_plugins; // keep track of widgets not plugins (only one instance of the plugin actually gets created anyway)
    QListWidget *active_plugins_list;
    Selector *selector;
    Dialog *dialog;

    QWidget *dialog_body();

    friend class LauncherPage;
};

class LauncherPage : public QStackedWidget, public Page {
    Q_OBJECT

   public:
    LauncherPage(Arbiter &arbiter, QWidget *parent = nullptr);

    void init() override;

   private:
    LauncherPlugins *plugin_tabs;

    QWidget *load_msg();
};
