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

#include <app/config.hpp>
#include <app/theme.hpp>
#include <app/tabs/open_auto.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow();

   protected:
    void showEvent(QShowEvent *event);

   private:
    static void update_system_volume(int position);
    QWidget *controls_widget();
    QWidget *volume_widget();

    Config *config;
    Theme *theme;
    OpenAutoTab *open_auto_tab;

   signals:
    void set_data_state(bool enabled);
    void set_open_auto_state(unsigned int alpha);
};

#endif
