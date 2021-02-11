#pragma once

#include <stdlib.h>

#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QMainWindow>
#include <QStringList>
#include <QtDBus/QtDBus>
#include <QtWidgets>

#include "app/config.hpp"
#include "app/pages/openauto.hpp"
#include "app/pages/page.hpp"

#include "app/arbiter.hpp"

class DashWindow : public QMainWindow {
    Q_OBJECT

   public:
    DashWindow();
    void add_widget(QWidget *widget);

    inline QList<QAbstractButton *> get_pages() { return this->rail_group->buttons(); }

   protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

   private:
    Arbiter arbiter;

    OpenAutoPage *openauto;
    QStackedWidget *stack;
    QVBoxLayout *rail;
    QButtonGroup *rail_group;
    QStackedLayout *pages;
    QHBoxLayout *bar;

    void init_ui();
    QLayout *body();
    void add_pages();
    void add_page(Page *page);
    void set_page(Page *page);

    QWidget *controls_bar();
    QLayout *quick_views();
    QWidget *power_control();
};
