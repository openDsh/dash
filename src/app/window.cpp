#include "app/window.hpp"

#include <math.h>

#include <QElapsedTimer>
#include <QtWidgets>
#include <cstdlib>
#include <sstream>

#include "app/widgets/sliders.hpp"
#include "app/pages/camera.hpp"
#include "app/pages/vehicle.hpp"
#include "app/pages/launcher.hpp"
#include "app/pages/media.hpp"
#include "app/pages/settings.hpp"
#include "app/widgets/dialog.hpp"

DashWindow::DashWindow()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->config = Config::get_instance();
    this->theme = Theme::get_instance();
    this->shortcuts = Shortcuts::get_instance();

    this->init_theme();
    this->init_config();

    this->openauto = new OpenAutoPage(this);
    this->stack = new QStackedWidget(this);
    this->rail = new QVBoxLayout();
    this->rail_group = new QButtonGroup(this);
    this->pages = new QStackedLayout();
    this->bar = new QHBoxLayout();

    connect(this->rail_group, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
            [this](int id, bool) { this->pages->setCurrentIndex(id); });
    connect(this->openauto, &OpenAutoPage::toggle_fullscreen, [this](QWidget *widget) { this->add_widget(widget); });

    connect(this->config, &Config::scale_changed, [theme = this->theme](double scale) { theme->set_scale(scale); });
    connect(this->config, &Config::page_changed,
            [rail_group = this->rail_group, pages = this->pages](QWidget *page, bool enabled) {
                QAbstractButton *button = rail_group->button(pages->indexOf(page));
                button->setVisible(enabled);
            });

    this->init_ui();
    this->init_shortcuts();
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

    this->config->add_quick_view("volume", volume_slider(false, this));
    this->config->add_quick_view("brightness", brightness_slider(true, this));
    this->config->add_quick_view("controls", this->controls_widget());
    this->config->add_quick_view("none", new QFrame(this));
}

void DashWindow::init_theme()
{
    this->theme->set_mode(this->config->get_dark_mode());
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

void DashWindow::init_shortcuts()
{
    Shortcut *brightness_down = new Shortcut(this->config->get_shortcut("brightness_down"), this);
    this->shortcuts->add_shortcut("brightness_down", "Decrease Brightness", brightness_down);
    connect(brightness_down, &Shortcut::activated, [config = this->config]() { config->set_brightness(config->get_brightness() - 4); });
    Shortcut *brightness_up = new Shortcut(this->config->get_shortcut("brightness_up"), this);
    this->shortcuts->add_shortcut("brightness_up", "Increase Brightness", brightness_up);
    connect(brightness_up, &Shortcut::activated, [config = this->config]() { config->set_brightness(config->get_brightness() + 4); });

    Shortcut *volume_down = new Shortcut(this->config->get_shortcut("volume_down"), this);
    this->shortcuts->add_shortcut("volume_down", "Decrease Volume", volume_down);
    connect(volume_down, &Shortcut::activated, [config = this->config]() { config->set_volume(config->get_volume() - 2); });
    Shortcut *volume_up = new Shortcut(this->config->get_shortcut("volume_up"), this);
    this->shortcuts->add_shortcut("volume_up", "Increase Volume", volume_up);
    connect(volume_up, &Shortcut::activated, [config = this->config]() { config->set_volume(config->get_volume() + 2); });
}

QLayout *DashWindow::body()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->pages->setContentsMargins(0, 0, 0, 0);

    this->bar->setContentsMargins(8, 0, 8, 0);
    this->bar->addWidget(this->controls_bar());

    QWidget *msg_ref = new QWidget(this);
    msg_ref->setObjectName("MsgRef");

    layout->addLayout(this->pages);
    layout->addWidget(msg_ref);
    layout->addLayout(this->bar);

    return layout;
}

void DashWindow::add_pages()
{
    this->add_page("Android Auto", this->openauto, "android_auto");
    this->add_page("Media", new MediaPage(this), "play_circle_outline");
    this->add_page("Vehicle", new VehiclePage(this), "directions_car");
    this->add_page("Camera", new CameraPage(this), "camera");
    this->add_page("Launcher", new LauncherPage(this), "widgets");
    this->add_page("Settings", new SettingsPage(this), "tune");

    // Toggle home page
    QString home_page = this->config->get_home_page();
    for (QAbstractButton *button : this->rail_group->buttons()) {
        if (!button->isHidden() && home_page == button->property("page").value<QWidget *>()->objectName()) {
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
    widget->setObjectName("ControlsBar");
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    Dialog *power_dialog = new Dialog(true, this);
    power_dialog->set_title("power off");
    power_dialog->set_body(this->power_control());

    QPushButton *shutdown_button = new QPushButton(widget);
    shutdown_button->setFlat(true);
    shutdown_button->setIconSize(Theme::icon_26);
    shutdown_button->setIcon(this->theme->make_button_icon("power_settings_new", shutdown_button));
    connect(shutdown_button, &QPushButton::clicked, [power_dialog]() { power_dialog->open(); });

    QPushButton *exit_button = new QPushButton(widget);
    exit_button->setFlat(true);
    exit_button->setIconSize(Theme::icon_26);
    exit_button->setIcon(this->theme->make_button_icon("close", exit_button));
    connect(exit_button, &QPushButton::clicked, []() { qApp->exit(); });

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

    Dialog *volume_dialog = new Dialog(false, volume);
    volume_dialog->set_body(volume_slider(true, this));

    QElapsedTimer *volume_timer = new QElapsedTimer();
    connect(volume, &QPushButton::pressed, [volume_timer]() { volume_timer->start(); });
    connect(volume, &QPushButton::released, [config = this->config, volume_timer, volume_dialog]() {
        if (volume_timer->hasExpired(1000))
            config->set_volume(0);
        else
            volume_dialog->open(2000);
    });
    QLabel *volume_value = new QLabel(QString::number(this->config->get_volume()), widget);
    volume_value->setFont(Theme::font_10);
    connect(this->config, &Config::volume_changed,
            [volume_value](int volume) { volume_value->setText(QString::number(volume)); });

    QPushButton *brightness = new QPushButton(widget);
    brightness->setFlat(true);
    brightness->setIconSize(Theme::icon_26);
    brightness->setIcon(this->theme->make_button_icon("brightness_high", brightness));

    Dialog *brightness_dialog = new Dialog(false, brightness);
    brightness_dialog->set_body(brightness_slider(false, this));

    connect(brightness, &QPushButton::clicked, [brightness_dialog]() {
        brightness_dialog->open(2000);
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
        this->config->set_dark_mode(mode);
        this->theme->set_mode(mode);
        this->theme->update();
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
        system("sudo shutdown -r now");
    });
    layout->addWidget(restart);

    QPushButton *power_off = new QPushButton(widget);
    power_off->setFlat(true);
    power_off->setIconSize(Theme::icon_42);
    power_off->setIcon(this->theme->make_button_icon("power_settings_new", power_off));
    connect(power_off, &QPushButton::clicked, [config = this->config]() {
        config->save();
        sync();
        system("sudo shutdown -h now");
    });
    layout->addWidget(power_off);

    return widget;
}
