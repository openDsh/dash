#include <BluezQt/Device>
#include <BluezQt/PendingCall>
#include <QLabel>
#include <QScrollArea>

#include <aasdk_proto/ButtonCodeEnum.pb.h>
#include <aasdk_proto/VideoFPSEnum.pb.h>
#include <aasdk_proto/VideoResolutionEnum.pb.h>

#include "openauto/Configuration/AudioOutputBackendType.hpp"
#include "openauto/Configuration/BluetootAdapterType.hpp"
#include "openauto/Configuration/HandednessOfTrafficType.hpp"

#include "app/config.hpp"
#include "app/tabs/settings.hpp"
#include "app/theme.hpp"
#include "app/widgets/color_label.hpp"
#include "app/widgets/switch.hpp"
#include "app/window.hpp"

SettingsTab::SettingsTab(QWidget *parent) : QTabWidget(parent)
{
    this->tabBar()->setFont(Theme::font_18);

    this->fill_tabs();
}

void SettingsTab::fill_tabs()
{
    this->addTab(new GeneralSettingsSubTab(this), "General");
    this->addTab(new LayoutSettingsSubTab(this), "Layout");
    this->addTab(new BluetoothSettingsSubTab(this), "Bluetooth");
    this->addTab(new ShortcutsSettingsSubTab(this), "Shortcuts");
    this->addTab(new OpenAutoSettingsSubTab(this), "OpenAuto");
}

GeneralSettingsSubTab::GeneralSettingsSubTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();
    this->shortcuts = Shortcuts::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(this->settings_widget());
}

QWidget *GeneralSettingsSubTab::settings_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    layout->addWidget(this->dark_mode_row_widget(), 1);
    layout->addWidget(this->color_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->mouse_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->si_units_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->volume_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->brightness_module_row_widget(), 1);
    layout->addWidget(this->brightness_row_widget(), 1);

    QScrollArea *scroll_area = new QScrollArea(this);
    Theme::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QWidget *GeneralSettingsSubTab::dark_mode_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Dark Mode", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->get_dark_mode());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(this->theme, &Theme::mode_updated,
            [toggle, config = this->config](bool mode) { toggle->setChecked(mode); });
    connect(toggle, &Switch::stateChanged, [theme = this->theme, config = this->config](bool state) {
        theme->set_mode(state);
        config->set_dark_mode(state);
    });
    Shortcut *shortcut = new Shortcut(this->config->get_shortcut("dark_mode_toggle"), this->window());
    this->shortcuts->add_shortcut("dark_mode_toggle", "Toggle Dark Mode", shortcut);
    connect(shortcut, &Shortcut::activated, [toggle]() { toggle->click(); });

    Shortcut *temp_shortcut_on = new Shortcut(this->config->get_shortcut("dark_mode_on"), this->window());
    this->shortcuts->add_shortcut("dark_mode_on", "[Enable Dark Mode]", temp_shortcut_on);
    connect(temp_shortcut_on, &Shortcut::activated, [toggle]() {
        if (!toggle->isChecked()) toggle->click();
    });
    Shortcut *temp_shortcut_off = new Shortcut(this->config->get_shortcut("dark_mode_off"), this->window());
    this->shortcuts->add_shortcut("dark_mode_off", "[Disable Dark Mode]", temp_shortcut_off);
    connect(temp_shortcut_off, &Shortcut::activated, [toggle]() {
        if (toggle->isChecked()) toggle->click();
    });

    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *GeneralSettingsSubTab::brightness_module_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Brightness Module", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    layout->addWidget(this->brightness_module_select_widget(), 1);

    return widget;
}

QWidget *GeneralSettingsSubTab::brightness_module_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    const QStringList modules = this->config->get_brightness_modules().keys();

    QLabel *label = new QLabel(this->config->get_brightness_module_name(), widget);
    label->setAlignment(Qt::AlignCenter);
    label->setFont(Theme::font_16);

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_left", left_button);
    connect(left_button, &QPushButton::clicked, [this, label, modules]() {
        int total_modules = modules.size();
        QString module =
            modules[((modules.indexOf(label->text()) - 1) % total_modules + total_modules) % total_modules];
        label->setText(module);
        this->config->set_brightness_module(module);
    });

    QPushButton *right_button = new QPushButton(widget);
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_right", right_button);
    connect(right_button, &QPushButton::clicked, [this, label, modules]() {
        QString module = modules[(modules.indexOf(label->text()) + 1) % modules.size()];
        label->setText(module);
        this->config->set_brightness_module(module);
    });

    layout->addStretch(1);
    layout->addWidget(left_button);
    layout->addWidget(label, 2);
    layout->addWidget(right_button);
    layout->addStretch(1);

    return widget;
}

