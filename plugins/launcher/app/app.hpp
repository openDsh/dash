#pragma once

#include <QProcess>
#include <QtWidgets>

#include "plugins/launcher_plugin.hpp"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#undef Bool
#undef CurrentTime
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted

class Config;
class Theme;

class XWorker : public QObject {
    Q_OBJECT

   public:
    XWorker(QObject *parent = nullptr);
    int get_window(uint64_t pid);

   private:
    const int MAX_RETRIES = 60;

    struct WindowProp {
        WindowProp(char *prop, unsigned long size);
        ~WindowProp();

        void *prop;
        unsigned long size;
    };

    WindowProp get_window_prop(Window window, Atom type, const char *name);

    Display *display;
    Window root_window;
};

class EmbeddedApp : public QWidget {
    Q_OBJECT

   public:
    EmbeddedApp(QWidget *parent = nullptr);
    ~EmbeddedApp();

    void start(QString app);
    void end();

   private:
    QProcess process;
    QVBoxLayout *container;
    XWorker *worker;

   signals:
    void closed();
    void opened();
};

class Launcher : public QWidget {
    Q_OBJECT

   public:
    Launcher(QWidget *parent = nullptr);

   private:
    QWidget *launcher_widget();
    QWidget *app_select_widget();
    QWidget *config_widget();
    void populate_dirs(QString path);
    void populate_apps(QString path);

    Theme *theme;
    Config *config;
    EmbeddedApp *app;
    QLabel *path_label;
    QListWidget *folders;
    QListWidget *apps;
};

class App : public QObject, LauncherPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID LauncherPlugin_iid FILE "app.json")
    Q_INTERFACES(LauncherPlugin)

   public:
    App() {}
    QList<QWidget *> widgets() override;
};
