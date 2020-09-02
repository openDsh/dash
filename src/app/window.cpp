#include "app/window.hpp"

#include <math.h>

#include <QElapsedTimer>
#include <QtWidgets>
#include <cstdlib>
#include <sstream>
#include <iostream>

#include "app/modules/brightness.hpp"
#include "app/tabs/camera.hpp"
#include "app/tabs/data.hpp"
#include "app/tabs/launcher.hpp"
#include "app/tabs/media.hpp"
#include "app/tabs/settings.hpp"
#include "app/widgets/dialog.hpp"

DashWindow::DashWindow()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->config = Config::get_instance();
    this->theme = Theme::get_instance();
    this->shortcuts = Shortcuts::get_instance();

    this->init_config();
    this->init_theme();

    this->openauto = new OpenAutoTab(this);
    this->stack = new QStackedWidget(this);
    this->rail = new QVBoxLayout();
    this->rail_group = new QButtonGroup(this);
    this->pages = new QStackedLayout();
    this->bar = new QHBoxLayout();

    connect(this->rail_group, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
            [this](int id, bool) { this->pages->setCurrentIndex(id); });
    connect(this->openauto, &OpenAutoTab::toggle_fullscreen, [this](QWidget *widget) { this->add_widget(widget); });

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

void DashWindow::add_widget(QWidget *widget)
{
    this->stack->addWidget(widget);
    this->stack->setCurrentWidget(widget);
}

void DashWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    this->theme->update();
    this->shortcuts->initialize_shortcuts();
}

void DashWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
    this->openauto->pass_key_event(event);
}

void DashWindow::keyReleaseEvent(QKeyEvent *event)
{
    QMainWindow::keyReleaseEvent(event);
    this->openauto->pass_key_event(event);
}

void DashWindow::init_config()
{
    qApp->setOverrideCursor(this->config->get_mouse_active() ? Qt::ArrowCursor : Qt::BlankCursor);

    this->config->add_quick_view("volume", this->volume_widget());
    this->config->add_quick_view("brightness", BrightnessModule::control_widget(true, this));
    this->config->add_quick_view("controls", this->controls_widget());
    this->config->add_quick_view("none", new QFrame(this));

    this->config->add_brightness_module("mocked", new MockedBrightnessModule(this));
    this->config->add_brightness_module("x", new XBrightnessModule());
    this->config->add_brightness_module("rpi 7\"", new RpiBrightnessModule());

    BrightnessModule *module = this->config->get_brightness_module();
    module->set_brightness(this->config->get_brightness());
}

void DashWindow::init_theme()
{
    this->theme->set_mode(this->config->get_dark_mode());
    this->theme->set_color(this->config->get_color());
    this->theme->set_scale(this->config->get_scale());
}

void DashWindow::init_ui()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->rail->setContentsMargins(0, 0, 0, 0);
    this->rail->setSpacing(0);

    layout->addLayout(this->rail);
    layout->addLayout(this->body());

    this->add_pages();

    this->stack->addWidget(widget);
    this->setCentralWidget(this->stack);
}

QLayout *DashWindow::body()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->pages->setContentsMargins(0, 0, 0, 0);

    this->bar->setContentsMargins(0, 0, 0, 0);
    this->bar->addWidget(this->controls_bar());

    QWidget *msg_ref = new QWidget(this);
    msg_ref->setObjectName("msg_ref");

    layout->addLayout(this->pages);
    layout->addWidget(msg_ref);
    layout->addLayout(this->bar);

    return layout;
}

void DashWindow::add_pages()
{
    this->add_page("Android Auto", this->openauto, "android_auto");
    this->add_page("Media", new MediaTab(this), "play_circle_outline");
    this->add_page("Vehicle", new VehicleTab(this), "directions_car");
    this->add_page("Camera", new CameraTab(this), "camera");
    this->add_page("Launcher", new LauncherTab(this), "widgets");
    this->add_page("Settings", new SettingsTab(this), "tune");

    // toggle initial page
    for (QAbstractButton *button : this->rail_group->buttons()) {
        if (!button->isHidden()) {
            button->setChecked(true);
            break;
        }
    }

    Shortcut *shortcut = new Shortcut(this->config->get_shortcut("cycle_pages"), this);
    this->shortcuts->add_shortcut("cycle_pages", "Cycle Pages", shortcut);
    connect(shortcut, &Shortcut::activated, [this]() {
        int idx = this->rail_group->checkedId();
        QList<QAbstractButton *> buttons = this->rail_group->buttons();
        do {
            idx = (idx + 1) % buttons.size();
        } while (buttons[idx]->isHidden());
        buttons[idx]->setChecked(true);
    });
}

