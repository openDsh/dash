#include "app/config.hpp"
#include "plugins/launcher_plugin.hpp"

#include "app/pages/launcher.hpp"

void LauncherPlugins::get_plugins()
{
    for (const QFileInfo &plugin : Config::plugin_dir("launcher").entryInfoList(QDir::Files)) {
        if (QLibrary::isLibrary(plugin.absoluteFilePath()))
            this->plugins[Config::fmt_plugin(plugin.baseName())] = plugin;
    }
}

LauncherPlugins::LauncherPlugins(QWidget *parent) : QTabWidget(parent)
{
    this->config = Config::get_instance();
    
    this->get_plugins();
    this->dialog = new Dialog(true, this->window());
    this->dialog->set_body(this->dialog_body());
    QPushButton *load_button = new QPushButton("load");
    connect(load_button, &QPushButton::clicked, [this]() {
        QString key = this->selector->get_current();
        if (!key.isNull()) {
            auto plugin_loader = new QPluginLoader(this);
            plugin_loader->setFileName(this->plugins[key].absoluteFilePath());

            if (LauncherPlugin *plugin = qobject_cast<LauncherPlugin *>(plugin_loader->instance())) {
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
    settings_button->setIconSize(Theme::icon_24);
    settings_button->setIcon(Theme::get_instance()->make_button_icon("settings", settings_button));
    connect(settings_button, &QPushButton::clicked, [this]() { this->dialog->open(); });
    this->setCornerWidget(settings_button);

    for (auto launcher_plugin : this->config->get_launcher_plugins()) {
        auto plugin_loader = new QPluginLoader(this);
        plugin_loader->setFileName(this->plugins[launcher_plugin].absoluteFilePath());

        if (LauncherPlugin *plugin = qobject_cast<LauncherPlugin *>(plugin_loader->instance())) {
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

    this->selector = new Selector(this->plugins.keys(), 0, Theme::font_14, this);
    layout->addWidget(this->selector);
    layout->addWidget(Theme::get_instance()->br(widget));

    QLabel *label = new QLabel("active plugins", widget);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    this->active_plugins_list = new QListWidget(widget);
    Theme::to_touch_scroller(this->active_plugins_list);
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

LauncherPage::LauncherPage(QWidget *parent) : QStackedWidget(parent)
{
    this->theme = Theme::get_instance();

    this->plugin_tabs = new LauncherPlugins(this);
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

    QLabel *label = new QLabel("load launcher plugin", widget);
    label->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->setSpacing(0);

    QPushButton *settings_button = new QPushButton(widget);
    settings_button->setFlat(true);
    settings_button->setIconSize(Theme::icon_24);
    settings_button->setIcon(this->theme->make_button_icon("settings", settings_button));
    connect(settings_button, &QPushButton::clicked, [this]() { this->plugin_tabs->dialog->open(); });

    layout2->addStretch();
    layout2->addWidget(settings_button);

    layout->addLayout(layout2);
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();

    return widget;
}
