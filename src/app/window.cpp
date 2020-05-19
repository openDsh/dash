#include <QElapsedTimer>
#include <QtWidgets>
#include <cstdlib>
#include <sstream>

#include <app/tabs/data.hpp>
#include <app/tabs/media.hpp>
#include <app/tabs/launcher.hpp>
#include <app/tabs/settings.hpp>
#include <app/window.hpp>
#include <app/modules/brightness.hpp>

MainWindow::MainWindow()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->config = Config::get_instance();

    this->theme = Theme::get_instance();
    this->theme->set_mode(this->config->get_dark_mode());
    this->theme->set_color(this->config->get_color());
    this->theme->set_scale(this->config->get_scale());

    connect(this->config, &Config::scale_changed, [theme = this->theme](double scale) { theme->set_scale(scale); });

    this->config->add_quick_view("volume", this->volume_widget());
    this->config->add_quick_view("none", new QFrame(this));

    this->config->add_brightness_module("mocked", new MockedBrightnessModule(this));
    this->config->add_brightness_module("x", new XBrightnessModule());
    this->config->add_brightness_module("rpi 7\"", new RpiBrightnessModule());

    BrightnessModule *module = this->config->get_brightness_module(this->config->get_brightness_module());
    module->set_brightness(this->config->get_brightness());

    QFrame *widget = new QFrame(this);
    this->layout = new QStackedLayout(widget);

    this->layout->addWidget(this->window_widget());

    setCentralWidget(widget);
}

QWidget *MainWindow::window_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(this->tabs_widget());
    QWidget *controls_widget = this->controls_widget();
    if (!this->config->get_controls_bar()) controls_widget->hide();
    connect(this->config, &Config::controls_bar_changed,
            [controls_widget](bool controls_bar) { controls_bar ? controls_widget->show() : controls_widget->hide(); });
    layout->addWidget(controls_widget);

    return widget;
}

QTabWidget *MainWindow::tabs_widget()
{
    QTabWidget *widget = new QTabWidget(this);
    widget->setTabPosition(QTabWidget::TabPosition::West);
    widget->setIconSize(this->TAB_SIZE);

    OpenAutoTab *openauto = new OpenAutoTab(this);
    openauto->setObjectName("OpenAuto");
    MediaTab *media = new MediaTab(this);
    media->setObjectName("Media");
    DataTab *data = new DataTab(this);
    data->setObjectName("Data");
    LauncherTab *launcher = new LauncherTab(this);
    launcher->setObjectName("Launcher");
    SettingsTab *settings = new SettingsTab(this);
    settings->setProperty("prevent_disable", true);

    int idx;
    idx = widget->addTab(openauto, QString());
    this->theme->add_tab_icon("directions_car", openauto, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(openauto));
    idx = widget->addTab(media, QString());
    this->theme->add_tab_icon("play_circle_outline", media, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(media));
    idx = widget->addTab(data, QString());
    this->theme->add_tab_icon("speed", data, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(data));
    idx = widget->addTab(launcher, QString());
    this->theme->add_tab_icon("widgets", launcher, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(launcher));
    idx = widget->addTab(settings, QString());
    this->theme->add_tab_icon("tune", settings, Qt::Orientation::Vertical);

    media->fill_tabs();
    settings->fill_tabs();

    connect(this->config, &Config::brightness_changed, [this, widget](int position) {
        BrightnessModule *module = this->config->get_brightness_module(this->config->get_brightness_module());
        module->set_brightness(position);
        if (widget->currentIndex() == 0 && module->update_androidauto()) emit set_openauto_state(position);
    });
    connect(this->config, &Config::page_changed, [this, widget](QWidget *page, bool enabled) {
        int idx = widget->indexOf(page);
        widget->setTabEnabled(idx, enabled);
        widget->setTabIcon(idx, (enabled) ? this->theme->get_tab_icon(idx) : QIcon());
        widget->ensurePolished();
    });
    connect(this->theme, &Theme::icons_updated,
            [widget, tab_size = this->TAB_SIZE](QList<tab_icon_t> &tab_icons, QList<button_icon_t> &button_icons,
                                                double scale) {
                widget->setIconSize(tab_size * scale);
                for (auto &icon : tab_icons) {
                    int idx = widget->indexOf(icon.first);
                    if (widget->isTabEnabled(idx)) widget->tabBar()->setTabIcon(idx, icon.second);
                }
                for (auto &icon : button_icons) {
                    QPushButton *button = std::get<0>(icon);
                    QSize size = std::get<2>(icon);
                    size.rwidth() *= scale;
                    size.rheight() *= scale;
                    button->setIconSize(size);
                    button->setIcon(std::get<1>(icon));
                }
            });
    connect(widget, &QTabWidget::currentChanged, [this](int index) {
        BrightnessModule *module = this->config->get_brightness_module(this->config->get_brightness_module());
        int alpha = module->update_androidauto() ? this->config->get_brightness() : 255;
        emit set_openauto_state((index == 0) ? alpha : 0);
    });

    return widget;
}