QWidget *GeneralSettingsSubTab::brightness_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Brightness", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    layout->addWidget(this->brightness_widget(), 1);

    return widget;
}

QWidget *GeneralSettingsSubTab::brightness_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(76, 255);
    slider->setValue(this->config->get_brightness());
    connect(slider, &QSlider::valueChanged,
            [config = this->config](int position) { config->set_brightness(position); });
    connect(this->config, &Config::brightness_changed,
            [slider](int brightness) { slider->setValue(brightness); });

    Shortcut *dim_shortcut = new Shortcut(this->config->get_shortcut("brightness_down"), this->window());
    this->shortcuts->add_shortcut("brightness_down", "Decrease Brightness", dim_shortcut);
    connect(dim_shortcut, &Shortcut::activated, [slider]() { slider->setValue(slider->value() - 4); });
    Shortcut *brighten_shortcut = new Shortcut(this->config->get_shortcut("brightness_up"), this->window());
    this->shortcuts->add_shortcut("brightness_up", "Increase Brightness", brighten_shortcut);
    connect(brighten_shortcut, &Shortcut::activated, [slider]() { slider->setValue(slider->value() + 4); });

    QPushButton *dim_button = new QPushButton(widget);
    dim_button->setFlat(true);
    dim_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("brightness_low", dim_button);
    connect(dim_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 18); });

    QPushButton *brighten_button = new QPushButton(widget);
    brighten_button->setFlat(true);
    brighten_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("brightness_high", brighten_button);
    connect(brighten_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 18); });

    layout->addStretch(1);
    layout->addWidget(dim_button);
    layout->addWidget(slider, 4);
    layout->addWidget(brighten_button);
    layout->addStretch(1);

    return widget;
}

QWidget *GeneralSettingsSubTab::si_units_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("SI Units", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->get_si_units());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) { config->set_si_units(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *GeneralSettingsSubTab::color_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Color", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    layout->addWidget(this->color_select_widget(), 1);

    return widget;
}

QWidget *GeneralSettingsSubTab::color_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    const QStringList colors = this->theme->get_colors().keys();

    ColorLabel *label = new ColorLabel(Theme::icon_16, widget);
    label->scale(this->config->get_scale());
    label->setFont(Theme::font_16);
    connect(this->config, &Config::scale_changed, [label](double scale) { label->scale(scale); });

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_left", left_button);
    connect(left_button, &QPushButton::clicked, [this, colors, label]() {
        int total_colors = colors.size();
        QString color = colors[((colors.indexOf(label->text()) - 1) % total_colors + total_colors) % total_colors];
        label->update(color);
        this->theme->set_color(color);
        this->config->set_color(color);
    });

    QPushButton *right_button = new QPushButton(widget);
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_right", right_button);
    connect(right_button, &QPushButton::clicked, [this, colors, label]() {
        QString color = colors[(colors.indexOf(label->text()) + 1) % colors.size()];
        label->update(color);
        this->theme->set_color(color);
        this->config->set_color(color);
    });

    layout->addStretch(1);
    layout->addWidget(left_button);
    layout->addWidget(label, 2);
    layout->addWidget(right_button);
    layout->addStretch(1);

    return widget;
}

QWidget *GeneralSettingsSubTab::mouse_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Mouse", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->get_mouse_active());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        qApp->setOverrideCursor(state ? Qt::ArrowCursor : Qt::BlankCursor);
        config->set_mouse_active(state);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *GeneralSettingsSubTab::volume_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Volume", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    layout->addWidget(this->volume_widget(), 1);

    return widget;
}

