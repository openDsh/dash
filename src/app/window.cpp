#include <QElapsedTimer>
#include <QtWidgets>
#include <cstdlib>
#include <math.h>
#include <sstream>

#include "app/modules/brightness.hpp"
#include "app/tabs/camera.hpp"
#include "app/tabs/data.hpp"
#include "app/tabs/launcher.hpp"
#include "app/tabs/media.hpp"
#include "app/tabs/settings.hpp"
#include "app/widgets/dialog.hpp"
#include "app/window.hpp"

MainWindow::MainWindow()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->config = Config::get_instance();
    qApp->setOverrideCursor(this->config->get_mouse_active() ? Qt::ArrowCursor : Qt::BlankCursor);

    this->theme = Theme::get_instance();
    this->theme->set_mode(this->config->get_dark_mode());
    this->theme->set_color(this->config->get_color());
    this->theme->set_scale(this->config->get_scale());

    connect(this->config, &Config::scale_changed, [theme = this->theme](double scale) { theme->set_scale(scale); });

    this->shortcuts = Shortcuts::get_instance();

    this->config->add_quick_view("volume", this->volume_widget());
    this->config->add_quick_view("brightness", this->brightness_widget());
    this->config->add_quick_view("controls", this->controls_widget());
    this->config->add_quick_view("none", new QFrame(this));

    this->config->add_brightness_module("mocked", new MockedBrightnessModule(this));
    this->config->add_brightness_module("x", new XBrightnessModule());
    this->config->add_brightness_module("rpi 7\"", new RpiBrightnessModule());

    BrightnessModule *module = this->config->get_brightness_module();
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
    QWidget *controls = this->controls_bar_widget();
    if (!this->config->get_controls_bar()) controls->hide();
    connect(this->config, &Config::controls_bar_changed,
            [controls](bool controls_bar) { controls_bar ? controls->show() : controls->hide(); });
    layout->addWidget(controls);

    return widget;
}

