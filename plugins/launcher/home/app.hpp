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
class DesktopEntry;
class ILauncherPlugin;

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

class ILauncherPlugin : public LauncherPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID LauncherPlugin_iid FILE "home.json")
    Q_INTERFACES(LauncherPlugin)

public:
    ILauncherPlugin() { this->settings.beginGroup("App"); }
    ~ILauncherPlugin();
    QList<QWidget *> widgets() override;
    void remove_widget(int idx) override;
    void add_widget(QWidget *widget) override;

private:
    void init();
    
   
};

class Home : public QWidget {
    Q_OBJECT

   public:
    Home(Arbiter *arbiter, QSettings &settings, int idx, ILauncherPlugin *plugin, QWidget *parent = nullptr);
    ~Home();
    void update_idx(int idx);

   private:
    //const QString DEFAULT_DIR = QDir().absolutePath();
    QGridLayout *entries_grid;
    QWidget *container;
    QScrollArea *scroll_area;
    QStackedLayout *layout;
    QWidget *config_widget();//not implemented
  
    Arbiter *arbiter;
    QSettings &settings;
    ILauncherPlugin *plugin;
    int idx;

    void setup_ui();
   
};


class DesktopEntry : public QWidget{
    Q_OBJECT

public:
    DesktopEntry(QString fileLocation, Arbiter *arbiter, ILauncherPlugin *plugin, QWidget *parent = nullptr);
    
    static QList<DesktopEntry *> get_entries(Arbiter *arbiter, ILauncherPlugin *plugin);
    static int resolutionFromString(QString string);
  
    inline QString get_exec() { return this->exec_; };
    inline QString get_icon() { return this->icon_; };
    inline QString get_name() { return this->name_; };
    inline QString get_path() { return this->path_; };
    inline QList<QString> get_args() {return this->args_;};
    QPixmap get_pixmap();
    QWidget *get_widget();

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
    ILauncherPlugin *plugin;
    QString exec_;
    QString path_;
    QString name_;
    QString icon_;
    QList<QString> args_;

    void setup_ui();

};