QWidget *GeneralSettingsSubTab::volume_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(0, 100);
    slider->setValue(this->config->get_volume());
    connect(slider, &QSlider::valueChanged, [config = this->config](int position) {
        config->set_volume(position);
    });
    connect(this->config, &Config::volume_changed,
            [slider](int volume) { slider->setValue(volume); });

    Shortcut *lower_shortcut = new Shortcut(this->config->get_shortcut("volume_down"), this->window());
    this->shortcuts->add_shortcut("volume_down", "Decrease Volume", lower_shortcut);
    connect(lower_shortcut, &Shortcut::activated, [slider]() { slider->setValue(slider->value() - 2); });
    Shortcut *upper_shortcut = new Shortcut(this->config->get_shortcut("volume_up"), this->window());
    this->shortcuts->add_shortcut("volume_up", "Increase Volume", upper_shortcut);
    connect(upper_shortcut, &Shortcut::activated, [slider]() { slider->setValue(slider->value() + 2); });

    QPushButton *lower_button = new QPushButton(widget);
    lower_button->setFlat(true);
    lower_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("volume_down", lower_button);
    connect(lower_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 10); });

    QPushButton *raise_button = new QPushButton(widget);
    raise_button->setFlat(true);
    raise_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("volume_up", raise_button);
    connect(raise_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 10); });

    layout->addStretch(1);
    layout->addWidget(lower_button);
    layout->addWidget(slider, 4);
    layout->addWidget(raise_button);
    layout->addStretch(1);

    return widget;
}

LayoutSettingsSubTab::LayoutSettingsSubTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(this->settings_widget());
}

QWidget *LayoutSettingsSubTab::settings_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    layout->addWidget(this->pages_widget());
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->controls_bar_widget(), 1);
    layout->addWidget(this->quick_view_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->scale_row_widget(), 1);

    QScrollArea *scroll_area = new QScrollArea(this);
    Theme::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QWidget *LayoutSettingsSubTab::pages_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Pages", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox(widget);
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    DashWindow *window = qobject_cast<DashWindow *>(this->window());

    for (QAbstractButton *page : window->get_pages()) {
        QCheckBox *button = new QCheckBox(page->property("page").value<QWidget *>()->objectName(), group);
        button->setFont(Theme::font_14);
        button->setChecked(!page->isHidden());
        connect(button, &QCheckBox::toggled, [page, config = this->config](bool checked) {
            config->set_page(page->property("page").value<QWidget *>(), checked);
        });
        group_layout->addWidget(button);
    }

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *LayoutSettingsSubTab::controls_bar_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Controls Bar", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->get_controls_bar());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) { config->set_controls_bar(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *LayoutSettingsSubTab::quick_view_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Quick View", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    layout->addWidget(this->quick_view_select_widget(), 1);

    return widget;
}

QWidget *LayoutSettingsSubTab::quick_view_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    const QStringList views = this->config->get_quick_views().keys();

    QLabel *label = new QLabel(this->config->get_quick_view(), widget);
    label->setAlignment(Qt::AlignCenter);
    label->setFont(Theme::font_16);

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_left", left_button);
    connect(left_button, &QPushButton::clicked, [this, label, views]() {
        int total_views = views.size();
        QString view = views[((views.indexOf(label->text()) - 1) % total_views + total_views) % total_views];
        label->setText(view);
        this->config->set_quick_view(view);
    });

    QPushButton *right_button = new QPushButton(widget);
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_right", right_button);
    connect(right_button, &QPushButton::clicked, [this, label, views]() {
        QString view = views[(views.indexOf(label->text()) + 1) % views.size()];
        label->setText(view);
        this->config->set_quick_view(view);
    });

    layout->addStretch(1);
    layout->addWidget(left_button);
    layout->addWidget(label, 2);
    layout->addWidget(right_button);
    layout->addStretch(1);

    return widget;
}

QWidget *LayoutSettingsSubTab::scale_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Scale", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    layout->addWidget(this->scale_widget(), 1);

    return widget;
}

