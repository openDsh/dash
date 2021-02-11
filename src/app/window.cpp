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
    , arbiter(this)
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->openauto = this->arbiter.layout().openauto_page;
    this->stack = new QStackedWidget(this);
    this->rail = new QVBoxLayout();
    this->rail_group = new QButtonGroup(this);
    this->pages = new QStackedLayout();
    this->bar = new QHBoxLayout();

    connect(this->rail_group, QOverload<int>::of(&QButtonGroup::buttonPressed), [this](int id){
        this->arbiter.set_curr_page(this->arbiter.layout().page(id));
    });
    connect(this->openauto, &OpenAutoPage::toggle_fullscreen, [this](QWidget *widget) { this->add_widget(widget); });

    this->init_ui();

    connect(&this->arbiter, &Arbiter::page_toggled, [this](Page *page){
        int id = this->arbiter.layout().page_id(page);
        this->rail_group->button(id)->setVisible(page->enabled());
    });
    connect(&this->arbiter, &Arbiter::curr_page_changed, [this](Page *page){ this->set_page(page); });
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

    this->set_page(this->arbiter.layout().curr_page);
}

void DashWindow::add_page(Page *page)
{
    QPushButton *button = new QPushButton();
    button->setProperty("color_hint", true);
    button->setCheckable(true);
    button->setFlat(true);
    this->arbiter.forge().iconize(page->icon_name(), button, 32);

    this->pages->addWidget(page->widget());
    page->init();
    this->rail_group->addButton(button, this->arbiter.layout().page_id(page));
    this->rail->addWidget(button);

    button->setVisible(page->enabled());
}

void DashWindow::set_page(Page *page)
{
    int id = this->arbiter.layout().page_id(page);
    this->rail_group->button(id)->setChecked(true);
    this->pages->setCurrentWidget(page->widget());
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
    power_dialog->set_title("Power Off");
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
