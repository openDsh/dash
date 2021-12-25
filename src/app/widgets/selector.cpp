#include <algorithm>

#include <QColor>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>

#include "app/arbiter.hpp"
#include "app/widgets/selector.hpp"

Selector::Selector(QList<QString> options, QString current, QFont font, Arbiter &arbiter, QWidget *parent, QString placeholder)
    : QWidget(parent)
    , options(options)
    , placeholder(placeholder)
    , arbiter(arbiter)
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
    auto it = std::max_element(this->options.begin(), this->options.end(), [](const auto &a, const auto &b){
        return a.size() < b.size();
    });

    int base = 32 * this->arbiter.layout().scale;
    int size = (it != this->options.end()) ? it->size() : 0;
    int width = (base * 2) + std::max(this->label->width(), QFontMetrics(this->label->font()).width(size)) + (12 * 4);
    int height = std::max(base, this->label->height()) + (12 * 2);
    return QSize(width, height);
}

QLayout *Selector::selector()
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    this->update_label();

    QPushButton *left_button = new QPushButton();
    left_button->setFlat(true);
    this->arbiter.forge().iconize("arrow_left", left_button, 32);
    connect(left_button, &QPushButton::clicked, [this]() {
        int count = this->options.size();
        this->current_idx = ((this->current_idx - 1) % count + count) % count;
        this->update_label();
        emit item_changed(this->get_current());
        emit idx_changed(this->current_idx - (this->placeholder.isNull() ? 0 : 1));
    });

    QPushButton *right_button = new QPushButton();
    right_button->setFlat(true);
    this->arbiter.forge().iconize("arrow_right", right_button, 32);
    connect(right_button, &QPushButton::clicked, [this]() {
        this->current_idx = (this->current_idx + 1) % this->options.size();
        this->update_label();
        emit item_changed(this->get_current());
        emit idx_changed(this->current_idx - (this->placeholder.isNull() ? 0 : 1));
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