QWidget *LayoutSettingsSubTab::scale_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(1, 4);
    slider->setValue(this->config->get_scale() * 4);
    QLabel *value = new QLabel(QString("x%1").arg(slider->value() / 4.0), widget);
    value->setFont(Theme::font_14);
    connect(slider, &QSlider::valueChanged, [config = this->config, slider, value](int position) {
        slider->setEnabled(false);
        double scale = position / 4.0;
        value->setText(QString("x%1").arg(scale));
        config->set_scale(scale);
        slider->setEnabled(true);
        slider->setFocus();
    });
    connect(slider, &QSlider::sliderMoved, [value](int position) {
        double scale = position / 4.0;
        value->setText(QString("x%1").arg(scale));
    });

    layout->addStretch(2);
    layout->addWidget(slider, 4);
    layout->addWidget(value, 2);

    return widget;
}

BluetoothSettingsSubTab::BluetoothSettingsSubTab(QWidget *parent) : QWidget(parent)
{
    this->bluetooth = Bluetooth::get_instance();
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->addWidget(this->controls_widget(), 1);
    layout->addWidget(this->devices_widget(), 1);
}

QWidget *BluetoothSettingsSubTab::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("Media Player", widget);
    label->setFont(Theme::font_16);
    layout->addStretch();
    layout->addWidget(label);

    QLabel *connected_device = new QLabel(this->bluetooth->get_media_player().first, widget);
    connected_device->setIndent(16);
    connected_device->setFont(Theme::font_14);
    connect(this->bluetooth, &Bluetooth::media_player_changed,
            [connected_device](QString name, BluezQt::MediaPlayerPtr) { connected_device->setText(name); });
    layout->addWidget(connected_device);
    layout->addStretch();

    layout->addWidget(this->scanner_widget());

    return widget;
}

QWidget *BluetoothSettingsSubTab::scanner_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *button = new QPushButton("scan", widget);
    button->setFont(Theme::font_14);
    button->setFlat(true);
    button->setCheckable(true);
    button->setEnabled(this->bluetooth->has_adapter());
    button->setIconSize(Theme::icon_36);
    this->theme->add_button_icon("bluetooth_searching", button);
    connect(button, &QPushButton::clicked, [bluetooth = this->bluetooth](bool checked) {
        if (checked)
            bluetooth->start_scan();
        else
            bluetooth->stop_scan();
    });
    layout->addWidget(button);

    ProgressIndicator *loader = new ProgressIndicator(widget);
    loader->scale(this->config->get_scale());
    connect(this->config, &Config::scale_changed, [loader](double scale) { loader->scale(scale); });
    connect(this->bluetooth, &Bluetooth::scan_status, [button, loader](bool status) {
        if (status)
            loader->start_animation();
        else
            loader->stop_animation();
        button->setChecked(status);
    });
    layout->addWidget(loader);

    return widget;
}

QWidget *BluetoothSettingsSubTab::devices_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    for (BluezQt::DevicePtr device : this->bluetooth->get_devices()) {
        if (device->address() == this->config->get_bluetooth_device()) device->connectToDevice();
        QPushButton *button = new QPushButton(device->name(), widget);
        button->setFont(Theme::font_16);
        button->setCheckable(true);
        if (device->isConnected()) button->setChecked(true);
        connect(button, &QPushButton::clicked, [config = this->config, button, device](bool checked = false) {
            button->setChecked(!checked);
            if (checked) {
                device->connectToDevice();
                config->set_bluetooth_device(device->address());
            }
            else {
                device->disconnectFromDevice();
                config->set_bluetooth_device(QString());
            }
        });

        this->devices[device] = button;
        layout->addWidget(button);
    }
    connect(this->bluetooth, &Bluetooth::device_added, [this, layout, widget](BluezQt::DevicePtr device) {
        QPushButton *button = new QPushButton(device->name(), widget);
        button->setFont(Theme::font_16);
        button->setCheckable(true);
        if (device->isConnected()) button->setChecked(true);
        connect(button, &QPushButton::clicked, [button, device](bool checked = false) {
            button->setChecked(!checked);
            if (checked)
                device->connectToDevice()->waitForFinished();
            else
                device->disconnectFromDevice()->waitForFinished();
        });
        this->devices[device] = button;
        layout->addWidget(button);
        qApp->processEvents();
        this->theme->update();
    });
    connect(this->bluetooth, &Bluetooth::device_changed, [this](BluezQt::DevicePtr device) {
        this->devices[device]->setText(device->name());
        this->devices[device]->setChecked(device->isConnected());
    });
    connect(this->bluetooth, &Bluetooth::device_removed, [this, layout](BluezQt::DevicePtr device) {
        layout->removeWidget(devices[device]);
        delete this->devices[device];
        this->devices.remove(device);
    });

    QScrollArea *scroll_area = new QScrollArea(this);
    Theme::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

