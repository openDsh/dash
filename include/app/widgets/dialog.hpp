#pragma once

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "app/theme.hpp"

class Dialog : public QDialog {
    Q_OBJECT

   public:
    Dialog(bool fullscreen, QWidget *parent = nullptr);
    void open(int timeout = 0);

    inline void set_title(QString str)
    {
        QLabel *label = new QLabel(str, this);
        label->setFont(QFont("Montserrat", 18, QFont::Bold));
        this->title->addWidget(label);
        qApp->processEvents();
        Theme::get_instance()->update();
    }
    inline void set_body(QWidget *widget)
    {
        this->setMinimumSize(widget->size());
        this->body->addWidget(widget);
        qApp->processEvents();
        Theme::get_instance()->update();
    }
    inline void set_button(QPushButton *button)
    {
        if (this->buttons->count() == 0)
            this->add_cancel_button();
        button->setFont(Theme::font_16);
        button->setFlat(true);
        this->buttons->addWidget(button, 0, Qt::AlignRight);
        connect(button, &QPushButton::clicked, [this]() { this->close(); });
        qApp->processEvents();
        Theme::get_instance()->update();
    }

   protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

   private:
    QVBoxLayout *title;
    QVBoxLayout *body;
    QHBoxLayout *buttons;
    QTimer *timer;
    bool fullscreen;

    QWidget *content_widget();
    void set_position();

    inline void add_cancel_button()
    {
        this->buttons->addStretch();

        QPushButton *button = new QPushButton("cancel", this);
        button->setFont(Theme::font_16);
        button->setFlat(true);
        connect(button, &QPushButton::clicked, [this]() { this->close(); });
        this->buttons->addWidget(button, 0, Qt::AlignRight);
    }
};
