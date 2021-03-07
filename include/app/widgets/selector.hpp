#pragma once

#include <QFont>
#include <QList>
#include <QPushButton>
#include <QWidget>

class Arbiter;

class Selector : public QWidget {
    Q_OBJECT

   public:
    Selector(QList<QString> options, QString current, QFont font, Arbiter &arbiter, QWidget *parent = nullptr, QString placeholder = QString());

    inline QString get_current() { return (this->options.size() > 0) ? this->options.value(this->current_idx, this->placeholder) : QString(); }
    inline void set_options(QList<QString> options)
    {
        this->options = options;
        this->set_state();
        this->current_idx = 0;
        this->update_label();
        emit item_changed(this->get_current());
        emit idx_changed(this->current_idx - (this->placeholder.isNull() ? 0 : 1));
    }

   protected:
    QSize sizeHint() const override;

   private:
    QList<QString> options;
    int current_idx;
    QString placeholder;
    Arbiter &arbiter;

    QLabel *label;

    QLayout *selector();
    void set_state();

    inline void update_label()
    {
        QFont styled_font(this->label->font());
        styled_font.setItalic(this->placeholder == this->get_current());

        this->label->setFont(styled_font);
        this->label->setText(this->get_current());
    }

   signals:
    void item_changed(QString item);
    void idx_changed(int idx);
};
