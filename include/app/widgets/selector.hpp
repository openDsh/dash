#pragma once

#include <QFont>
#include <QList>
#include <QPushButton>
#include <QWidget>

class Selector : public QWidget {
    Q_OBJECT

   public:
    Selector(QList<QString> options, QFont font, QWidget *parent = nullptr);

    inline QString get_current() { return this->options[this->current_idx]; }

   private:
    QFont font;
    int current_idx;
    QList<QString> options;

    QLayout *selector();
};
