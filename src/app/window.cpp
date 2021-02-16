#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>

#include "app/widgets/dialog.hpp"

#include "app/window.hpp"

DashWindow::NavRail::NavRail(QObject *parent)
    : layout(new QVBoxLayout())
    , group(new QButtonGroup(parent))
{
    this->layout->setContentsMargins(0, 0, 0, 0);
    this->layout->setSpacing(0);
}

DashWindow::Body::Body(Arbiter &arbiter)
    : layout(new QVBoxLayout())
    , frame(new QStackedLayout())
{
    this->layout->setContentsMargins(0, 0, 0, 0);
    this->layout->setSpacing(0);

    this->frame->setContentsMargins(0, 0, 0, 0);
    this->layout->addLayout(this->frame);

    auto msg_ref = new QWidget();
    msg_ref->setObjectName("MsgRef");
    this->layout->addWidget(msg_ref);

    this->layout->addWidget(DashWindow::Body::control_bar(arbiter));
}

QWidget *DashWindow::Body::power_control(Arbiter &arbiter)
{
    auto widget = new QWidget();
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto restart = new QPushButton();
    restart->setFlat(true);
    arbiter.forge().iconize("refresh", restart, 36);
    connect(restart, &QPushButton::clicked, [&arbiter]{
        arbiter.settings().sync();
        sync();
        system(Session::System::REBOOT_CMD);
    });
    layout->addWidget(restart);

    auto power_off = new QPushButton();
    power_off->setFlat(true);
    arbiter.forge().iconize("power_settings_new", power_off, 36);
    connect(power_off, &QPushButton::clicked, [&arbiter]{
        arbiter.settings().sync();
        sync();
        system(Session::System::SHUTDOWN_CMD);
    });
    layout->addWidget(power_off);

    return widget;
}

QWidget *DashWindow::Body::control_bar(Arbiter &arbiter)
{
    auto widget = new QWidget();
    widget->setObjectName("ControlBar");
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto quick_views = new QStackedLayout();
    quick_views->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(quick_views);
    layout->addStretch();
    for (auto quick_view : arbiter.layout().control_bar.quick_views()) {
        quick_views->addWidget(quick_view->widget());
        quick_view->init();
    }
    quick_views->setCurrentWidget(arbiter.layout().control_bar.curr_quick_view->widget());
    connect(&arbiter, &Arbiter::curr_quick_view_changed, [quick_views](QuickView *quick_view){
        quick_views->setCurrentWidget(quick_view->widget());
    });

    auto dialog = new Dialog(arbiter, true, arbiter.window());
    dialog->set_title("Power Off");
    dialog->set_body(DashWindow::Body::power_control(arbiter));
    auto shutdown = new QPushButton();
    shutdown->setFlat(true);
    arbiter.forge().iconize("power_settings_new", shutdown, 26);
    layout->addWidget(shutdown);
    connect(shutdown, &QPushButton::clicked, [dialog]{ dialog->open(); });

    auto exit = new QPushButton();
    exit->setFlat(true);
    arbiter.forge().iconize("close", exit, 26);
    layout->addWidget(exit);
    connect(exit, &QPushButton::clicked, []{ qApp->exit(); });

    widget->setVisible(arbiter.layout().control_bar.enabled);
    connect(&arbiter, &Arbiter::control_bar_changed, [widget](bool enabled){
        widget->setVisible(enabled);
    });

    return widget;
}

DashWindow::DashWindow()
    : QMainWindow()
    , arbiter(this)
    , rail(this)
    , body(this->arbiter)
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    auto app = new QWidget();
    auto layout = new QHBoxLayout(app);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto stack = new QStackedWidget(this);
    stack->addWidget(app);
    connect(this->arbiter.layout().openauto_page, &OpenAutoPage::toggle_fullscreen, [stack](QWidget *widget){
        stack->addWidget(widget);
        stack->setCurrentWidget(widget);
    });

    layout->addLayout(this->rail.layout);
    layout->addLayout(this->body.layout);
    this->add_pages();

    this->setCentralWidget(stack);

    connect(this->rail.group, QOverload<int>::of(&QButtonGroup::buttonPressed), [this](int id){
        this->arbiter.set_curr_page(this->arbiter.layout().page(id));
    });
    connect(&this->arbiter, &Arbiter::curr_page_changed, [this](Page *page){ this->set_page(page); });
    connect(&this->arbiter, &Arbiter::page_changed, [this](Page *page, bool enabled){
        int id = this->arbiter.layout().page_id(page);
        this->rail.group->button(id)->setVisible(enabled);

        if ((this->arbiter.layout().curr_page == page) && !enabled) {
            auto next_id = id;
            do {
                next_id = (next_id + 1) % this->arbiter.layout().pages().size();
            } while (!this->arbiter.layout().page(next_id)->enabled());
            this->arbiter.set_curr_page(this->arbiter.layout().page(next_id));
        }
    });
}

void DashWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    this->arbiter.update();
}

void DashWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
    this->arbiter.layout().openauto_page->pass_key_event(event);
}

void DashWindow::keyReleaseEvent(QKeyEvent *event)
{
    QMainWindow::keyReleaseEvent(event);
    this->arbiter.layout().openauto_page->pass_key_event(event);
}

void DashWindow::add_pages()
{
    for (auto page : this->arbiter.layout().pages()) {
        auto button = new QPushButton();
        button->setProperty("color_hint", true);
        button->setCheckable(true);
        button->setFlat(true);
        this->arbiter.forge().iconize(page->icon_name(), button, 32);

        this->rail.group->addButton(button, this->arbiter.layout().page_id(page));
        this->rail.layout->addWidget(button);
        this->body.frame->addWidget(page->widget());

        page->init();
        button->setVisible(page->enabled());
    }
    this->set_page(this->arbiter.layout().curr_page);
}

void DashWindow::set_page(Page *page)
{
    auto id = this->arbiter.layout().page_id(page);
    this->rail.group->button(id)->setChecked(true);
    this->body.frame->setCurrentWidget(page->widget());
}