void DashWindow::add_page(QString name, QWidget *page, QString icon)
{
    page->setObjectName(name);

    QPushButton *button = new QPushButton();
    button->setProperty("page", QVariant::fromValue(page));
    button->setCheckable(true);
    button->setFlat(true);
    button->setIconSize(Theme::icon_32);
    button->setIcon(this->theme->make_button_icon(icon, button));

    Shortcut *shortcut = new Shortcut(this->config->get_shortcut(name), this);
    this->shortcuts->add_shortcut(name, name, shortcut);
    connect(shortcut, &Shortcut::activated, [this, button]() {
        if (!button->isHidden())
            button->setChecked(true);
    });

    int idx = this->pages->addWidget(page);
    this->rail_group->addButton(button, idx);
    this->rail->addWidget(button);

    button->setVisible(this->config->get_page(page));
}

QWidget *DashWindow::controls_bar()
{
    QWidget *widget = new QWidget(this);
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *shutdown_button = new QPushButton(widget);
    shutdown_button->setFlat(true);
    shutdown_button->setIconSize(Theme::icon_26);
    shutdown_button->setIcon(this->theme->make_button_icon("power_settings_new", shutdown_button));
    connect(shutdown_button, &QPushButton::clicked, [this]() {
        Dialog *dialog = new Dialog(true, this);
        dialog->set_title("power off");
        dialog->set_body(this->power_control());

        dialog->open();
    });

    QPushButton *exit_button = new QPushButton(widget);
    exit_button->setFlat(true);
    exit_button->setIconSize(Theme::icon_26);
    exit_button->setIcon(this->theme->make_button_icon("close", exit_button));
    connect(exit_button, &QPushButton::clicked, [this]() {
        this->config->save();
        qApp->exit();
    });

    layout->addLayout(this->quick_views());
    layout->addStretch();
    layout->addWidget(shutdown_button);
    layout->addWidget(exit_button);

    widget->setVisible(this->config->get_controls_bar());
    connect(this->config, &Config::controls_bar_changed, [widget](bool controls_bar) { widget->setVisible(controls_bar); });

    return widget;
}

QLayout *DashWindow::quick_views()
{
    QStackedLayout *layout = new QStackedLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    for (auto quick_view : this->config->get_quick_views().values())
        layout->addWidget(quick_view);
    layout->setCurrentWidget(this->config->get_quick_view(this->config->get_quick_view()));
    connect(this->config, &Config::quick_view_changed,
            [this, layout](QString quick_view) { layout->setCurrentWidget(this->config->get_quick_view(quick_view)); });

    return layout;
}

QWidget *DashWindow::volume_widget(bool skip_buttons)
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
    connect(this->config, &Config::volume_changed, [slider](int volume) { slider->setValue(volume); });

    if (!skip_buttons) {
        QPushButton *lower_button = new QPushButton(widget);
        lower_button->setFlat(true);
        lower_button->setIconSize(Theme::icon_26);
        lower_button->setIcon(this->theme->make_button_icon("volume_down", lower_button));
        connect(lower_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 10); });

        QPushButton *raise_button = new QPushButton(widget);
        raise_button->setFlat(true);
        raise_button->setIconSize(Theme::icon_26);
        raise_button->setIcon(this->theme->make_button_icon("volume_up", raise_button));
        connect(raise_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 10); });

        layout->addWidget(lower_button);
        layout->addWidget(raise_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

QWidget *DashWindow::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *volume = new QPushButton(widget);
    volume->setFlat(true);
    volume->setIconSize(Theme::icon_26);
    volume->setIcon(this->theme->make_button_icon("volume_up", volume));
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
    brightness->setIcon(this->theme->make_button_icon("brightness_high", brightness));
    connect(brightness, &QPushButton::clicked, [this, brightness]() {
        Dialog *dialog = new Dialog(false, brightness);
        dialog->set_body(BrightnessModule::control_widget(false, this));
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
    dark_mode->setIcon(this->theme->make_button_icon("dark_mode", dark_mode));
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

QWidget *DashWindow::power_control()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *restart = new QPushButton(widget);
    restart->setFlat(true);
    restart->setIconSize(Theme::icon_42);
    restart->setIcon(this->theme->make_button_icon("refresh", restart));
    connect(restart, &QPushButton::clicked, [config = this->config]() {
        config->save();
        sync();
        if (system("shutdown -r now") < 0)
            qApp->exit();
    });
    layout->addWidget(restart);

    QPushButton *power_off = new QPushButton(widget);
    power_off->setFlat(true);
    power_off->setIconSize(Theme::icon_42);
    power_off->setIcon(this->theme->make_button_icon("power_settings_new", power_off));
    connect(power_off, &QPushButton::clicked, [config = this->config]() {
        config->save();
        sync();
        if (system("shutdown -h now") < 0)
            qApp->exit();
    });
    layout->addWidget(power_off);

    return widget;
}

void DashWindow::update_system_volume(int position)
{
    QProcess *lProc = new QProcess();
    std::string command = "";
    lProc->start(QString(command.c_str()));
    lProc->waitForFinished();
}
