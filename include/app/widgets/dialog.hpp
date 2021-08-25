#pragma once

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>


class Arbiter;

class Dialog : public QDialog {
    Q_OBJECT

   public:
    Dialog(Arbiter &arbiter, bool fullscreen, QWidget *parent = nullptr);
    void open(int timeout = 0);

    void set_title(QString str);
    void set_body(QWidget *widget);
    void set_button(QPushButton *button);

   protected:
    Arbiter &arbiter;

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
        button->setFlat(true);
        connect(button, &QPushButton::clicked, [this]() { this->close(); });
        this->buttons->addWidget(button, 0, Qt::AlignRight);
    }
};

class SnackBar : public Dialog {
    Q_OBJECT

   public:
    SnackBar(Arbiter &arbiter);

    void resizeEvent(QResizeEvent* event);

   protected:
    void mousePressEvent(QMouseEvent *event) override;

   private:
    QWidget *get_ref();
};
