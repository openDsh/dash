#include <algorithm>

#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>

#include "app/widgets/selector.hpp"
#include "app/theme.hpp"

Selector::Selector(QList<QString> options, QString current, QFont font, QWidget *parent, QString placeholder) :
        QWidget(parent), options(options), placeholder(placeholder)
{
    this->label = new QLabel(this);
    this->label->setAlignment(Qt::AlignCenter);
    this->label->setFont(font);

    this->set_state();
    this->current_idx = std::max(0, this->options.indexOf(current));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(this->selector());
}

QSize Selector::sizeHint() const
{
    int width = (Theme::icon_32.width() * 2) + this->label->width() + (12 * 2);
    int height = std::max(Theme::icon_32.height(), this->label->height()) + (12 * 2);
    return QSize(width, height);
}

QLayout *Selector::selector()
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    this->update_label();

    QPushButton *left_button = new QPushButton();
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    left_button->setIcon(Theme::get_instance()->make_button_icon("arrow_left", left_button));
    connect(left_button, &QPushButton::clicked, [this]() {
        int count = this->options.size();
        this->current_idx = ((this->current_idx - 1) % count + count) % count;
        this->update_label();
        emit item_changed(this->get_current());
    });

    QPushButton *right_button = new QPushButton();
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    right_button->setIcon(Theme::get_instance()->make_button_icon("arrow_right", right_button));
    connect(right_button, &QPushButton::clicked, [this]() {
        this->current_idx = (this->current_idx + 1) % this->options.size();
        this->update_label();
        emit item_changed(this->get_current());
    });

    layout->addWidget(left_button);
    layout->addWidget(this->label, 4);
    layout->addWidget(right_button);

    return layout;
}

void Selector::set_state()
{
    if (this->options.size() == 0) {
        this->setEnabled(false);
    }
    else {
        this->setEnabled(true);
        if (!this->placeholder.isNull())
            this->options.insert(0, this->placeholder);
    }
}
