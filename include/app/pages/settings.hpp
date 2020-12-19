#pragma once

#include <QMap>
#include <QtWidgets>
#include "openauto/Configuration/Configuration.hpp"

#include "app/bluetooth.hpp"
#include "app/config.hpp"
#include "app/shortcuts.hpp"
#include "app/theme.hpp"

class SettingsPage : public QTabWidget {
    Q_OBJECT

   public:
    SettingsPage(QWidget *parent = nullptr);
};

class MainSettingsTab : public QWidget {
    Q_OBJECT

   public:
    MainSettingsTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *dark_mode_row_widget();
    QWidget *brightness_plugin_row_widget();
    QWidget *brightness_plugin_select_widget();
    QWidget *brightness_row_widget();
    QWidget *brightness_widget();
    QWidget *si_units_row_widget();
    QWidget *color_row_widget();
    QWidget *color_select_widget();
    QWidget *mouse_row_widget();
    QWidget *volume_row_widget();
    QWidget *volume_widget();

    Config *config;
    Theme *theme;
    Shortcuts *shortcuts;
};

class LayoutSettingsTab : public QWidget {
    Q_OBJECT

   public:
    LayoutSettingsTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *default_page_widget();
    QWidget *default_page_select_widget();
    QWidget *pages_widget();
    QWidget *controls_bar_widget();
    QWidget *quick_view_row_widget();
    QWidget *quick_view_select_widget();
    QWidget *scale_row_widget();
    QWidget *scale_widget();

    Config *config;
    Theme *theme;
};

class BluetoothSettingsTab : public QWidget {
    Q_OBJECT

   public:
    BluetoothSettingsTab(QWidget *parent = nullptr);

   private:
    QWidget *controls_widget();
    QWidget *scanner_widget();
    QWidget *devices_widget();

    Bluetooth *bluetooth;
    Config *config;
    Theme *theme;
    QMap<BluezQt::DevicePtr, QPushButton *> devices;
};

class ActionsSettingsTab : public QWidget {
    Q_OBJECT

   public:
    ActionsSettingsTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *shortcut_row_widget(QString key, QString name, Shortcut *shortcut);
    QWidget *shortcut_input_widget(QString id, Shortcut *shortcut);

    Theme *theme;
    Config *config;
    Shortcuts *shortcuts;
};
