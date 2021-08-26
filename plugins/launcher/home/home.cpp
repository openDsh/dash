#include <QElapsedTimer>
#include <unistd.h>
#include <QString>
#include <QProcessEnvironment>

#include "app/config.hpp"
#include "app/arbiter.hpp"
#include "app/session.hpp"
#include "app/pages/launcher.hpp"

#include "home.hpp"

XWorker::WindowProp::WindowProp(char *prop, unsigned long size)
{
    this->size = size;
    this->prop = new char[this->size + 1];

    std::copy(prop, prop + size, (char *)this->prop);
    ((char *)this->prop)[size] = '\0';
}

XWorker::WindowProp::~WindowProp()
{
    if (this->prop != nullptr) {
        delete (char *)this->prop;
        this->prop = nullptr;
    }
}

//REFACTOR: create XInfo Class and implement classes for traversing the x tree, tracking newly opened
//windows
XWorker::XWorker(QObject *parent) : QObject(parent)
{
    this->display = XOpenDisplay(0);
    this->root_window = DefaultRootWindow(this->display);
}

int XWorker::get_window(uint64_t pid)
{
    int retries = 0;
    while (retries < MAX_RETRIES) {
        WindowProp client_list = this->get_window_prop(this->root_window, XA_WINDOW, "_NET_CLIENT_LIST");
        Window *windows = (Window *)client_list.prop;
        for (unsigned long i = 0; i < (client_list.size / sizeof(Window)); i++) {
            if (pid == *(uint64_t *)this->get_window_prop(windows[i], XA_CARDINAL, "_NET_WM_PID").prop)
                return windows[i];
        }
        usleep(500000);
        retries++;
    }

    return -1;
}

XWorker::WindowProp XWorker::get_window_prop(Window window, Atom type, const char *name)
{
    Atom prop = XInternAtom(this->display, name, false);

    Atom actual_type_return;
    int actual_format_return;
    unsigned long nitems_return;
    unsigned long bytes_after_return;
    unsigned char *prop_return;
    XGetWindowProperty(this->display, window, prop, 0, 1024, false, type, &actual_type_return, &actual_format_return,
                       &nitems_return, &bytes_after_return, &prop_return);

    unsigned long size = (actual_format_return / 8) * nitems_return;
    if (actual_format_return == 32) size *= sizeof(long) / 4;

    WindowProp window_prop((char *)prop_return, size);
    XFree(prop_return);

    return window_prop;
}

EmbeddedApp::EmbeddedApp(Arbiter *arbiter, QWidget *parent) : QWidget(parent), process()
{
    this->arbiter = arbiter;
    this->worker = new XWorker(this);

    this->process.setStandardOutputFile(QProcess::nullDevice());
    this->process.setStandardErrorFile(QProcess::nullDevice());
    connect(&this->process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int, QProcess::ExitStatus) { this->end(); });

    this->container = new QVBoxLayout(this);
    this->container->setContentsMargins(0, 0, 0, 0);
}

EmbeddedApp::~EmbeddedApp()
{
    this->process.kill();
    this->process.waitForFinished();

    delete this->container;
    delete this->worker;
}

void EmbeddedApp::start(QString app)
{
    this->process.setProgram(app);
    this->process.start();
    this->process.waitForStarted();

    QWindow *window = QWindow::fromWinId(worker->get_window(this->process.processId()));

    QWindow *thisWindow = QWindow::fromWinId(this->winId());
    window->setParent(thisWindow);
    window->setFlags(Qt::FramelessWindowHint);
    usleep(50000);

    this->container->addWidget(QWidget::createWindowContainer(window, this));
    emit opened();
}

void EmbeddedApp::end()
{
    this->process.terminate();
    delete this->container->takeAt(0);
    emit closed();
}

void ILauncherPlugin::init(){
    
    this->loaded_widgets.push_front(new Home(this->arbiter, this->settings, 0, this));

}

QList<QWidget *> ILauncherPlugin::widgets()
{
    if(this->loaded_widgets.count() == 0) this->init();
    return this->loaded_widgets;
}

