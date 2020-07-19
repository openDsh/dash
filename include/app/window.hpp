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
#include "app/tabs/openauto.hpp"
#include "app/theme.hpp"

class DashWindow : public QMainWindow {
    Q_OBJECT

   public:
    DashWindow();
    void add_widget(QWidget *widget);

    inline QList<QAbstractButton *> get_pages() { return this->rail_group->buttons(); }

   protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

   private:
    Config *config;
    Theme *theme;
    Shortcuts *shortcuts;

    OpenAutoTab *openauto;
    QStackedWidget *stack;
    QVBoxLayout *rail;
    QButtonGroup *rail_group;
    QStackedLayout *pages;
    QHBoxLayout *bar;

    void init_config();
    void init_theme();
    void init_ui();
    QBoxLayout *body();
    void add_pages();
    void add_page(QString name, QWidget *page, QString icon);

    QWidget *quick_view_widget();
    QWidget *controls_bar_widget();
    QWidget *volume_widget(bool skip_buttons = false);
    QWidget *brightness_widget(bool skip_buttons = false);
    QWidget *controls_widget();
    QWidget *power_control_widget();
    QWidget *save_control_widget();
    void update_system_volume(int position);
};