QTabWidget *MainWindow::tabs_widget()
{
    QTabWidget *widget = new QTabWidget(this);
    widget->setTabPosition(QTabWidget::TabPosition::West);
    widget->setIconSize(this->TAB_SIZE);

    Shortcut *cycle_pages_shortcut = new Shortcut(this->config->get_shortcut("cycle_pages"), this);
    this->shortcuts->add_shortcut("cycle_pages", "Cycle Pages", cycle_pages_shortcut);
    connect(cycle_pages_shortcut, &Shortcut::activated, [widget]() {
        int idx = widget->currentIndex();
        do {
            idx = (idx + 1) % widget->count();
        } while (!widget->isTabEnabled(idx));
        widget->setCurrentIndex(idx);
    });

    this->openauto = new OpenAutoTab(this);
    this->openauto->setObjectName("Android Auto");
    Shortcut *openauto_shortcut = new Shortcut(this->config->get_shortcut("android_auto_page"), this);
    this->shortcuts->add_shortcut("android_auto_page", "Open Android Auto Page", openauto_shortcut);
    connect(openauto_shortcut, &Shortcut::activated, [widget, openauto = this->openauto]() {
        int idx = widget->indexOf(openauto);
        if (widget->isTabEnabled(idx)) widget->setCurrentIndex(idx);
    });

    MediaTab *media = new MediaTab(this);
    media->setObjectName("Media");
    Shortcut *media_shortcut = new Shortcut(this->config->get_shortcut("media_page"), this);
    this->shortcuts->add_shortcut("media_page", "Open Media Page", media_shortcut);
    connect(media_shortcut, &Shortcut::activated, [widget, media]() {
        int idx = widget->indexOf(media);
        if (widget->isTabEnabled(idx)) widget->setCurrentIndex(idx);
    });

    DataTab *data = new DataTab(this);
    data->setObjectName("Data");
    Shortcut *data_shortcut = new Shortcut(this->config->get_shortcut("data_page"), this);
    this->shortcuts->add_shortcut("data_page", "Open Data Page", data_shortcut);
    connect(data_shortcut, &Shortcut::activated, [widget, data]() {
        int idx = widget->indexOf(data);
        if (widget->isTabEnabled(idx)) widget->setCurrentIndex(idx);
    });

    CameraTab *camera = new CameraTab(this);
    camera->setObjectName("Camera");
    Shortcut *camera_shortcut = new Shortcut(this->config->get_shortcut("camera_page"), this);
    this->shortcuts->add_shortcut("camera_page", "Open Camera Page", camera_shortcut);
    connect(camera_shortcut, &Shortcut::activated, [widget, camera]() {
        int idx = widget->indexOf(camera);
        if (widget->isTabEnabled(idx)) widget->setCurrentIndex(idx);
    });

    LauncherTab *launcher = new LauncherTab(this);
    launcher->setObjectName("Launcher");
    Shortcut *launcher_shortcut = new Shortcut(this->config->get_shortcut("launcher_page"), this);
    this->shortcuts->add_shortcut("launcher_page", "Open Launcher Page", launcher_shortcut);
    connect(launcher_shortcut, &Shortcut::activated, [widget, launcher]() {
        int idx = widget->indexOf(launcher);
        if (widget->isTabEnabled(idx)) widget->setCurrentIndex(idx);
    });

    SettingsTab *settings = new SettingsTab(this);
    settings->setProperty("prevent_disable", true);
    Shortcut *settings_shortcut = new Shortcut(this->config->get_shortcut("settings_page"), this);
    this->shortcuts->add_shortcut("settings_page", "Open Settings Page", settings_shortcut);
    connect(settings_shortcut, &Shortcut::activated, [widget, settings]() {
        widget->setCurrentIndex(widget->indexOf(settings));
    });

    int idx;
    idx = widget->addTab(openauto, QString());
    this->theme->add_tab_icon("android_auto", openauto, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(openauto));
    idx = widget->addTab(media, QString());
    this->theme->add_tab_icon("play_circle_outline", media, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(media));
    idx = widget->addTab(data, QString());
    this->theme->add_tab_icon("speed", data, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(data));
    idx = widget->addTab(camera, QString());
    this->theme->add_tab_icon("linked_camera", camera, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(camera));
    idx = widget->addTab(launcher, QString());
    this->theme->add_tab_icon("widgets", launcher, Qt::Orientation::Vertical);
    widget->setTabEnabled(idx, this->config->get_page(launcher));
    idx = widget->addTab(settings, QString());
    this->theme->add_tab_icon("tune", settings, Qt::Orientation::Vertical);

    media->fill_tabs();
    settings->fill_tabs();

    connect(this->config, &Config::brightness_changed, [this, widget](int position) {
        BrightnessModule *module = this->config->get_brightness_module();
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
        BrightnessModule *module = this->config->get_brightness_module();
        int alpha = module->update_androidauto() ? this->config->get_brightness() : 255;
        emit set_openauto_state((index == 0) ? alpha : 0);
    });

    return widget;
}

QWidget *MainWindow::controls_bar_widget()
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
    this->theme->add_button_icon("save_alt", save_button);
    connect(save_button, &QPushButton::clicked, [this, save_button]() {
        Dialog *dialog = new Dialog(false, save_button);
        dialog->set_body(this->save_control_widget());

        this->config->save();
        dialog->open(1000);
    });

    QPushButton *shutdown_button = new QPushButton(widget);
    shutdown_button->setFlat(true);
    shutdown_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("power_settings_new", shutdown_button);
    connect(shutdown_button, &QPushButton::clicked, [this]() {
        Dialog *dialog = new Dialog(true, this);
        dialog->set_title("power off");
        dialog->set_body(this->power_control_widget());

        dialog->open();
    });

    QPushButton *exit_button = new QPushButton(widget);
    exit_button->setFlat(true);
    exit_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("close", exit_button);
    connect(exit_button, &QPushButton::clicked, []() { qApp->exit(); });

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
    connect(this->config, &Config::quick_view_changed,
            [this, layout](QString quick_view) { layout->setCurrentWidget(this->config->get_quick_view(quick_view)); });

    return widget;
}