ShortcutsSettingsSubTab::ShortcutsSettingsSubTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();
    this->shortcuts = Shortcuts::get_instance();
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(this->settings_widget());
}

QWidget *ShortcutsSettingsSubTab::settings_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QMap<QString, QPair<QString, Shortcut *>> shortcuts = this->shortcuts->get_shortcuts();
    for (auto id : shortcuts.keys()) {
        QPair<QString, Shortcut *> shortcut = shortcuts[id];
        layout->addWidget(this->shortcut_row_widget(id, shortcut.first, shortcut.second));
    }
    connect(this->shortcuts, &Shortcuts::shortcut_added,
            [this, layout](QString id, QString description, Shortcut *shortcut) {
                layout->addWidget(this->shortcut_row_widget(id, description, shortcut));
            });

    QScrollArea *scroll_area = new QScrollArea(this);
    Theme::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QWidget *ShortcutsSettingsSubTab::shortcut_row_widget(QString id, QString description, Shortcut *shortcut)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel(description, widget);
    label->setFont(Theme::font_16);

    layout->addWidget(label, 1);
    layout->addWidget(this->shortcut_input_widget(id, shortcut), 1);

    return widget;
}

QWidget *ShortcutsSettingsSubTab::shortcut_input_widget(QString id, Shortcut *shortcut)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *symbol = new QPushButton(widget);
    QSizePolicy symbol_policy = symbol->sizePolicy();
    symbol_policy.setRetainSizeWhenHidden(true);
    symbol->setSizePolicy(symbol_policy);
    symbol->setFocusPolicy(Qt::NoFocus);
    symbol->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    symbol->setFlat(true);
    symbol->setCheckable(true);
    if (shortcut->to_str().isEmpty())
        symbol->hide();
    else
        symbol->setChecked(shortcut->to_str().startsWith("gpio"));
    symbol->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("developer_board", symbol, "keyboard");

    ShortcutInput *input = new ShortcutInput(shortcut->to_str(), widget);
    input->setProperty("add_hint", true);
    input->setFlat(true);
    input->setFont(QFont("Titillium Web", 18));
    connect(input, &ShortcutInput::shortcut_updated, [this, id, symbol](QString shortcut) {
        if (shortcut.isEmpty()) {
            symbol->hide();
        }
        else {
            symbol->setChecked(shortcut.startsWith("gpio"));
            symbol->show();
        }
        this->shortcuts->update_shortcut(id, shortcut);
        this->config->set_shortcut(id, shortcut);
    });

    layout->addStretch(1);
    layout->addWidget(input, 3);
    layout->addStretch(1);
    layout->addWidget(symbol, 1, Qt::AlignRight);

    return widget;
}

OpenAutoSettingsSubTab::OpenAutoSettingsSubTab(QWidget *parent) : QWidget(parent)
{
    this->bluetooth = Bluetooth::get_instance();
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(this->settings_widget());
}

