#include <QElapsedTimer>
#include <unistd.h>
#include <typeinfo>

#include "app/config.hpp"
#include "app/arbiter.hpp"
#include "app/session.hpp"

#include "app.hpp"

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

XWorker::XWorker(QObject *parent) : QObject(parent)
{
    this->display = XOpenDisplay(nullptr);
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

EmbeddedApp::EmbeddedApp(QWidget *parent) : QWidget(parent), process()
{
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
    window->setFlags(Qt::FramelessWindowHint);
    usleep(500000);

    this->container->addWidget(QWidget::createWindowContainer(window, this));

    emit opened();
}

void EmbeddedApp::end()
{
    this->process.terminate();
    delete this->container->takeAt(0);
    emit closed();
}

Launcher::Launcher(Arbiter &arbiter, QSettings &settings, int idx, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
    , settings(settings)
    , idx(idx)
{
    this->setObjectName("App");

    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    this->app = new EmbeddedApp(this);
    connect(this->app, &EmbeddedApp::opened, [layout]() { layout->setCurrentIndex(1); });
    connect(this->app, &EmbeddedApp::closed, [layout]() { layout->setCurrentIndex(0); });

    auto launcher_app = this->settings.value(this->app_key()).toString();
    this->auto_launch = !launcher_app.isEmpty();

    layout->addWidget(this->launcher_widget());
    layout->addWidget(this->app);

    if (this->auto_launch)     
        this->app->start(launcher_app);
}

Launcher::~Launcher()
{
    delete this->app;
}

void Launcher::update_idx(int idx)
{
    if (idx == this->idx)
        return;

    QString home;
    QString app;
    if (this->settings.contains(this->home_key()))
        home = this->settings.value(this->home_key(), this->DEFAULT_DIR).toString();
    if (this->settings.contains(this->app_key()))
        app = this->settings.value(this->app_key()).toString();

    this->settings.remove(QString::number(this->idx));
    this->idx = idx;
    if (!home.isNull())
        this->settings.setValue(this->home_key(), home);
    if (!app.isNull())
        this->settings.setValue(this->app_key(), app);
}

QWidget *Launcher::launcher_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    this->path_label = new QLabel(this->settings.value(this->home_key(), this->DEFAULT_DIR).toString(), this);

    layout->addStretch(1);
    layout->addWidget(this->path_label, 1);
    layout->addWidget(this->app_select_widget(), 6);
    layout->addWidget(this->config_widget(), 1, Qt::AlignRight);
    layout->addStretch(1);

    return widget;
}

QWidget *Launcher::app_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QString root_path(this->path_label->text());

    QPushButton *home_button = new QPushButton(widget);
    home_button->setFlat(true);
    home_button->setCheckable(true);
    home_button->setChecked(true);
    connect(home_button, &QPushButton::clicked, [this](bool checked = false) {
        if (checked)
            this->settings.setValue(this->home_key(), this->path_label->text());
        else
            this->settings.remove(this->home_key());
    });
    this->arbiter.forge().iconize("playlist_add", "playlist_add_check", home_button, 32, true);
    layout->addWidget(home_button, 0, Qt::AlignTop);

    this->folders = new QListWidget(widget);
    Session::Forge::to_touch_scroller(this->folders);
    this->populate_dirs(root_path);
    layout->addWidget(this->folders, 4);

    this->apps = new QListWidget(widget);
    Session::Forge::to_touch_scroller(this->apps);
    this->populate_apps(root_path);
    connect(this->apps, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        QString app_path = this->path_label->text() + '/' + item->text();
        if (this->auto_launch)
            this->settings.setValue(this->app_key(), app_path);
        this->app->start(app_path);
    });
    connect(this->folders, &QListWidget::itemClicked, [this, home_button](QListWidgetItem *item) {
        if (!item->isSelected())
            return;

        this->apps->clear();
        QString current_path(item->data(Qt::UserRole).toString());
        this->path_label->setText(current_path);
        this->populate_apps(current_path);
        this->populate_dirs(current_path);

        home_button->setChecked(this->settings.value(this->home_key(), this->DEFAULT_DIR).toString() == current_path);
    });
    layout->addWidget(this->apps, 5);

    return widget;
}

QWidget *Launcher::config_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QCheckBox *checkbox = new QCheckBox("launch at startup", widget);
    checkbox->setChecked(this->auto_launch);
    connect(checkbox, &QCheckBox::toggled, [this, checkbox](bool checked) {
        this->auto_launch = checked;
        if (this->auto_launch && this->apps->currentItem())
            this->settings.setValue(this->app_key(), this->path_label->text() + '/' + this->apps->currentItem()->text());
        else
            this->settings.remove(this->app_key());
    });

    layout->addWidget(checkbox);

    return widget;
}

void Launcher::populate_dirs(QString path)
{
    this->folders->clear();
    QDir current_dir(path);
    for (QFileInfo dir : current_dir.entryInfoList(QDir::AllDirs | QDir::Readable)) {
        if (dir.fileName() == ".")
            continue;

        QListWidgetItem *item = new QListWidgetItem(dir.fileName(), this->folders);
        if (dir.fileName() == "..") {
            item->setText("↲");

            if (current_dir.isRoot())
                item->setFlags(Qt::NoItemFlags);
        }
        else {
            item->setText(dir.fileName());
        }
        item->setData(Qt::UserRole, QVariant(dir.absoluteFilePath()));
    }
}

void Launcher::populate_apps(QString path)
{
    for (QString app : QDir(path).entryList(QDir::Files | QDir::Executable))
        new QListWidgetItem(app, this->apps);
}

QList<QWidget *> App::widgets()
{
    int size = this->loaded_widgets.size();
    this->loaded_widgets.append(new Launcher(*this->arbiter, this->settings, size));
    return this->loaded_widgets.mid(size);
}

void App::remove_widget(int idx)
{
    LauncherPlugin::remove_widget(idx);

    this->settings.remove(QString::number(idx));
    for (int i = 0; i < this->loaded_widgets.size(); i++) {
        auto typeStr(typeid(this->loaded_widgets[i]).name());
        qDebug() << typeStr;
      
        if (Launcher *launcher = qobject_cast<Launcher *>(this->loaded_widgets[i]))
            launcher->update_idx(i);
        
       
    }
}
