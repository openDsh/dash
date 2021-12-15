#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>

#include "app/quick_views/quick_view.hpp"
#include "app/utilities/icon_engine.hpp"
#include "app/widgets/dialog.hpp"

#include "app/window.hpp"

Dash::NavRail::NavRail()
    : group()
    , layout(new QVBoxLayout())
{
    this->layout->setContentsMargins(0, 0, 0, 0);
    this->layout->setSpacing(0);
}

Dash::Body::Body()
    : layout(new QVBoxLayout())
    , frame(new QStackedLayout())
{
    this->layout->setContentsMargins(0, 0, 0, 0);
    this->layout->setSpacing(0);

    this->frame->setContentsMargins(0, 0, 0, 0);
    this->layout->addLayout(this->frame, 1);
}

Dash::Dash(Arbiter &arbiter)
    : QWidget()
    , arbiter(arbiter)
    , rail()
    , body()
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addLayout(this->rail.layout);
    layout->addLayout(this->body.layout);

    connect(&this->rail.group, QOverload<int>::of(&QButtonGroup::buttonPressed), [this](int id){
        this->arbiter.set_curr_page(id);
    });
    connect(&this->arbiter, &Arbiter::curr_page_changed, [this](Page *page){
        this->set_page(page);
    });
    connect(&this->arbiter, &Arbiter::page_changed, [this](Page *page, bool enabled){
        int id = this->arbiter.layout().page_id(page);
        this->rail.group.button(id)->setVisible(enabled);

        if ((this->arbiter.layout().curr_page == page) && !enabled)
            this->arbiter.set_curr_page(this->arbiter.layout().next_enabled_page(page));
    });
}

void Dash::init()
{
    auto msg_ref = new QWidget();
    msg_ref->setObjectName("MsgRef");
    this->body.layout->addWidget(msg_ref);

    this->rail.layout->addWidget(this->main_menu());
    this->body.layout->addWidget(this->control_bar());

    for (auto page : this->arbiter.layout().pages()) {
        auto button = page->button();
        button->setCheckable(true);
        button->setFlat(true);
        QIcon icon(new StylizedIconEngine(this->arbiter, QString(":/icons/%1.svg").arg(page->icon_name()), true));
        this->arbiter.forge().iconize(icon, button, 32);

        this->rail.group.addButton(button, this->arbiter.layout().page_id(page));
        this->rail.layout->addWidget(button);
        this->body.frame->addWidget(page->widget());

        page->init();
        button->setVisible(page->enabled());
    }
    this->set_page(this->arbiter.layout().curr_page);
}

void Dash::set_page(Page *page)
{
    bool fullscreen = this->arbiter.layout().fullscreen_mode;
    int fullscreen_delay = this->arbiter.layout().fullscreen_delay;
    if (fullscreen && fullscreen_delay != 0) {
        this->arbiter.toggle_fullscreen_mode();
        this->arbiter.toggle_fullscreen_mode();
    }
    auto id = this->arbiter.layout().page_id(page);
    this->rail.group.button(id)->setChecked(true);
    this->body.frame->setCurrentWidget(page->widget());
}

QWidget *Dash::main_menu()
{
    auto widget = new QWidget();
    widget->setObjectName("MainMenu");
    auto layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    for (auto page : this->arbiter.layout().pages()) {
        auto button = new QPushButton();
        button->setProperty("color_hint", true);
        button->setCheckable(true);
        button->setFlat(true);
        this->arbiter.forge().iconize(page->icon_name(), button, 32, true);

        this->rail.group.addButton(button, this->arbiter.layout().page_id(page));
        layout->addWidget(button);
        this->body.frame->addWidget(page->widget());

        page->init();
        button->setVisible(page->enabled());
    }
    this->menu_width = widget->width();
    connect(&this->arbiter, &Arbiter::fullscreen_mode_changed, [widget, this](bool enabled)
        {
        if (!enabled) {
            widget->setMaximumWidth(this->menu_width);
        } else {
            if(!this->arbiter.layout().fullscreen_acknowledged) {
                QTimer::singleShot(0, [=]() {
                    this->fullscreen_hint_dialog()->exec();
                    this->arbiter.set_fullscreen_acknowledged(this->arbiter.layout().fullscreen_acknowledged);
                    this->parentWidget()->grabKeyboard();
                });
            }
            QPropertyAnimation *animation = new QPropertyAnimation(widget, "maximumWidth");
            animation->setDuration(this->arbiter.layout().fullscreen_delay);
            animation->setStartValue(this->menu_width);
            animation->setEndValue(0);
            animation->start();
        }
    });

    return widget;
}