QWidget *MainWindow::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *tab_spacer = new QWidget(this);
    tab_spacer->setFixedWidth(this->TAB_SIZE.width() * this->config->get_scale());
    connect(this->config, &Config::scale_changed,
            [tab_spacer, width = this->TAB_SIZE.width()](double scale) { tab_spacer->setFixedWidth(width * scale); });

    QPushButton *save_button = new QPushButton(widget);
    save_button->setFlat(true);
    save_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("save", save_button);
    connect(save_button, &QPushButton::clicked, [config = this->config]() { config->save(); });

    QPushButton *shutdown_button = new QPushButton(widget);
    shutdown_button->setFlat(true);
    shutdown_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("power_settings_new", shutdown_button);

    QPushButton *exit_button = new QPushButton(widget);
    exit_button->setFlat(true);
    exit_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("close", exit_button);
    connect(exit_button, &QPushButton::clicked, []() { qApp->exit(); });

    QElapsedTimer *timer = new QElapsedTimer();
    connect(shutdown_button, &QPushButton::pressed, [timer]() { timer->start(); });
    connect(shutdown_button, &QPushButton::released, [timer]() {
        sync();

        std::stringstream cmd;
        cmd << "shutdown -" << (timer->hasExpired(2000) ? 'r' : 'h') << " now";
        if (system(cmd.str().c_str()) < 0) qApp->exit();
    });

    QWidget *quick_view = this->quick_view_widget();
    quick_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    layout->addWidget(tab_spacer);
    layout->addWidget(quick_view);
    layout->addStretch();
    layout->addWidget(save_button);
    layout->addWidget(shutdown_button);
    layout->addWidget(exit_button);

    return widget;
}

QWidget *MainWindow::quick_view_widget()
{
    QWidget *widget = new QWidget(this);
    QStackedLayout *layout = new QStackedLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    for (auto quick_view : this->config->get_quick_views().values()) layout->addWidget(quick_view);
    layout->setCurrentWidget(this->config->get_quick_view(this->config->get_quick_view()));
    connect(this->config, &Config::quick_view_changed, [this, layout](QString quick_view) {
        layout->setCurrentWidget(this->config->get_quick_view(quick_view));
    });

    return widget;
}

QWidget *MainWindow::volume_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setRange(0, 100);
    slider->setSliderPosition(this->config->get_volume());
    update_system_volume(slider->sliderPosition());
    connect(slider, &QSlider::valueChanged, [config = this->config](int position) {
        config->set_volume(position);
        MainWindow::update_system_volume(position);
    });

    QPushButton *lower_button = new QPushButton(widget);
    lower_button->setFlat(true);
    lower_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("volume_down", lower_button);
    connect(lower_button, &QPushButton::clicked, [slider]() {
        int position = slider->sliderPosition() - 10;
        slider->setSliderPosition(position);
    });

    QPushButton *raise_button = new QPushButton(widget);
    raise_button->setFlat(true);
    raise_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("volume_up", raise_button);
    connect(raise_button, &QPushButton::clicked, [slider]() {
        int position = slider->sliderPosition() + 10;
        slider->setSliderPosition(position);
    });

    layout->addWidget(lower_button);
    layout->addWidget(slider, 4);
    layout->addWidget(raise_button);

    return widget;
}

void MainWindow::update_system_volume(int position)
{
    QProcess *lProc = new QProcess();
    std::string command = "amixer set Master " + std::to_string(position) + "% --quiet";
    lProc->start(QString(command.c_str()));
    lProc->waitForFinished();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    emit is_ready();
}
