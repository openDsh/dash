#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <QMap>
#include <QtWidgets>
#include <f1x/openauto/autoapp/Configuration/Configuration.hpp>

#include <app/bluetooth.hpp>
#include <app/config.hpp>
#include <app/shortcuts.hpp>
#include <app/theme.hpp>

namespace aasdk = f1x::aasdk;

class SettingsTab : public QTabWidget {
    Q_OBJECT

   public:
    SettingsTab(QWidget *parent = nullptr);
    void fill_tabs();
};

class GeneralSettingsSubTab : public QWidget {
    Q_OBJECT

   public:
    GeneralSettingsSubTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *dark_mode_row_widget();
    QWidget *brightness_module_row_widget();
    QWidget *brightness_module_select_widget();
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

class LayoutSettingsSubTab : public QWidget {
    Q_OBJECT

   public:
    LayoutSettingsSubTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *pages_widget();
    QWidget *controls_bar_widget();
    QWidget *quick_view_row_widget();
    QWidget *quick_view_select_widget();
    QWidget *scale_row_widget();
    QWidget *scale_widget();

    Config *config;
    Theme *theme;
};

class BluetoothSettingsSubTab : public QWidget {
    Q_OBJECT

   public:
    BluetoothSettingsSubTab(QWidget *parent = nullptr);

   private:
    QWidget *controls_widget();
    QWidget *scanner_widget();
    QWidget *devices_widget();

    Bluetooth *bluetooth;
    Config *config;
    Theme *theme;
    QMap<BluezQt::DevicePtr, QPushButton *> devices;
};

class ShortcutsSettingsSubTab : public QWidget {
    Q_OBJECT

   public:
    ShortcutsSettingsSubTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *shortcut_row_widget(QString key, QString name, Shortcut *shortcut);
    QWidget *shortcut_input_widget(QString id, Shortcut *shortcut);

    Theme *theme;
    Config *config;
    Shortcuts *shortcuts;
};

class OpenAutoSettingsSubTab : public QWidget {
    Q_OBJECT

   public:
    OpenAutoSettingsSubTab(QWidget *parent = nullptr);

   private:
    QWidget *settings_widget();
    QWidget *rhd_row_widget();
    QWidget *frame_rate_row_widget();
    QWidget *resolution_row_widget();
    QWidget *dpi_row_widget();
    QWidget *dpi_widget();
    QWidget *rt_audio_row_widget();
    QWidget *audio_channels_row_widget();
    QWidget *bluetooth_row_widget();
    QWidget *touchscreen_row_widget();
    QCheckBox *button_checkbox(QString name, QString key, aasdk::proto::enums::ButtonCode::Enum code, QWidget *parent);
    QWidget *buttons_row_widget();

    Bluetooth *bluetooth;
    Config *config;
    Theme *theme;
};

#endif