QWidget *OpenAutoSettingsSubTab::settings_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    layout->addWidget(this->rhd_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->frame_rate_row_widget(), 1);
    layout->addWidget(this->resolution_row_widget(), 1);
    layout->addWidget(this->dpi_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->rt_audio_row_widget(), 1);
    layout->addWidget(this->audio_channels_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->bluetooth_row_widget(), 1);
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->touchscreen_row_widget(), 1);
    layout->addWidget(this->buttons_row_widget(), 1);

    QScrollArea *scroll_area = new QScrollArea(this);
    Theme::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QWidget *OpenAutoSettingsSubTab::rhd_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Right-Hand-Drive", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->openauto_config->getHandednessOfTrafficType() ==
                       openauto::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE);
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setHandednessOfTrafficType(
            state ? openauto::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE
                  : openauto::configuration::HandednessOfTrafficType::LEFT_HAND_DRIVE);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *OpenAutoSettingsSubTab::frame_rate_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Frame Rate", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox(widget);
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QRadioButton *fps_30_button = new QRadioButton("30fps", group);
    fps_30_button->setFont(Theme::font_14);
    fps_30_button->setChecked(this->config->openauto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    connect(fps_30_button, &QRadioButton::clicked,
            [config = this->config]() { config->openauto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_30); });
    group_layout->addWidget(fps_30_button);

    QRadioButton *fps_60_button = new QRadioButton("60fps", group);
    fps_60_button->setFont(Theme::font_14);
    fps_60_button->setChecked(this->config->openauto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_60);
    connect(fps_60_button, &QRadioButton::clicked,
            [config = this->config]() { config->openauto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_60); });
    group_layout->addWidget(fps_60_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *OpenAutoSettingsSubTab::resolution_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Resolution", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox(widget);
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QRadioButton *res_480_button = new QRadioButton("480p", group);
    res_480_button->setFont(Theme::font_14);
    res_480_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                               aasdk::proto::enums::VideoResolution::_480p);
    connect(res_480_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_480p);
    });
    group_layout->addWidget(res_480_button);

    QRadioButton *res_720_button = new QRadioButton("720p", group);
    res_720_button->setFont(Theme::font_14);
    res_720_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                               aasdk::proto::enums::VideoResolution::_720p);
    connect(res_720_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_720p);
    });
    group_layout->addWidget(res_720_button);

    QRadioButton *res_1080_button = new QRadioButton("1080p", group);
    res_1080_button->setFont(Theme::font_14);
    res_1080_button->setChecked(this->config->openauto_config->getVideoResolution() ==
                                aasdk::proto::enums::VideoResolution::_1080p);
    connect(res_1080_button, &QRadioButton::clicked, [config = this->config]() {
        config->openauto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_1080p);
    });
    group_layout->addWidget(res_1080_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *OpenAutoSettingsSubTab::dpi_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("DPI", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    layout->addWidget(this->dpi_widget(), 1);

    return widget;
}

QWidget *OpenAutoSettingsSubTab::dpi_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(0, 512);
    slider->setValue(this->config->openauto_config->getScreenDPI());
    QLabel *value = new QLabel(QString::number(slider->value()), widget);
    value->setFont(Theme::font_14);
    connect(slider, &QSlider::valueChanged, [config = this->config, value](int position) {
        config->openauto_config->setScreenDPI(position);
        value->setText(QString::number(position));
    });
    connect(slider, &QSlider::sliderMoved, [value](int position) {
        value->setText(QString::number(position));
    });

    layout->addStretch(2);
    layout->addWidget(slider, 4);
    layout->addWidget(value, 2);

    return widget;
}