QWidget *MainWindow::volume_widget(bool skip_buttons)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(0, 100);
    slider->setValue(this->config->get_volume());
    update_system_volume(slider->value());
    connect(slider, &QSlider::valueChanged, [config = this->config](int position) {
        config->set_volume(position);
        MainWindow::update_system_volume(position);
    });
    connect(this->config, &Config::volume_changed,
            [slider](int volume) { slider->setValue(volume); });

    if (!skip_buttons) {
        QPushButton *lower_button = new QPushButton(widget);
        lower_button->setFlat(true);
        lower_button->setIconSize(Theme::icon_32);
        this->theme->add_button_icon("volume_down", lower_button);
        connect(lower_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 10); });

        QPushButton *raise_button = new QPushButton(widget);
        raise_button->setFlat(true);
        raise_button->setIconSize(Theme::icon_32);
        this->theme->add_button_icon("volume_up", raise_button);
        connect(raise_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 10); });

        layout->addWidget(lower_button);
        layout->addWidget(raise_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

QWidget *MainWindow::brightness_widget(bool skip_buttons)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(76, 255);
    slider->setValue(this->config->get_brightness());
    connect(slider, &QSlider::valueChanged,
            [config = this->config](int position) { config->set_brightness(position); });
    connect(this->config, &Config::brightness_changed,
            [slider](int brightness) { slider->setValue(brightness); });

    if (!skip_buttons) {
        QPushButton *dim_button = new QPushButton(widget);
        dim_button->setFlat(true);
        dim_button->setIconSize(Theme::icon_32);
        this->theme->add_button_icon("brightness_low", dim_button);
        connect(dim_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 18); });

        QPushButton *brighten_button = new QPushButton(widget);
        brighten_button->setFlat(true);
        brighten_button->setIconSize(Theme::icon_32);
        this->theme->add_button_icon("brightness_high", brighten_button);
        connect(brighten_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 18); });

        layout->addWidget(dim_button);
        layout->addWidget(brighten_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

QWidget *MainWindow::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QPushButton *volume = new QPushButton(widget);
    volume->setFlat(true);
    volume->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("volume_up", volume);
    QElapsedTimer *volume_timer = new QElapsedTimer();
    connect(volume, &QPushButton::pressed, [volume_timer]() { volume_timer->start(); });
    connect(volume, &QPushButton::released, [this, volume, volume_timer]() {
        if (volume_timer->hasExpired(1000)) {
            config->set_volume(0);
        }
        else {
            Dialog *dialog = new Dialog(false, volume);
            dialog->set_body(this->volume_widget(true));
            dialog->open(2000);
        }
    });
    QLabel *volume_value = new QLabel(QString::number(this->config->get_volume()), widget);
    volume_value->setFont(Theme::font_12);
    connect(this->config, &Config::volume_changed,
            [volume_value](int volume) { volume_value->setText(QString::number(volume)); });

    QPushButton *brightness = new QPushButton(widget);
    brightness->setFlat(true);
    brightness->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("brightness_high", brightness);
    connect(brightness, &QPushButton::clicked, [this, brightness]() {
        Dialog *dialog = new Dialog(false, brightness);
        dialog->set_body(this->brightness_widget(true));
        dialog->open(2000);
    });
    QLabel *brightness_value = new QLabel(QString::number(std::ceil(this->config->get_brightness() / 2.55)), widget);
    brightness_value->setFont(Theme::font_12);
    connect(this->config, &Config::brightness_changed, [brightness_value](int brightness) {
        brightness_value->setText(QString::number(std::ceil(brightness / 2.55)));
    });

    QPushButton *dark_mode = new QPushButton(widget);
    dark_mode->setFlat(true);
    dark_mode->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("dark_mode", dark_mode);
    connect(dark_mode, &QPushButton::clicked, [this]() {
        bool mode = !theme->get_mode();
        this->theme->set_mode(mode);
        this->config->set_dark_mode(mode);
    });

    layout->addWidget(volume, 1);
    layout->addWidget(volume_value, 7);
    layout->addStretch();
    layout->addWidget(brightness, 1);
    layout->addWidget(brightness_value, 7);
    layout->addStretch();
    layout->addWidget(dark_mode, 1);

    return widget;
}

