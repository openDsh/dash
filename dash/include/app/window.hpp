#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <stdlib.h>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QMainWindow>
#include <QStringList>
#include <QtDBus/QtDBus>
#include <QtWidgets>

#include "app/theme.hpp"

static int RESOLUTION = std::atoi(std::getenv("RESOLUTION"));

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QMainWindow *parent = 0);

    inline QObject &get_open_auto_active_area() { return *this->open_auto_active_area; };

    void start_open_auto();

   private slots:
    void update_brightness(int);
    void update_system_volume(int);
    void update_slider_volume();
    void update_icons(QList<QPair<int, QIcon>> &tab_icons, QList<QPair<QPushButton *, QIcon>> &button_icons);

   signals:
    void data_tab_toggle(bool);
    void toggle_open_auto(unsigned int);

   private:
    const int TAB_SIZE = 48 * RESOLUTION;

    Theme *theme;
    QTabWidget *tabs;
    QObject *open_auto_active_area;
    QSlider *volume_control;
    int brightness = 255;
};

#endif
