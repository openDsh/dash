#include "app/window.hpp"

#include <math.h>

#include <QElapsedTimer>
#include <QtWidgets>
#include <cstdlib>
#include <sstream>

#include "app/pages/camera.hpp"
#include "app/pages/vehicle.hpp"
#include "app/pages/launcher.hpp"
#include "app/pages/media.hpp"
#include "app/pages/settings.hpp"
#include "app/widgets/dialog.hpp"

DashWindow::DashWindow()
    : QMainWindow()
    , arbiter()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->config = Config::get_instance();
    this->shortcuts = Shortcuts::get_instance();

    this->openauto = this->arbiter.layout().openauto_page;
    this->stack = new QStackedWidget(this);
    this->rail = new QVBoxLayout();
    this->rail_group = new QButtonGroup(this);
    this->pages = new QStackedLayout();
    this->bar = new QHBoxLayout();

    connect(this->rail_group, QOverload<int, bool>::of(&QButtonGroup::buttonToggled), [this](int id, bool){
        this->pages->setCurrentWidget(this->arbiter.layout().page(id)->widget());
    });
    connect(this->openauto, &OpenAutoPage::toggle_fullscreen, [this](QWidget *widget) { this->add_widget(widget); });

    this->init_ui();
    this->init_shortcuts();

    connect(&this->arbiter, &Arbiter::page_toggled, [this](Page *page){
        int id = this->arbiter.layout().page_id(page);
        this->rail_group->button(id)->setVisible(page->enabled());
    });
}

void DashWindow::add_widget(QWidget *widget)
{
    this->stack->addWidget(widget);
    this->stack->setCurrentWidget(widget);
}

void DashWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    this->arbiter.update();
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
    connect(brightness_down, &Shortcut::activated, [this]{
        auto brightness = std::min(std::max(76, this->arbiter.system().brightness.value - 4), 255);
        this->arbiter.set_brightness(brightness);
    });
    Shortcut *brightness_up = new Shortcut(this->config->get_shortcut("brightness_up"), this);
    this->shortcuts->add_shortcut("brightness_up", "Increase Brightness", brightness_up);
    connect(brightness_up, &Shortcut::activated, [this]{
        auto brightness = std::min(std::max(78, this->arbiter.system().brightness.value + 4), 255);
        this->arbiter.set_brightness(brightness);
    });

    Shortcut *volume_down = new Shortcut(this->config->get_shortcut("volume_down"), this);
    this->shortcuts->add_shortcut("volume_down", "Decrease Volume", volume_down);
    connect(volume_down, &Shortcut::activated, [this]{
        auto volume = std::min(std::max(0, this->arbiter.system().volume - 2), 100);
        this->arbiter.set_volume(volume);
    });
    Shortcut *volume_up = new Shortcut(this->config->get_shortcut("volume_up"), this);
    this->shortcuts->add_shortcut("volume_up", "Increase Volume", volume_up);
    connect(volume_up, &Shortcut::activated, [this]{
        auto volume = std::min(std::max(0, this->arbiter.system().volume + 2), 100);
        this->arbiter.set_volume(volume);
    });
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
    for (auto page : this->arbiter.layout().pages())
        this->add_page(page);

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

void DashWindow::add_page(Page *page)
{
    QPushButton *button = new QPushButton();
    button->setProperty("color_hint", true);
    button->setCheckable(true);
    button->setFlat(true);
    this->arbiter.forge().iconize(page->icon_name(), button, 32);

    auto name = page->pretty_name();
    Shortcut *shortcut = new Shortcut(this->config->get_shortcut(name), this);
    this->shortcuts->add_shortcut(name, name, shortcut);
    connect(shortcut, &Shortcut::activated, [this, button]{
        if (!button->isHidden())
            button->setChecked(true);
    });

    this->pages->addWidget(page->widget());
    page->init();
    this->rail_group->addButton(button, this->arbiter.layout().page_id(page));
    this->rail->addWidget(button);

    button->setVisible(page->enabled());
}

QWidget *DashWindow::controls_bar()
{
    QWidget *widget = new QWidget(this);
    widget->setObjectName("ControlBar");
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    Dialog *power_dialog = new Dialog(this->arbiter, true, this);
    power_dialog->set_title("power off");
    power_dialog->set_body(this->power_control());

    QPushButton *shutdown_button = new QPushButton(widget);
    shutdown_button->setFlat(true);
    this->arbiter.forge().iconize("power_settings_new", shutdown_button, 26);
    connect(shutdown_button, &QPushButton::clicked, [power_dialog]{ power_dialog->open(); });

    QPushButton *exit_button = new QPushButton(widget);
    exit_button->setFlat(true);
    this->arbiter.forge().iconize("close", exit_button, 26);
    connect(exit_button, &QPushButton::clicked, []{ qApp->exit(); });

    layout->addLayout(this->quick_views());
    layout->addStretch();
    layout->addWidget(shutdown_button);
    layout->addWidget(exit_button);

    widget->setVisible(this->arbiter.layout().control_bar.enabled);
    connect(&this->arbiter, &Arbiter::control_bar_toggled, [this, widget]{
        widget->setVisible(this->arbiter.layout().control_bar.enabled);
    });

    return widget;
}

QLayout *DashWindow::quick_views()
{
    QStackedLayout *layout = new QStackedLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    for (auto quick_view : this->arbiter.layout().control_bar.quick_views()) {
        layout->addWidget(quick_view->widget());
        quick_view->init();
    }
    layout->setCurrentWidget(this->arbiter.layout().control_bar.curr_quick_view->widget());
    connect(&this->arbiter, &Arbiter::curr_quick_view_changed, [layout](QuickView *quick_view){
        layout->setCurrentWidget(quick_view->widget());
    });

    return layout;
}

QWidget *DashWindow::power_control()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *restart = new QPushButton(widget);
    restart->setFlat(true);
    this->arbiter.forge().iconize("refresh", restart, 36);
    connect(restart, &QPushButton::clicked, [this]{
        this->arbiter.settings().sync();
        sync();
        system(Session::System::REBOOT_CMD);
    });
    layout->addWidget(restart);

    QPushButton *power_off = new QPushButton(widget);
    power_off->setFlat(true);
    this->arbiter.forge().iconize("power_settings_new", power_off, 36);
    connect(power_off, &QPushButton::clicked, [this]{
        this->arbiter.settings().sync();
        sync();
        system(Session::System::SHUTDOWN_CMD);
    });
    layout->addWidget(power_off);

    return widget;
}
