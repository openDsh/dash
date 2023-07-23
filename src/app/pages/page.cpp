#include <QHBoxLayout>

#include "app/arbiter.hpp"

#include "app/pages/page.hpp"

PageContainer::PageContainer(QWidget *widget)
    : QFrame(nullptr)
    , widget(widget)
{
    (new QStackedLayout(this))->addWidget(this->widget);
}

QWidget *PageContainer::take()
{
    this->layout()->removeWidget(this->widget);
    return this->widget;
}

void PageContainer::reset()
{
    this->layout()->addWidget(this->widget);
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

Page::Page(Arbiter &arbiter, QString name, QString icon_name, bool toggleable, QWidget *widget)
    : arbiter(arbiter)
    , name_(name)
    , icon_name_(icon_name)
    , toggleable_(toggleable)
    , container_(new PageContainer(widget))
    , button_(new QPushButton())
    , enabled_(true)
{
}

void Page::enable(bool enable)
{
    if (this->toggleable_)
        this->enabled_ = enable;
}

QPushButton *Page::settings_button()
{
    auto settings_button = new QPushButton();
    settings_button->setFlat(true);
    this->arbiter.forge().iconize("settings", settings_button, 24);

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
    auto dialog = new Dialog(this->arbiter, true, this->container_);
    dialog->set_body(this->settings_body());
    auto save_button = new QPushButton("save");
    QObject::connect(save_button, &QPushButton::clicked, [this]{ this->on_settings_save(); });
    dialog->set_button(save_button);

    return dialog;
}
