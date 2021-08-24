#pragma once

#include <QDir>
#include <QProcess>
#include <QtWidgets>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

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

class Arbiter;
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
    EmbeddedApp(Arbiter *arbiter, QWidget *parent = nullptr);
    ~EmbeddedApp();

    void start(QString app);
    void end();

   private:
    QProcess process;
    QVBoxLayout *container;
    XWorker *worker;
    Arbiter *arbiter;

   signals:
    void closed();
    void opened();
};


//TODO: REFACTOR - change name to ILauncherPlugin
class App : public LauncherPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID LauncherPlugin_iid FILE "home.json")
    Q_INTERFACES(LauncherPlugin)

public:
    App();
    QList<QWidget *> widgets() override;
    void remove_widget(int idx) override;

    void add_widget(QWidget *widget);

public slots:

    signals:

   // void widget_added() override;


private:
    void init();
    QList<QWidget *> _widgets;


};

class DesktopEntry : public QWidget{
    Q_OBJECT

public:
    DesktopEntry(QString fileLocation, Arbiter *arbiter, App *plugin, QWidget *parent = nullptr);
    QWidget *get_widget();
    static QList<DesktopEntry *> get_entries(Arbiter *arbiter, App *plugin);
    static int resolutionFromString(QString string);
   // static QList<QWidget *> get_all_widgets();
    inline QString get_exec() { return this->exec_; };
    inline QString get_icon() { return this->icon_; };
    inline QString get_name() { return this->name_; };
    inline QString get_path() { return this->path_; };
    inline QList<QString> get_args() {return this->args_;};
    QPixmap get_pixmap();

public slots:

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *iconLabel;
    QLabel *nameLabel;

    Arbiter *arbiter;
    App *plugin;
    QString exec_;
    QString path_;
    QString name_;
    QString icon_;
    QList<QString> args_;

    void setup_ui();

};
//TODO: REFACTOR - rename to home
class Launcher : public QWidget {
    Q_OBJECT

   public:
    Launcher(Arbiter *arbiter, QSettings &settings, int idx, App *plugin, QWidget *parent = nullptr);
    ~Launcher();
    void update_idx(int idx);

   private:
    const QString DEFAULT_DIR = QDir().absolutePath();

//  QWidget *launcher_widget();
   // QWidget *homepage_widget();
    QWidget *home_widget();
   // QWidget *app_select_widget();
    QWidget *config_widget();
   // void populate_dirs(QString path);
   // void populate_apps(QString path);

    inline QString home_key() { return QString("%1/home").arg(this->idx); }
    inline QString app_key() { return QString("%1/app").arg(this->idx); }

    Arbiter *arbiter;
    QSettings &settings;
    EmbeddedApp *app;
    App *plugin;
    QLabel *path_label;
   // QListWidget *folders;
   // QListWidget *apps;
    int idx;
    //bool auto_launch = false;

};

