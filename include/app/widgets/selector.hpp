#pragma once

#include <QFont>
#include <QList>
#include <QPushButton>
#include <QWidget>

class Selector : public QWidget {
    Q_OBJECT

   public:
    Selector(QList<QString> options, int current_idx, QFont font, QWidget *parent = nullptr);
    // allow widgets to be passed in as options

    inline QString get_current() { return this->options.value(this->current_idx, QString()); }

   private:
    QFont font;
    int current_idx;
    QList<QString> options;

    QLayout *selector();

   signals:
    void item_changed(QString item);
};
