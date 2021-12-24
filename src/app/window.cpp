#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>

#include "app/quick_views/quick_view.hpp"
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

    this->body.layout->addWidget(this->control_bar());

    for (auto page : this->arbiter.layout().pages()) {
        auto button = new QPushButton();
        button->setProperty("color_hint", true);
        button->setCheckable(true);
        button->setFlat(true);
        this->arbiter.forge().iconize(page->icon_name(), button, 32, true);

        this->rail.group.addButton(button, this->arbiter.layout().page_id(page));
        this->rail.layout->addWidget(button);
        this->body.frame->addWidget(page->widget());

        page->init();
        button->setVisible(page->enabled());
    }
    this->set_page(this->arbiter.layout().curr_page);
    this->init_connected_pages();
}

void Dash::init_connected_pages()
{
    if (Config::get_instance()->get_show_aa_connected())
    {
        OpenAutoPage *oaPage = this->arbiter.layout().openauto_page;
        QAbstractButton *oaButton = this->rail.group.button(this->arbiter.layout().page_id(oaPage));
        connect(oaPage, &OpenAutoPage::connected, this, [this, oaPage, oaButton](bool connected){
            QElapsedTimer timer;
            timer.start();
            if (connected)
                this->arbiter.forge().set_icon(oaPage->connected_icon_name(), oaButton, false);
            else 
                this->arbiter.forge().set_icon(oaPage->icon_name(), oaButton, true);
        });
    }
}

void Dash::set_page(Page *page)
{
    auto id = this->arbiter.layout().page_id(page);
    this->rail.group.button(id)->setChecked(true);
    this->body.frame->setCurrentWidget(page->widget());
}

QWidget *Dash::control_bar() const
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

    connect(this->arbiter.layout().openauto_page, &OpenAutoPage::toggle_fullscreen, [stack](QWidget *widget){
        stack->addWidget(widget);
        stack->setCurrentWidget(widget);
    });
}

void Window::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    this->arbiter.update();
}

// void Window::keyPressEvent(QKeyEvent *event)
// {
//     QMainWindow::keyPressEvent(event);
//     this->arbiter.layout().openauto_page->pass_key_event(event);
// }

// void Window::keyReleaseEvent(QKeyEvent *event)
// {
//     QMainWindow::keyReleaseEvent(event);
//     this->arbiter.layout().openauto_page->pass_key_event(event);
// }
