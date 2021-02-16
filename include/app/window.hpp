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

class DashWindow : public QMainWindow {
    Q_OBJECT

   public:
    DashWindow();

   protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

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

       private:
        static QWidget *power_control(Arbiter &arbiter);
        static QWidget *control_bar(Arbiter &arbiter);
    };

    Arbiter arbiter;
    NavRail rail;
    Body body;

    void add_pages();
    void set_page(Page *page);
};
