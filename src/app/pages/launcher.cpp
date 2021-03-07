#include "app/arbiter.hpp"
#include "app/config.hpp"
#include "plugins/launcher_plugin.hpp"

#include "app/pages/launcher.hpp"

void LauncherPlugins::get_plugins()
{
    for (const QFileInfo &plugin : Session::plugin_dir("launcher").entryInfoList(QDir::Files)) {
        if (QLibrary::isLibrary(plugin.absoluteFilePath()))
            this->plugins[Session::fmt_plugin(plugin.baseName())] = plugin;
    }
}

LauncherPlugins::LauncherPlugins(Arbiter &arbiter, QWidget *parent)
    : QTabWidget(parent)
    , arbiter(arbiter)
{
    this->config = Config::get_instance();
    
    this->get_plugins();
    this->dialog = new Dialog(this->arbiter, true, this->window());
    this->dialog->set_body(this->dialog_body());
    QPushButton *load_button = new QPushButton("load");
    connect(load_button, &QPushButton::clicked, [this]() {
        QString key = this->selector->get_current();
        if (!key.isNull()) {
            auto plugin_loader = new QPluginLoader(this);
            plugin_loader->setFileName(this->plugins[key].absoluteFilePath());

            if (LauncherPlugin *plugin = qobject_cast<LauncherPlugin *>(plugin_loader->instance())) {
                plugin->dashize(&this->arbiter);
                for (QWidget *tab : plugin->widgets())
                    this->addTab(tab, tab->objectName());
                this->active_plugins.append(plugin_loader);
                this->active_plugins_list->addItem(key);
                this->config->set_launcher_plugin(key);
            }
            else {
                delete plugin_loader;
            }
        }
    });
    this->dialog->set_button(load_button);

    QPushButton *settings_button = new QPushButton(this);
    settings_button->setFlat(true);
    this->arbiter.forge().iconize("settings", settings_button, 24);
    connect(settings_button, &QPushButton::clicked, [this]() { this->dialog->open(); });
    this->setCornerWidget(settings_button);

    for (auto launcher_plugin : this->config->get_launcher_plugins()) {
        auto plugin_loader = new QPluginLoader(this);
        plugin_loader->setFileName(this->plugins[launcher_plugin].absoluteFilePath());

        if (LauncherPlugin *plugin = qobject_cast<LauncherPlugin *>(plugin_loader->instance())) {
            plugin->dashize(&this->arbiter);
            for (QWidget *tab : plugin->widgets())
                this->addTab(tab, tab->objectName());
            this->active_plugins.append(plugin_loader);
            this->active_plugins_list->addItem(launcher_plugin);
        }
        else {
            delete plugin_loader;
        }
    }
}

QWidget *LauncherPlugins::dialog_body()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    this->selector = new Selector(this->plugins.keys(), 0, this->arbiter.forge().font(14), this->arbiter, this);
    layout->addWidget(this->selector);
    layout->addWidget(Session::Forge::br());

    QLabel *label = new QLabel("Active Plugins", widget);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    this->active_plugins_list = new QListWidget(widget);
    Session::Forge::to_touch_scroller(this->active_plugins_list);
    connect(this->active_plugins_list, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        int idx = this->active_plugins_list->row(item);
        QWidget *tab = this->widget(idx);
        this->removeTab(idx);
        delete tab;

        this->config->set_launcher_plugin(item->text(), true);

        delete this->active_plugins_list->takeItem(idx);

        if (LauncherPlugin *plugin = qobject_cast<LauncherPlugin *>(this->active_plugins[idx]->instance()))
            plugin->remove_widget(idx);

        this->active_plugins.removeAt(idx);
    });
    layout->addWidget(this->active_plugins_list);

    return widget;
}

LauncherPage::LauncherPage(Arbiter &arbiter, QWidget *parent)
    : QStackedWidget(parent)
    , Page(arbiter, "Launcher", "widgets", true, this)
{
}

void LauncherPage::init()
{
    this->plugin_tabs = new LauncherPlugins(this->arbiter, this);
    connect(this->plugin_tabs, &QTabWidget::currentChanged, [this](int idx) { this->setCurrentIndex((idx == -1) ? 0 : 1); });

    this->addWidget(this->load_msg());
    this->addWidget(this->plugin_tabs);

    if (this->plugin_tabs->count() > 0)
        this->setCurrentIndex(1);
}

QWidget *LauncherPage::load_msg()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *label = new QLabel("Load Launcher Plugin", widget);
    label->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->setSpacing(0);

    QPushButton *settings_button = new QPushButton(widget);
    settings_button->setFlat(true);
    this->arbiter.forge().iconize("settings", settings_button, 24);
    connect(settings_button, &QPushButton::clicked, [this]() { this->plugin_tabs->dialog->open(); });

    layout2->addStretch();
    layout2->addWidget(settings_button);

    layout->addLayout(layout2);
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();

    return widget;
}