QWidget *OpenAutoSettingsSubTab::rt_audio_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("RtAudio", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->openauto_config->getAudioOutputBackendType() ==
                       openauto::configuration::AudioOutputBackendType::RTAUDIO);
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setAudioOutputBackendType(state
                                                               ? openauto::configuration::AudioOutputBackendType::RTAUDIO
                                                               : openauto::configuration::AudioOutputBackendType::QT);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *OpenAutoSettingsSubTab::audio_channels_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Audio Channels", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox(widget);
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    QCheckBox *music_button = new QCheckBox("Music", group);
    music_button->setFont(Theme::font_14);
    music_button->setChecked(this->config->openauto_config->musicAudioChannelEnabled());
    connect(music_button, &QCheckBox::toggled,
            [config = this->config](bool checked) { config->openauto_config->setMusicAudioChannelEnabled(checked); });
    group_layout->addWidget(music_button);
    group_layout->addStretch(2);

    QCheckBox *speech_button = new QCheckBox("Speech", group);
    speech_button->setFont(Theme::font_14);
    speech_button->setChecked(this->config->openauto_config->speechAudioChannelEnabled());
    connect(speech_button, &QCheckBox::toggled,
            [config = this->config](bool checked) { config->openauto_config->setSpeechAudioChannelEnabled(checked); });
    group_layout->addWidget(speech_button);

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *OpenAutoSettingsSubTab::bluetooth_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Bluetooth", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->openauto_config->getBluetoothAdapterType() ==
                       openauto::configuration::BluetoothAdapterType::LOCAL);
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) {
        config->openauto_config->setBluetoothAdapterType(state ? openauto::configuration::BluetoothAdapterType::LOCAL
                                                               : openauto::configuration::BluetoothAdapterType::NONE);
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *OpenAutoSettingsSubTab::touchscreen_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Touchscreen", widget);
    label->setFont(Theme::font_16);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->openauto_config->getTouchscreenEnabled());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged,
            [config = this->config](bool state) { config->openauto_config->setTouchscreenEnabled(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QCheckBox *OpenAutoSettingsSubTab::button_checkbox(QString name, QString key,
                                                   aasdk::proto::enums::ButtonCode::Enum code, QWidget *parent)
{
    QCheckBox *checkbox = new QCheckBox(QString("%1 [%2]").arg(name).arg(key), parent);
    checkbox->setFont(Theme::font_14);
    checkbox->setChecked(std::find(this->config->openauto_button_codes.begin(),
                                   this->config->openauto_button_codes.end(),
                                   code) != this->config->openauto_button_codes.end());
    connect(checkbox, &QCheckBox::toggled, [config = this->config, code](bool checked) {
        if (checked) {
            config->openauto_button_codes.push_back(code);
        }
        else {
            config->openauto_button_codes.erase(
                std::remove(config->openauto_button_codes.begin(), config->openauto_button_codes.end(), code),
                config->openauto_button_codes.end());
        }
    });

    return checkbox;
}

QWidget *OpenAutoSettingsSubTab::buttons_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Buttons", widget);
    label->setFont(Theme::font_16);

    QGroupBox *group = new QGroupBox(widget);
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    group_layout->addWidget(this->button_checkbox("Enter", "Enter", aasdk::proto::enums::ButtonCode::ENTER, widget));
    group_layout->addWidget(this->button_checkbox("Left", "Left", aasdk::proto::enums::ButtonCode::LEFT, widget));
    group_layout->addWidget(this->button_checkbox("Right", "Right", aasdk::proto::enums::ButtonCode::RIGHT, widget));
    group_layout->addWidget(this->button_checkbox("Up", "Up", aasdk::proto::enums::ButtonCode::UP, widget));
    group_layout->addWidget(this->button_checkbox("Down", "Down", aasdk::proto::enums::ButtonCode::DOWN, widget));
    group_layout->addWidget(this->button_checkbox("Back", "Esc", aasdk::proto::enums::ButtonCode::BACK, widget));
    group_layout->addWidget(this->button_checkbox("Home", "H", aasdk::proto::enums::ButtonCode::HOME, widget));
    group_layout->addWidget(this->button_checkbox("Phone", "P", aasdk::proto::enums::ButtonCode::PHONE, widget));
    group_layout->addWidget(this->button_checkbox("Call End", "O", aasdk::proto::enums::ButtonCode::CALL_END, widget));
    group_layout->addWidget(this->button_checkbox("Play", "X", aasdk::proto::enums::ButtonCode::PLAY, widget));
    group_layout->addWidget(this->button_checkbox("Pause", "C", aasdk::proto::enums::ButtonCode::PAUSE, widget));
    group_layout->addWidget(this->button_checkbox("Prev Track", "V", aasdk::proto::enums::ButtonCode::PREV, widget));
    group_layout->addWidget(this->button_checkbox("Next Track", "N", aasdk::proto::enums::ButtonCode::NEXT, widget));
    group_layout->addWidget(
        this->button_checkbox("Toggle Play", "B", aasdk::proto::enums::ButtonCode::TOGGLE_PLAY, widget));
    group_layout->addWidget(this->button_checkbox("Voice", "M", aasdk::proto::enums::ButtonCode::MICROPHONE_1, widget));
    group_layout->addWidget(
        this->button_checkbox("Scroll", "1/2", aasdk::proto::enums::ButtonCode::SCROLL_WHEEL, widget));

    layout->addWidget(label, 1);
    layout->addWidget(group, 1, Qt::AlignHCenter);

    return widget;
}
