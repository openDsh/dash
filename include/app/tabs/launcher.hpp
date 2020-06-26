#ifndef LAUNCHER_HPP_
#define LAUNCHER_HPP_

#include <QProcess>
#include <QtWidgets>

#include <app/config.hpp>
#include <app/theme.hpp>

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
    QProcess *process;
    QVBoxLayout *container;
    XWorker *worker;

   signals:
    void closed();
    void opened();
};

class LauncherTab : public QWidget {
    Q_OBJECT

   public:
    LauncherTab(QWidget *parent = nullptr);

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

#endif