Dialog *Dash::fullscreen_hint_dialog()
{
    QWidget *body = new QWidget();
    auto layout = new QVBoxLayout(body);
    layout->setContentsMargins(10, 10, 10, 10);
    // layout->setSpacing(0);

    // layout->setStretch(1, 3);
    QLabel *hint1 = new QLabel("Exit by double clicking mouse");
    QLabel *hint2 = new QLabel("Or Settings->Layout->Fullscreen mode.");
    QCheckBox *remember = new QCheckBox("Do not show", this);
    remember->setChecked(this->arbiter.layout().fullscreen_acknowledged);
    connect(remember, &QCheckBox::toggled, [this](bool checked){
        this->arbiter.set_fullscreen_acknowledged(checked);
    });
    layout->addWidget(hint1, 1);
    layout->addWidget(hint2);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(remember, 1);

    QPushButton *fake_button = new QPushButton();
    fake_button->setVisible(false);

    auto *dialog = new Dialog(this->arbiter, true, this->window());
    dialog->set_title("Full screen mode enabled");
    dialog->set_body(body);
    dialog->set_button(fake_button);
    return dialog;
}

QWidget *Dash::control_bar()
{
    auto widget = new QWidget();
    widget->setObjectName("ControlBar");
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto quick_views = new QStackedLayout();
    quick_views->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(quick_views);
    for (auto quick_view : this->arbiter.layout().control_bar.quick_views()) {
        quick_views->addWidget(quick_view->widget());
        quick_view->init();
    }
    quick_views->setCurrentWidget(this->arbiter.layout().control_bar.curr_quick_view->widget());
    connect(&this->arbiter, &Arbiter::curr_quick_view_changed, [quick_views](QuickView *quick_view){
        quick_views->setCurrentWidget(quick_view->widget());
    });

    layout->addStretch();

    auto dialog = new Dialog(this->arbiter, true, this->arbiter.window());
    dialog->set_title("Power Off");
    dialog->set_body(this->power_control());
    auto shutdown = new QPushButton();
    shutdown->setFlat(true);
    this->arbiter.forge().iconize("power_settings_new", shutdown, 26);
    layout->addWidget(shutdown);
    connect(shutdown, &QPushButton::clicked, [dialog]{ dialog->open(); });

    auto exit = new QPushButton();
    exit->setFlat(true);
    this->arbiter.forge().iconize("close", exit, 26);
    layout->addWidget(exit);
    connect(exit, &QPushButton::clicked, []{ qApp->exit(); });

    widget->setVisible(this->arbiter.layout().control_bar.enabled);
    connect(&this->arbiter, &Arbiter::control_bar_changed, [widget](bool enabled){
        widget->setVisible(enabled);
    });
    this->control_bar_height = widget->height();
    connect(&this->arbiter, &Arbiter::fullscreen_mode_changed, [widget, this](bool enabled){
        // widget->setVisible(!enabled);
        if (!enabled) {
            widget->setMaximumHeight(this->control_bar_height);
        } else {
            QPropertyAnimation *animation = new QPropertyAnimation(widget, "maximumHeight");
            animation->setDuration(this->arbiter.layout().fullscreen_delay);
            animation->setStartValue(this->control_bar_height);
            animation->setEndValue(0);
            animation->start();
        }
    });

    return widget;
}

QWidget *Dash::power_control() const
{
    auto widget = new QWidget();
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto restart = new QPushButton();
    restart->setFlat(true);
    this->arbiter.forge().iconize("refresh", restart, 36);
    connect(restart, &QPushButton::clicked, [this]{
        this->arbiter.settings().sync();
        sync();
        system(Session::System::REBOOT_CMD);
    });
    layout->addWidget(restart);

    auto power_off = new QPushButton();
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

Window::Window()
    : QMainWindow()
    , arbiter(this)
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    auto stack = new QStackedWidget();
    this->setCentralWidget(stack);

    auto dash = new Dash(this->arbiter);
    stack->addWidget(dash);
    dash->init();
}

void Window::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    this->arbiter.update();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
    this->arbiter.layout().openauto_page->pass_key_event(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    QMainWindow::keyReleaseEvent(event);
    this->arbiter.layout().openauto_page->pass_key_event(event);
}

void Window::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton ){
        this->arbiter.toggle_fullscreen_mode();
    }
    QMainWindow::mouseDoubleClickEvent(event);
}
