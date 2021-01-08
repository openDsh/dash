#include <QHBoxLayout>

#include "app/pages/page.hpp"

Page::Page(Arbiter &arbiter, QString pretty_name, QString icon_name, bool toggleable, QWidget *widget)
    : arbiter(arbiter)
    , pretty_name_(pretty_name)
    , icon_name_(icon_name)
    , toggleable_(toggleable)
    , widget_(widget)
    , enabled_(true)
{
}

void Page::toggle()
{
    if (this->toggleable_)
        this->enabled_ = !this->enabled_;
}

Page::Settings::Settings() : QWidget()
{
    this->layout_ = new QVBoxLayout(this);
}

void Page::Settings::add_row(QString label, QWidget *control)
{
    this->layout_->addLayout(Page::Settings::row(label, control));
}

QLayout *Page::Settings::row(QString label, QWidget *control)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(new QLabel(label), 1);
    layout->addWidget(control, 1, Qt::AlignHCenter);

    return layout;
}

QPushButton *Page::settings_button()
{
    auto settings_button = new QPushButton();
    settings_button->setFlat(true);
    settings_button->setIconSize(Theme::icon_24);
    auto icon = Theme::get_instance()->make_button_icon("settings", settings_button);
    settings_button->setIcon(icon);

    auto dialog = this->dialog();
    QObject::connect(settings_button, &QPushButton::clicked, [dialog]{ dialog->open(); });

    return settings_button;
}

QLayout *Page::settings_layout()
{
    auto layout = new QHBoxLayout();
    layout->addStretch();
    layout->addWidget(this->settings_button());

    return layout;
}

Dialog *Page::dialog()
{
    auto dialog = new Dialog(true, this->widget_);
    dialog->set_body(this->settings_body());
    auto save_button = new QPushButton("save");
    QObject::connect(save_button, &QPushButton::clicked, [this]{ this->on_settings_save(); });
    dialog->set_button(save_button);

    return dialog;
}

// BasedPage::BasedPage(QString pretty_name, QString icon_name, bool toggleable, bool enable_settings, QWidget *parent)
//     : QWidget(parent)
//     , Page(pretty_name, icon_name, toggleable, this)
// {
//     auto widget
//     auto layout = new QVBoxLayout(this);
//     layout->setContentsMargins(0, 0, 0, 0);
//     layout->setSpacing(0);

//     if (enable_settings)
//         layout->addLayout(this->settings_button_layout());
// }

// StackedPage::StackedPage(QString pretty_name, QString icon_name, bool toggleable, bool enable_settings, QWidget *parent)
//     : QStackedWidget(parent)
//     , Page(pretty_name, icon_name, toggleable, this)
// {
// }

// TabbedPage::TabbedPage(QString pretty_name, QString icon_name, bool toggleable, bool enable_settings, QWidget *parent)
//     : QTabWidget(parent)
//     , Page(pretty_name, icon_name, toggleable, this)
// {
//     if (enable_settings)
//         this->setCornerWidget(this->settings_button());
// }
