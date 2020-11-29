#pragma once

#include <QFont>
#include <QList>
#include <QPushButton>
#include <QWidget>

class Selector : public QWidget {
    Q_OBJECT

   public:
    Selector(QList<QString> options, QString current, QFont font, QWidget *parent = nullptr, QString placeholder = QString());

    inline QString get_current() { return (this->options.size() > 0) ? this->options.value(this->current_idx, this->placeholder) : QString(); }
    inline void set_options(QList<QString> options)
    {
        this->options = options;
        this->set_state();
        this->current_idx = 0;
        this->update_label();
        emit item_changed(this->get_current());
    }

   private:
    QList<QString> options;
    QFont font;
    int current_idx;
    QString placeholder;

    QLabel *label;

    QLayout *selector();
    void set_state();

    inline void update_label()
    {
        QFont italicized(this->font);
        italicized.setItalic(true);

        this->label->setFont((this->placeholder == this->get_current()) ? italicized : this->font);
        this->label->setText(this->get_current());
    }

   signals:
    void item_changed(QString item);
};
