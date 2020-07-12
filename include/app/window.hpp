#pragma once

#include <stdlib.h>

#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QMainWindow>
#include <QStringList>
#include <QtDBus/QtDBus>
#include <QtWidgets>

#include <app/config.hpp>
#include <app/shortcuts.hpp>
#include <app/tabs/openauto.hpp>
#include <app/theme.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow();

    inline void add_widget(QWidget *widget) { this->layout->addWidget(widget); }
    inline void remove_widget(QWidget *widget) { this->layout->removeWidget(widget); }
    inline void set_widget() { layout->setCurrentIndex(1); }
    inline void unset_widget() { layout->setCurrentIndex(0); }

   protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

   private:
    static void update_system_volume(int position);
    QWidget *window_widget();
    QTabWidget *tabs_widget();
    QWidget *controls_bar_widget();
    QWidget *quick_view_widget();
    QWidget *volume_widget(bool skip_buttons = false);
    QWidget *brightness_widget(bool skip_buttons = false);
    QWidget *controls_widget();
    QWidget *power_control_widget();
    QWidget *save_control_widget();

    Config *config;
    Theme *theme;
    Shortcuts *shortcuts;
    QStackedLayout *layout;
    OpenAutoTab *openauto;

    const QSize TAB_SIZE = Theme::icon_48;

   signals:
    void is_ready();
    void set_openauto_state(unsigned int alpha);
};

class Window2 : public QMainWindow {
    Q_OBJECT

   public:
    Window2();
    inline QList<QAbstractButton *> get_pages() { return this->rail_group->buttons(); }

   protected:

   private:
    Config *config;
    Theme *theme;
    Shortcuts *shortcuts;

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

   signals:
};
