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
#include "app/shortcuts.hpp"
#include "app/pages/openauto.hpp"
#include "app/pages/page.hpp"
#include "app/theme.hpp"

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
    Config *config;
    Theme *theme;
    Shortcuts *shortcuts;

    OpenAutoPage *openauto;
    QStackedWidget *stack;
    QVBoxLayout *rail;
    QButtonGroup *rail_group;
    QStackedLayout *pages;
    QHBoxLayout *bar;

    void init_config();
    void init_theme();
    void init_ui();
    void init_shortcuts();
    QLayout *body();
    void add_pages();
    void add_page(Page *page);

    QWidget *controls_bar();
    QLayout *quick_views();
    QWidget *brightness_widget(bool skip_buttons = false);
    QWidget *controls_widget();
    QWidget *power_control();
    QWidget *save_control();
};