QWidget *MainWindow::power_control_widget()
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *restart = new QPushButton(widget);
    restart->setFlat(true);
    restart->setIconSize(Theme::icon_56);
    this->theme->add_button_icon("refresh", restart);
    connect(restart, &QPushButton::clicked, []() {
        sync();
        if (system("shutdown -r now") < 0) qApp->exit();
    });
    layout->addWidget(restart);

    QPushButton *power_off = new QPushButton(widget);
    power_off->setFlat(true);
    power_off->setIconSize(Theme::icon_56);
    this->theme->add_button_icon("power_settings_new", power_off);
    connect(power_off, &QPushButton::clicked, []() {
        sync();
        if (system("shutdown -h now") < 0) qApp->exit();
    });
    layout->addWidget(power_off);

    return widget;
}

QWidget *MainWindow::save_control_widget()
{
    QWidget *widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QStackedLayout *layout = new QStackedLayout(widget);

    QLabel *check = new QLabel("✓", widget);
    check->setFont(Theme::font_24);
    check->setAlignment(Qt::AlignCenter);

    ProgressIndicator *loader = new ProgressIndicator(widget);
    loader->scale(this->config->get_scale());
    connect(this->config, &Config::scale_changed, [loader](double scale) { loader->scale(scale); });
    connect(this->config, &Config::save_status, [loader, layout](bool status) {
        if (status) {
            loader->start_animation();
        }
        else {
            QTimer::singleShot(1000, [=]() {
                layout->setCurrentIndex(1);
                loader->stop_animation();
            });
        }
    });

    layout->addWidget(loader);
    layout->addWidget(check);

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
    this->theme->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
    if (this->openauto != nullptr) this->openauto->send_key_event(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    QMainWindow::keyReleaseEvent(event);
    if (this->openauto != nullptr) this->openauto->send_key_event(event);
}

// make rail class because its about to get more complicated

Window2::Window2()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->config = Config::get_instance();
    this->theme = Theme::get_instance();
    this->shortcuts = Shortcuts::get_instance();

    this->init_config();
    this->init_theme();

    this->rail = new QVBoxLayout();
    this->rail_group = new QButtonGroup();
    this->pages = new QStackedLayout();
    this->bar = new QHBoxLayout();

    connect(this->rail_group, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
            [pages = this->pages](int id, bool) { pages->setCurrentIndex(id); });
    connect(this->config, &Config::scale_changed, [theme = this->theme](double scale) { theme->set_scale(scale); });
    connect(this->config, &Config::page_changed,
            [rail_group = this->rail_group, pages = this->pages](QWidget *page, bool enabled) {
                QAbstractButton *button = rail_group->button(pages->indexOf(page));
                button->setVisible(enabled);
            });
    connect(this->config, &Config::brightness_changed, [config = this->config](int position) {
        BrightnessModule *module = config->get_brightness_module();
        module->set_brightness(position);
    });

    this->init_ui();
}

void Window2::init_config()
{
    qApp->setOverrideCursor(this->config->get_mouse_active() ? Qt::ArrowCursor : Qt::BlankCursor);

    this->config->add_quick_view("volume", this->volume_widget());
    this->config->add_quick_view("brightness", this->brightness_widget());
    this->config->add_quick_view("controls", this->controls_widget());
    this->config->add_quick_view("none", new QFrame(this));

    this->config->add_brightness_module("mocked", new MockedBrightnessModule(this));
    this->config->add_brightness_module("x", new XBrightnessModule());
    this->config->add_brightness_module("rpi 7\"", new RpiBrightnessModule());

    BrightnessModule *module = this->config->get_brightness_module();
    module->set_brightness(this->config->get_brightness());
}

void Window2::init_theme()
{
    this->theme->set_mode(this->config->get_dark_mode());
    this->theme->set_color(this->config->get_color());
    this->theme->set_scale(this->config->get_scale());
}

