#pragma once

#include <QFrame>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "app/widgets/dialog.hpp"

class Arbiter;

class PageContainer : public QFrame {
    Q_OBJECT

   public:
    PageContainer(QWidget *widget);
    QWidget *take();
    void reset();

   private:
    QWidget *widget;
};

class Page {
   public:
    Page(Arbiter &arbiter, QString name, QString icon_name, bool toggleable, QWidget *widget);
    void enable(bool enable);

    const QString &name() { return this->name_; }
    const QString &icon_name() { return this->icon_name_; }
    const bool &toggleale() { return this->toggleable_; }
    PageContainer *container() { return this->container_; }
    QPushButton *button() { return this->button_; }
    const bool &enabled() { return this->enabled_; }

    virtual void init() = 0;

   protected:
    class Settings : public QWidget {
       public:
        Settings();
        void add_row(QString label, QWidget *control);
        // void add_br();
        // void add_widget(QWidget *widget);

       private:
        QVBoxLayout *layout_;

        static QLayout *row(QString label, QWidget *control);
    };

    Arbiter &arbiter;

    QPushButton *settings_button();
    QLayout *settings_layout();

   private:
    const QString name_;
    const QString icon_name_;
    const bool toggleable_;
    PageContainer *container_;
    QPushButton *button_;
    bool enabled_;

    Dialog *dialog();

    virtual Settings *settings_body() { return new Settings(); }
    virtual void on_settings_save() {}
};
