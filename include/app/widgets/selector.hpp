#pragma once

#include <QFont>
#include <QList>
#include <QPushButton>
#include <QWidget>

class Selector : public QWidget {
    Q_OBJECT

   public:
    Selector(QList<QString> options, QString current, QFont font, QWidget *parent = nullptr, bool add_null = false);

    inline QString get_current() { return this->options.value(this->current_idx, QString()); }
    inline void set_options(QList<QString> options, bool add_null = false)
    {
        this->options = options;
        if (add_null)
            this->options.insert(0, QString());
        this->setEnabled(this->options.size() > 0);
        this->current_idx = 0;
        this->label->setText(this->get_current());
        emit item_changed(this->get_current());
    }

   private:
    QList<QString> options;
    QFont font;
    int current_idx;

    QLabel *label;

    QLayout *selector();

   signals:
    void item_changed(QString item);
};