void Window2::init_ui()
{
    QFrame *widget = new QFrame(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->rail->setContentsMargins(0, 0, 0, 0);
    this->rail->setSpacing(0);

    layout->addLayout(this->rail);
    layout->addLayout(this->body());

    this->add_pages();

    this->setCentralWidget(widget);
}

QBoxLayout *Window2::body()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->pages->setContentsMargins(0, 0, 0, 0);

    this->bar->setContentsMargins(0, 0, 0, 0);
    this->bar->addWidget(this->controls_bar_widget());

    layout->addLayout(this->pages);
    layout->addLayout(this->bar);

    return layout;
}

void Window2::add_pages()
{
    this->add_page("Android Auto", new OpenAutoTab(this), "android_auto");
    this->add_page("Media", new MediaTab(this), "play_circle_outline");
    this->add_page("Data", new DataTab(this), "speed");
    this->add_page("Camera", new CameraTab(this), "linked_camera");
    this->add_page("Launcher", new LauncherTab(this), "widgets");
    this->add_page("Settings", new SettingsTab(this), "tune");

    // toggle initial page
    for (QAbstractButton *button : this->rail_group->buttons()) {
        if (!button->isHidden()) {
            button->setChecked(true);
            break;
        }
    }
}

void Window2::add_page(QString name, QWidget *page, QString icon)
{
    page->setObjectName(name);

    QPushButton *button = new QPushButton();
    button->setCheckable(true);
    button->setFlat(true);
    button->setIconSize(Theme::icon_32);
    button->setIcon(this->theme->add_button_icon2(icon, button));

    button->setProperty("page", QVariant::fromValue(page));
    button->setProperty("base_icon_size", QVariant::fromValue(button->iconSize()));

    int idx = this->pages->addWidget(page);
    this->rail_group->addButton(button, idx);
    this->rail->addWidget(button);

    button->setVisible(this->config->get_page(page));
}

QWidget *Window2::quick_view_widget()
{
    QWidget *widget = new QWidget(this);
    QStackedLayout *layout = new QStackedLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    for (auto quick_view : this->config->get_quick_views().values()) layout->addWidget(quick_view);
    layout->setCurrentWidget(this->config->get_quick_view(this->config->get_quick_view()));
    connect(this->config, &Config::quick_view_changed,
            [this, layout](QString quick_view) { layout->setCurrentWidget(this->config->get_quick_view(quick_view)); });

    return widget;
}

QWidget *Window2::controls_bar_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *save_button = new QPushButton(widget);
    save_button->setFlat(true);
    save_button->setIconSize(Theme::icon_26);
    save_button->setIcon(this->theme->add_button_icon2("save_alt", save_button));
    save_button->setProperty("base_icon_size", QVariant::fromValue(save_button->iconSize()));
    connect(save_button, &QPushButton::clicked, [this, save_button]() {
        Dialog *dialog = new Dialog(false, save_button);
        dialog->set_body(this->save_control_widget());

        this->config->save();
        dialog->open(1000);
    });

    QPushButton *shutdown_button = new QPushButton(widget);
    shutdown_button->setFlat(true);
    shutdown_button->setIconSize(Theme::icon_26);
    shutdown_button->setIcon(this->theme->add_button_icon2("power_settings_new", shutdown_button));
    shutdown_button->setProperty("base_icon_size", QVariant::fromValue(shutdown_button->iconSize()));
    connect(shutdown_button, &QPushButton::clicked, [this]() {
        Dialog *dialog = new Dialog(true, this);
        dialog->set_title("power off");
        dialog->set_body(this->power_control_widget());

        dialog->open();
    });

    QPushButton *exit_button = new QPushButton(widget);
    exit_button->setFlat(true);
    exit_button->setIconSize(Theme::icon_26);
    exit_button->setIcon(this->theme->add_button_icon2("close", exit_button));
    exit_button->setProperty("base_icon_size", QVariant::fromValue(exit_button->iconSize()));
    connect(exit_button, &QPushButton::clicked, []() { qApp->exit(); });

    QWidget *quick_view = this->quick_view_widget();
    quick_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    layout->addWidget(quick_view);
    layout->addStretch();
    layout->addWidget(save_button);
    layout->addWidget(shutdown_button);
    layout->addWidget(exit_button);

    if (!this->config->get_controls_bar()) widget->hide();
    connect(this->config, &Config::controls_bar_changed,
            [widget](bool controls_bar) { controls_bar ? widget->show() : widget->hide(); });

    return widget;
}

