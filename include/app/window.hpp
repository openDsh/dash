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
        QVBoxLayout *layout;
        QButtonGroup *group;

        NavRail(QObject *parent);
    };

    struct Body {
        QVBoxLayout *layout;
        QStackedLayout *frame;

        Body(Arbiter &arbiter);
    };

    Arbiter &arbiter;
    NavRail rail;
    Body body;

    void set_page(Page *page);
    QWidget *control_bar();
    QWidget *power_control();
};

class Window : public QMainWindow {
    Q_OBJECT

   public:
    Window();

   protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

   private:
    Arbiter arbiter;
};
