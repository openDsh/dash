#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>

#include "app/widgets/selector.hpp"
#include "app/theme.hpp"

Selector::Selector(QList<QString> options, QString current, QFont font, QWidget *parent, bool add_null) :
        QWidget(parent), options(options), font(font)
{
    if (this->options.size() == 0)
        this->setEnabled(false);
    if (add_null)
        this->options.insert(0, QString());
    this->current_idx = this->options.indexOf(current);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(this->selector());
}

QLayout *Selector::selector()
{
    QHBoxLayout *layout = new QHBoxLayout();

    this->label = new QLabel(this);
    this->label->setText(this->get_current());
    this->label->setAlignment(Qt::AlignCenter);
    this->label->setFont(this->font);

    QPushButton *left_button = new QPushButton();
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    left_button->setIcon(Theme::get_instance()->make_button_icon("arrow_left", left_button));
    connect(left_button, &QPushButton::clicked, [this]() {
        int count = this->options.size();
        this->current_idx = ((this->current_idx - 1) % count + count) % count;
        this->label->setText(this->get_current());
        emit item_changed(this->get_current());
    });

    QPushButton *right_button = new QPushButton();
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    right_button->setIcon(Theme::get_instance()->make_button_icon("arrow_right", right_button));
    connect(right_button, &QPushButton::clicked, [this]() {
        this->current_idx = (this->current_idx + 1) % this->options.size();
        this->label->setText(this->get_current());
        emit item_changed(this->get_current());
    });

    layout->addWidget(left_button);
    layout->addWidget(this->label, 4);
    layout->addWidget(right_button);

    return layout;
}