QWidget *Window2::volume_widget(bool skip_buttons)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(0, 100);
    slider->setValue(this->config->get_volume());
    update_system_volume(slider->value());
    connect(slider, &QSlider::valueChanged, [this](int position) {
        this->config->set_volume(position);
        this->update_system_volume(position);
    });
    connect(this->config, &Config::volume_changed,
            [slider](int volume) { slider->setValue(volume); });

    if (!skip_buttons) {
        QPushButton *lower_button = new QPushButton(widget);
        lower_button->setFlat(true);
        lower_button->setIconSize(Theme::icon_26);
        lower_button->setIcon(this->theme->add_button_icon2("volume_down", lower_button));
        lower_button->setProperty("base_icon_size", QVariant::fromValue(lower_button->iconSize()));
        connect(lower_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 10); });

        QPushButton *raise_button = new QPushButton(widget);
        raise_button->setFlat(true);
        raise_button->setIconSize(Theme::icon_26);
        this->theme->add_button_icon("volume_up", raise_button);
        raise_button->setIcon(this->theme->add_button_icon2("volume_up", raise_button));
        raise_button->setProperty("base_icon_size", QVariant::fromValue(raise_button->iconSize()));
        connect(raise_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 10); });

        layout->addWidget(lower_button);
        layout->addWidget(raise_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

QWidget *Window2::brightness_widget(bool skip_buttons)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(76, 255);
    slider->setValue(this->config->get_brightness());
    connect(slider, &QSlider::valueChanged,
            [config = this->config](int position) { config->set_brightness(position); });
    connect(this->config, &Config::brightness_changed,
            [slider](int brightness) { slider->setValue(brightness); });

    if (!skip_buttons) {
        QPushButton *dim_button = new QPushButton(widget);
        dim_button->setFlat(true);
        dim_button->setIconSize(Theme::icon_26);
        dim_button->setIcon(this->theme->add_button_icon2("brightness_low", dim_button));
        dim_button->setProperty("base_icon_size", QVariant::fromValue(dim_button->iconSize()));
        connect(dim_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 18); });

        QPushButton *brighten_button = new QPushButton(widget);
        brighten_button->setFlat(true);
        brighten_button->setIconSize(Theme::icon_26);
        brighten_button->setIcon(this->theme->add_button_icon2("brightness_high", brighten_button));
        brighten_button->setProperty("base_icon_size", QVariant::fromValue(brighten_button->iconSize()));
        connect(brighten_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 18); });

        layout->addWidget(dim_button);
        layout->addWidget(brighten_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

QWidget *Window2::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *volume = new QPushButton(widget);
    volume->setFlat(true);
    volume->setIconSize(Theme::icon_26);
    this->theme->add_button_icon("volume_up", volume);
    volume->setIcon(this->theme->add_button_icon2("volume_up", volume));
    volume->setProperty("base_icon_size", QVariant::fromValue(volume->iconSize()));
    QElapsedTimer *volume_timer = new QElapsedTimer();
    connect(volume, &QPushButton::pressed, [volume_timer]() { volume_timer->start(); });
    connect(volume, &QPushButton::released, [this, volume, volume_timer]() {
        if (volume_timer->hasExpired(1000)) {
            config->set_volume(0);
        }
        else {
            Dialog *dialog = new Dialog(false, volume);
            dialog->set_body(this->volume_widget(true));
            dialog->open(2000);
        }
    });
    QLabel *volume_value = new QLabel(QString::number(this->config->get_volume()), widget);
    volume_value->setFont(Theme::font_10);
    connect(this->config, &Config::volume_changed,
            [volume_value](int volume) { volume_value->setText(QString::number(volume)); });

    QPushButton *brightness = new QPushButton(widget);
    brightness->setFlat(true);
    brightness->setIconSize(Theme::icon_26);
    this->theme->add_button_icon("brightness_high", brightness);
    brightness->setIcon(this->theme->add_button_icon2("brightness_high", brightness));
    brightness->setProperty("base_icon_size", QVariant::fromValue(brightness->iconSize()));
    connect(brightness, &QPushButton::clicked, [this, brightness]() {
        Dialog *dialog = new Dialog(false, brightness);
        dialog->set_body(this->brightness_widget(true));
        dialog->open(2000);
    });
    QLabel *brightness_value = new QLabel(QString::number(std::ceil(this->config->get_brightness() / 2.55)), widget);
    brightness_value->setFont(Theme::font_10);
    connect(this->config, &Config::brightness_changed, [brightness_value](int brightness) {
        brightness_value->setText(QString::number(std::ceil(brightness / 2.55)));
    });

    QPushButton *dark_mode = new QPushButton(widget);
    dark_mode->setFlat(true);
    dark_mode->setIconSize(Theme::icon_26);
    this->theme->add_button_icon("dark_mode", dark_mode);
    dark_mode->setIcon(this->theme->add_button_icon2("dark_mode", dark_mode));
    dark_mode->setProperty("base_icon_size", QVariant::fromValue(dark_mode->iconSize()));
    connect(dark_mode, &QPushButton::clicked, [this]() {
        bool mode = !theme->get_mode();
        this->theme->set_mode(mode);
        this->config->set_dark_mode(mode);
    });

    layout->addWidget(volume, 1);
    layout->addWidget(volume_value, 7);
    layout->addStretch();
    layout->addWidget(brightness, 1);
    layout->addWidget(brightness_value, 7);
    layout->addStretch();
    layout->addWidget(dark_mode, 1);

    return widget;
}

