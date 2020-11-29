#include <QElapsedTimer>
#include <unistd.h>

#include "app/config.hpp"
#include "app/theme.hpp"

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
        for (unsigned long i = 0; i < client_list.size / sizeof(Window); i++) {
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

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *button = new QPushButton(this);
    button->setFlat(true);
    button->setIconSize(Theme::icon_20);
    connect(button, &QPushButton::clicked, [this]() { this->end(); });
    button->setIcon(Theme::get_instance()->make_button_icon("close", button));
    
    layout->addWidget(button, 0, Qt::AlignRight);

    this->container = new QVBoxLayout();
    layout->addLayout(this->container, 1);
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

Launcher::Launcher(QWidget *parent) : QWidget(parent)
{
    this->setObjectName("App");

    this->theme = Theme::get_instance();
    this->config = Config::get_instance();

    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    this->app = new EmbeddedApp(parent);
    connect(this->app, &EmbeddedApp::opened, [layout]() { layout->setCurrentIndex(1); });
    connect(this->app, &EmbeddedApp::closed, [layout]() { layout->setCurrentIndex(0); });

    layout->addWidget(this->launcher_widget());
    layout->addWidget(this->app);

    if (this->config->get_launcher_auto_launch() && !this->config->get_launcher_app().isEmpty())
        this->app->start(this->config->get_launcher_app());
}

Launcher::~Launcher()
{
    delete this->app;
}

QWidget *Launcher::launcher_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    this->path_label = new QLabel(this->config->get_launcher_home(), this);
    this->path_label->setFont(Theme::font_14);

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

    QString root_path(this->config->get_launcher_home());

    QPushButton *home_button = new QPushButton(widget);
    home_button->setFlat(true);
    home_button->setCheckable(true);
    home_button->setChecked(this->config->get_launcher_home() == root_path);
    home_button->setIconSize(Theme::icon_32);
    connect(home_button, &QPushButton::clicked, [this](bool checked = false) {
        this->config->set_launcher_home(checked ? this->path_label->text() : QDir().absolutePath());
    });
    home_button->setIcon(theme->make_button_icon("playlist_add", home_button, "playlist_add_check"));
    layout->addWidget(home_button, 0, Qt::AlignTop);

    this->folders = new QListWidget(widget);
    this->folders->setFont(Theme::font_14);
    Theme::to_touch_scroller(this->folders);
    this->populate_dirs(root_path);
    layout->addWidget(this->folders, 4);

    this->apps = new QListWidget(widget);
    this->apps->setFont(Theme::font_14);
    Theme::to_touch_scroller(this->apps);
    this->populate_apps(root_path);
    connect(this->apps, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        QString app_path = this->path_label->text() + '/' + item->text();
        if (this->config->get_launcher_auto_launch()) this->config->set_launcher_app(app_path);
        this->app->start(app_path);
    });
    connect(this->folders, &QListWidget::itemClicked, [this, home_button](QListWidgetItem *item) {
        if (!item->isSelected()) return;

        this->apps->clear();
        QString current_path(item->data(Qt::UserRole).toString());
        this->path_label->setText(current_path);
        this->populate_apps(current_path);
        this->populate_dirs(current_path);

        home_button->setChecked(this->config->get_launcher_home() == current_path);
    });
    layout->addWidget(this->apps, 5);

    return widget;
}

QWidget *Launcher::config_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QCheckBox *checkbox = new QCheckBox("launch at startup", widget);
    checkbox->setFont(Theme::font_14);
    checkbox->setChecked(this->config->get_launcher_auto_launch());
    connect(checkbox, &QCheckBox::toggled, [this, checkbox](bool checked) {
        this->config->set_launcher_auto_launch(checked);
        QString launcher_app;
        if (checked) {
            launcher_app.append(this->path_label->text() + '/');
            if (this->apps->currentItem() == nullptr) {
                if (this->apps->count() > 0)
                    launcher_app.append(this->apps->item(0)->text());
                else
                    checkbox->setChecked(false);
            }
            else {
                launcher_app.append(this->apps->currentItem()->text());
            }
        }
        this->config->set_launcher_app(launcher_app);
    });

    layout->addWidget(checkbox);

    return widget;
}

void Launcher::populate_dirs(QString path)
{
    this->folders->clear();
    QDir current_dir(path);
    for (QFileInfo dir : current_dir.entryInfoList(QDir::AllDirs | QDir::Readable)) {
        if (dir.fileName() == ".") continue;

        QListWidgetItem *item = new QListWidgetItem(dir.fileName(), this->folders);
        if (dir.fileName() == "..") {
            item->setText("↲");

            if (current_dir.isRoot()) item->setFlags(Qt::NoItemFlags);
        }
        else {
            item->setText(dir.fileName());
        }
        item->setData(Qt::UserRole, QVariant(dir.absoluteFilePath()));
    }
}

void Launcher::populate_apps(QString path)
{
    for (QString app : QDir(path).entryList(QDir::Files | QDir::Executable)) new QListWidgetItem(app, this->apps);
}

App::~App()
{
    for (auto widget : this->loaded_widgets)
        delete widget;
}

QList<QWidget *> App::widgets()
{
    int size = this->loaded_widgets.size();
    this->loaded_widgets.append(new Launcher());
    return this->loaded_widgets.mid(size);
}
