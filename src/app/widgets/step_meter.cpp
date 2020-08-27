#include <QColor>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QFrame>

#include "app/widgets/step_meter.hpp"

StepMeter::StepMeter(int steps, int val, QWidget *parent) : QFrame(parent)
{
    this->steps = steps;
    this->val = std::min(std::max(1, val), this->steps);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 0, 6, 0);

    for (int i = 1; i <= this->steps; i++) {
        QLayout *block = this->make_block(i);
        layout->addLayout(block);
    }

    this->setMaximumHeight(this->width() / 2);
}

QLayout *StepMeter::make_block(int split)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addStretch(this->steps - split);

    QWidget *block = new QWidget(this);
    this->blocks.append(block);
    block->setMinimumWidth(this->width() / this->steps);
    block->setAutoFillBackground(true);
    layout->addWidget(block, split);  

    return layout;
}

void StepMeter::update_notch_colors()
{
    QPalette palette(this->palette());
    palette.setColor(QPalette::Window, this->notch_color);

    for (int i = val - 1; i < this->steps; i++)
        this->blocks[i]->setPalette(palette);
}

void StepMeter::update_step_colors()
{
    QPalette palette(this->palette());
    palette.setColor(QPalette::Window, this->step_color);

    for (int i = 0; i < this->val; i++)
        this->blocks[i]->setPalette(palette);
}