QWidget *Window2::power_control_widget()
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *restart = new QPushButton(widget);
    restart->setFlat(true);
    restart->setIconSize(Theme::icon_42);
    this->theme->add_button_icon("refresh", restart);
    restart->setIcon(this->theme->add_button_icon2("refresh", restart));
    restart->setProperty("base_icon_size", QVariant::fromValue(restart->iconSize()));
    connect(restart, &QPushButton::clicked, []() {
        sync();
        if (system("shutdown -r now") < 0) qApp->exit();
    });
    layout->addWidget(restart);

    QPushButton *power_off = new QPushButton(widget);
    power_off->setFlat(true);
    power_off->setIconSize(Theme::icon_42);
    this->theme->add_button_icon("power_settings_new", power_off);
    power_off->setIcon(this->theme->add_button_icon2("power_settings_new", power_off));
    power_off->setProperty("base_icon_size", QVariant::fromValue(power_off->iconSize()));
    connect(power_off, &QPushButton::clicked, []() {
        sync();
        if (system("shutdown -h now") < 0) qApp->exit();
    });
    layout->addWidget(power_off);

    return widget;
}

QWidget *Window2::save_control_widget()
{
    QWidget *widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QStackedLayout *layout = new QStackedLayout(widget);

    QLabel *check = new QLabel("✓", widget);
    check->setFont(Theme::font_24);
    check->setAlignment(Qt::AlignCenter);

    ProgressIndicator *loader = new ProgressIndicator(widget);
    loader->scale(this->config->get_scale());
    connect(this->config, &Config::scale_changed, [loader](double scale) { loader->scale(scale); });
    connect(this->config, &Config::save_status, [loader, layout](bool status) {
        if (status) {
            loader->start_animation();
        }
        else {
            QTimer::singleShot(1000, [=]() {
                layout->setCurrentIndex(1);
                loader->stop_animation();
            });
        }
    });

    layout->addWidget(loader);
    layout->addWidget(check);

    return widget;
}

void Window2::update_system_volume(int position)
{
    QProcess *lProc = new QProcess();
    std::string command = "amixer set Master " + std::to_string(position) + "% --quiet";
    lProc->start(QString(command.c_str()));
    lProc->waitForFinished();
}