void ILauncherPlugin::remove_widget(int idx){
    
    //TODO: remove apps
  
}

void ILauncherPlugin::add_widget(QWidget *widget){

    this->loaded_widgets.push_back(widget);
    emit widget_added(widget);

}

Home::Home(Arbiter *arbiter, QSettings &settings, int idx, ILauncherPlugin *plugin, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
    , settings(settings)
    , idx(idx)
{
    this->plugin = plugin;
    this->setup_ui();
    
}

void Home::setup_ui()
{
    this->setObjectName("Home");
    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    container = new QWidget(this);
    entries_grid = new QGridLayout(container);
    entries_grid->setObjectName(QString::fromUtf8("entries_grid"));
    entries_grid->setSizeConstraint(QLayout::SetDefaultConstraint);
    container->setLayout(entries_grid);
    auto entries = DesktopEntry::get_entries(this->arbiter, this->plugin);
    QScrollArea *scroll_area = new QScrollArea(this);
    Session::Forge::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(container);
    layout->addWidget(scroll_area);

    //TODO: detect width and set appropriately
    int x = 0;
    int y = 0;
    for (int i = 0; i < entries.count(); ++i){

        DesktopEntry *entry = entries[i];
        connect(entry, &DesktopEntry::clicked, [this, entry]() {

           EmbeddedApp *app = new EmbeddedApp(this->arbiter, this);
           app->setObjectName(entry->get_name());
           this->plugin->add_widget(app);
           app->start(entry->get_exec());

        });
        entries_grid->addWidget(entry, y, x, 1, 1);
        //set x & y
        if(x > 6) {
            x = 0;
            y += 1;
        } else {
            x += 1;
        }

    }
  
}

void Home::update_idx(int idx){

    if (idx == this->idx)
        return;

}

QWidget *Home::config_widget()
{
    //NOT IMPLEMENTED
    QWidget *widget = new QWidget(this);
    return widget;
}

DesktopEntry::DesktopEntry(QString fileLocation, Arbiter *arbiter, ILauncherPlugin *plugin, QWidget *parent)
{
    this->arbiter = arbiter;
    this->plugin = plugin;

    QFile inputFile(fileLocation);
    inputFile.open(QIODevice::ReadOnly);

    if (!inputFile.isOpen()) return;  
    //parse desktop entry
    QTextStream stream(&inputFile);
    for (QString line = stream.readLine();
      !line.isNull();
      line = stream.readLine())
    {

        if(line.contains("Exec")){
            QStringList vals = line.split( "=" );
            this->exec_ = vals.value(1);

            QStringList args = vals.value(1).split( " " );
            this->exec_ = args.value(0);
            args.removeAt(0);
            this->args_ = args;
           
        }

        if(line.contains("Name") && !line.contains("Generic") && !line.contains("[") && !line.contains("]")){
            QStringList vals = line.split( "=" );
            this->name_ = vals.value(1);
        }

        if(line.contains("Icon")){
            QStringList vals = line.split( "=" );
            this->icon_ = vals.value(1);
        }

        if(line.contains("[Desktop") && !line.contains("Entry]")) break;//skip extra stuff in Desktop Entry

    };

    this->setup_ui();

}

void DesktopEntry::setup_ui(){
    //setup ui
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    iconLabel = new QLabel(this);
    iconLabel->setMaximumSize(QSize(64, 64));
    iconLabel->setObjectName(QString::fromUtf8("iconLabel"));
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap (this->get_pixmap().scaled(64,64,Qt::KeepAspectRatio));
    iconLabel->show();
    horizontalLayout->addWidget(iconLabel);
    verticalLayout->addLayout(horizontalLayout);
    nameLabel = new QLabel(this);
    QFontMetrics metrics(nameLabel->font());
    QString elidedText = metrics.elidedText(this->get_name(), Qt::ElideRight, (nameLabel->width() * 2) - 16);
    nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
    nameLabel->setText(elidedText);
    nameLabel->setMaximumSize(QSize(124, 64));
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);
    verticalLayout->addWidget(nameLabel);
    this->setMaximumSize(QSize(128, 128));

}

