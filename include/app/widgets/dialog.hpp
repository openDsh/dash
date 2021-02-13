#pragma once

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
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
        QFont font(Theme::font_16);
        font.setBold(true);
        label->setFont(font);
        this->title->addWidget(label);
    }
    inline void set_body(QWidget *widget)
    {
        if (this->fullscreen) {
            QScrollArea *scroll_area = new QScrollArea(this);
            Theme::to_touch_scroller(scroll_area);
            scroll_area->setWidgetResizable(true);
            scroll_area->setWidget(widget);

            this->body->addWidget(scroll_area);
        }
        else {
            this->body->addWidget(widget);
        }
    }
    inline void set_button(QPushButton *button)
    {
        if (this->buttons->count() == 0)
            this->add_cancel_button();
        button->setFlat(true);
        this->buttons->addWidget(button, 0, Qt::AlignRight);
        connect(button, &QPushButton::clicked, [this]() { this->close(); });
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
    double scale;

    QWidget *content_widget();
    void set_position();

    inline void add_cancel_button()
    {
        this->buttons->addStretch();

        QPushButton *button = new QPushButton("cancel", this);
        button->setFlat(true);
        connect(button, &QPushButton::clicked, [this]() { this->close(); });
        this->buttons->addWidget(button, 0, Qt::AlignRight);
    }
};

class SnackBar : public Dialog {
    Q_OBJECT

   public:
    SnackBar() : Dialog(false, get_ref()) {}

   protected:
    void resizeEvent(QResizeEvent* event);

   private:
    QWidget *get_ref();
};
