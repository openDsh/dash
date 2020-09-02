#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>

#include "app/widgets/selector.hpp"
#include "app/theme.hpp"

Selector::Selector(QList<QString> options, QFont font, QWidget *parent) : QWidget(parent), options(options)
{
    this->font = font;
    this->current_idx = 0;

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(this->selector());
}

QLayout *Selector::selector()
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel(this->options[this->current_idx]);
    label->setAlignment(Qt::AlignCenter);
    label->setFont(this->font);

    QPushButton *left_button = new QPushButton();
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    left_button->setIcon(Theme::get_instance()->make_button_icon("arrow_left", left_button));
    connect(left_button, &QPushButton::clicked, [this, label]() {
        int count = this->options.size();
        this->current_idx = ((this->current_idx - 1) % count + count) % count;
        label->setText(this->options[this->current_idx]);
    });

    QPushButton *right_button = new QPushButton();
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    right_button->setIcon(Theme::get_instance()->make_button_icon("arrow_right", right_button));
    connect(right_button, &QPushButton::clicked, [this, label]() {
        this->current_idx = (this->current_idx + 1) % this->options.size();
        label->setText(this->options[this->current_idx]);
    });

    layout->addWidget(left_button);
    layout->addWidget(label, 4);
    layout->addWidget(right_button);

    return layout;
}