void DesktopEntry::mousePressEvent(QMouseEvent *event){
    emit clicked();
}

int DesktopEntry::resolutionFromString(QString string){

    if(string.contains("scalable")) return 10000;
    if(string.contains("128")) return 128;
    if(string.contains("64")) return 64;
    if(string.contains("48")) return 48;
    if(string.contains("42")) return 42;
    if(string.contains("36")) return 36;
    if(string.contains("32")) return 32;
    if(string.contains("24")) return 24;
    if(string.contains("22")) return 22;
    if(string.contains("16")) return 16;
    return 0;//error
}

QPixmap DesktopEntry::get_pixmap(){

    /* The freedesktop.org standard specifies in which order and directories programs should look for icons:

    $HOME/.icons (for backwards compatibility)
    $XDG_DATA_DIRS/icons
    /usr/share/pixmaps

    from the spec:

    "In the theme directory are also a set of subdirectories containing image files.
    Each directory contains icons designed for a certain nominal icon size and scale,
    as described by the index.theme file. The subdirectories are allowed to be several levels deep,
    e.g. the subdirectory "48x48/apps" in the theme "hicolor" would end up at $basedir/icons/hicolor/48x48/apps{icon_}.{png|svg}"

    **Note:
     This implementation isn't fully spec compliant. It doesn't lookup themes. 
     The current implementation searches the icon directories for all themes and grabs the highest
     resolution it can from the first theme it finds a match in.
    */

    if(this->icon_ == "") {
        //TODO: change icons on dark / light change
        QPixmap pixmap(":/icons/widgets.svg");
        QSize size(512, 512);
        auto icon_mask = pixmap.scaled(size).createMaskFromColor(Qt::transparent);
        auto color = (this->arbiter->theme().mode == Session::Theme::Light) ? QColor(0, 0, 0) : QColor(255, 255, 255);
        color.setAlpha((this->arbiter->theme().mode == Session::Theme::Light) ? 162 : 134);
        QPixmap icon(size);
        icon.fill(color);
        icon.setMask(icon_mask);
        return icon;

    }

    //check if icon is path
    if(this->icon_.contains("/") && (this->icon_.contains(".png") || this->icon_.contains(".svg") || this->icon_.contains(".xpm"))){
        if(!QFile::exists(this->icon_)){
            return QPixmap(":/icons/widgets.svg");
        }
        return QPixmap(this->icon_);
    }

    //check in $HOME/.icons
    auto h_png_path = QDir::homePath() + "/" + this->icon_ + ".png";
    if(QFile::exists(h_png_path)) return QPixmap(h_png_path);
    auto h_svg_path = QDir::homePath() + "/" + this->icon_ + ".svg";
    if(QFile::exists(h_svg_path)) return QPixmap(h_svg_path);
    auto h_xpm_path = QDir::homePath() + "/" + this->icon_ + ".xpm";
    if(QFile::exists(h_xpm_path)) return QPixmap(h_xpm_path);

    //get theme dirs from DATA_DIRS
    QString xdg = qgetenv("XDG_DATA_DIRS");
    auto xdgArr = xdg.split(":");
    for(QString dir_name : xdgArr)
    {
        //drop repeated dirs
        if(dir_name.endsWith("/")) continue;

        //search themes in dir
        //TODO: prefer certain themes
        QDirIterator themes( dir_name + "/icons", QDir::Dirs | QDir::NoDotAndDotDot);
        while ( themes.hasNext() ) {

            QString themeDirPath = themes.next();
            QDir themeDir(themeDirPath);
            QString theme = themeDir.dirName();
            QDirIterator resolutions( themeDirPath, QDir::Dirs | QDir::NoDotAndDotDot);
           
            int highestResolution = 0;
            QString highestResPath = "";

            //get highest resolution icon possible
            while ( resolutions.hasNext() ) {

                QString resolutionPath = resolutions.next();
                QDir resolutionDir(resolutionPath);
                QString resolution = resolutionDir.dirName();
                int iRes = DesktopEntry::resolutionFromString(resolution);

                //skip if higher resolution already available
                if(highestResolution > iRes) continue;
                QDirIterator subdirs( resolutionPath, QDir::Dirs | QDir::NoDotAndDotDot);
                while ( subdirs.hasNext() ) {

                    QDir subDir(subdirs.next());
                    QString sub_dir_name = subDir.dirName();
                    auto xdg_png_path = dir_name + "/icons/" + theme + "/" + resolution + "/" + sub_dir_name + "/" + this->icon_ + ".png";
                    if(QFile::exists(xdg_png_path)) {
                       highestResolution = iRes;
                       highestResPath = xdg_png_path;

                    }
                    auto xdg_svg_path = dir_name + "/icons/" + theme + "/" + resolution + "/" + sub_dir_name + "/" + this->icon_ + ".svg";
                    if(QFile::exists(xdg_svg_path)) {
                        highestResolution = iRes;
                        highestResPath = xdg_svg_path;
                    }

                    auto xdg_xpm_path = dir_name + "/icons/" + theme + "/" + resolution + "/" + sub_dir_name + "/" + this->icon_ + ".xpm";
                    if(QFile::exists(xdg_xpm_path)) {
                        highestResolution = iRes;
                        highestResPath = xdg_xpm_path;
                    }

                }

            }

            if(highestResolution > 0){
                return QPixmap(highestResPath);
            }

         }
    }

    //check pixmaps
    auto pixmap_png_path = "/usr/share/pixmaps/" + this->icon_ + ".png";
    if(QFile::exists(pixmap_png_path)) {
        return QPixmap(pixmap_png_path);
    }
    auto pixmap_svg_path = "/usr/share/pixmaps/" + this->icon_ + ".svg";
    if(QFile::exists(pixmap_svg_path)) {
        return QPixmap(pixmap_svg_path);
    }

    auto pixmap_xpm_path = "/usr/share/pixmaps/" + this->icon_ + ".xpm";
    if(QFile::exists(pixmap_xpm_path)) {
        return QPixmap(pixmap_xpm_path);
    }

    //no icon found, return default
    QPixmap pixmap(":/icons/widgets.svg");
    QSize size(512, 512);
    auto icon_mask = pixmap.scaled(size).createMaskFromColor(Qt::transparent);
    auto color = (this->arbiter->theme().mode == Session::Theme::Light) ? QColor(0, 0, 0) : QColor(255, 255, 255);
    color.setAlpha((this->arbiter->theme().mode == Session::Theme::Light) ? 162 : 134);
    QPixmap icon(size);
    icon.fill(color);
    icon.setMask(icon_mask);

    return icon;
}

QList<DesktopEntry *> DesktopEntry::get_entries(Arbiter *arbiter, ILauncherPlugin *plugin)
{
    /*
     Desktop entry files must reside in the $XDG_DATA_DIRS/applications directory and must have a .desktop file extension.
    If $XDG_DATA_DIRS is not set, then the default path is /usr/share is used.
    If $XDG_DATA_HOME is not set, then the default path ~/.local/share is used.
    Desktop entries are collected from all directories in the $XDG_DATA_DIRS environment variable.
    Directories which appear first in $XDG_CONFIG_DIRS are given precedence when there are several .desktop files with the same name.
    */

    QList<DesktopEntry *> rtn;
    QString xdg = qgetenv("XDG_DATA_DIRS");
    for(QString dir_name : xdg.split(":"))
    {

        auto dir = dir_name + "/applications";
        QDir source(dir);
        if (!source.exists())
            continue;

        QStringList files = source.entryList(QStringList() << "*.desktop", QDir::Files);
        for (QString file: files)
        {
            //TODO: validate & error handle
            QString path = source.absoluteFilePath(file);
            DesktopEntry *entry = new DesktopEntry(path, arbiter, plugin);
            rtn.push_back(entry);

        }

    }

    return rtn;

}
