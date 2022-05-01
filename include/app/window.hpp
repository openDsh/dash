#pragma once

#include <QButtonGroup>
#include <QKeyEvent>
#include <QMainWindow>
#include <QObject>
#include <QShowEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWidget>

#include "app/config.hpp"
#include "app/pages/openauto.hpp"
#include "app/pages/page.hpp"

#include "app/arbiter.hpp"

class Dash : public QWidget {
    Q_OBJECT

   public:
    Dash(Arbiter &arbiter);
    void init();

   private:
    struct NavRail {
        QButtonGroup group;
        QVBoxLayout *layout;

        NavRail();
    };

    struct Body {
        QVBoxLayout *layout;
        QVBoxLayout *status_bar;
        QStackedLayout *frame;
        QVBoxLayout *control_bar;

        Body();
    };

    Arbiter &arbiter;
    NavRail rail;
    Body body;

    void set_page(Page *page);
    QWidget *status_bar() const;
    QWidget *control_bar() const;
    QWidget *power_control() const;
};

class Window : public QMainWindow {
    Q_OBJECT

   public:
    Window();

   protected:
    void showEvent(QShowEvent *event);
    // void keyPressEvent(QKeyEvent *event);
    // void keyReleaseEvent(QKeyEvent *event);

   private:
    Arbiter arbiter;
};
