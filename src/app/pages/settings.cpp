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
#include "app/pages/settings.hpp"
#include "app/theme.hpp"
#include "app/widgets/color_label.hpp"
#include "app/widgets/selector.hpp"
#include "app/widgets/switch.hpp"
#include "app/window.hpp"
#include "app/widgets/sliders.hpp"

SettingsPage::SettingsPage(QWidget *parent) : QTabWidget(parent)
{
    this->tabBar()->setFont(Theme::font_16);

    this->addTab(new MainSettingsTab(this), "Main");
    this->addTab(new LayoutSettingsTab(this), "Layout");
    this->addTab(new BluetoothSettingsTab(this), "Bluetooth");
    this->addTab(new ActionsSettingsTab(this), "Actions");
}

MainSettingsTab::MainSettingsTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();
    this->shortcuts = Shortcuts::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 0, 6, 0);

    layout->addWidget(this->settings_widget());
}

QWidget *MainSettingsTab::settings_widget()
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

QWidget *MainSettingsTab::dark_mode_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Dark Mode", widget);
    label->setFont(Theme::font_14);
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

QWidget *MainSettingsTab::brightness_module_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Brightness Module", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    layout->addWidget(this->brightness_module_select_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::brightness_module_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    const QStringList plugins = this->config->get_brightness_plugins();

    QLabel *label = new QLabel(this->config->get_brightness_plugin_name(), widget);
    label->setAlignment(Qt::AlignCenter);
    label->setFont(Theme::font_14);

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    left_button->setIcon(this->theme->make_button_icon("arrow_left", left_button));
    connect(left_button, &QPushButton::clicked, [this, label, plugins]() {
        int total_plugins = plugins.size();
        QString plugin =
            plugins[((plugins.indexOf(label->text()) - 1) % total_plugins + total_plugins) % total_plugins];
        label->setText(plugin);
        this->config->set_brightness_plugin(plugin);
    });

    QPushButton *right_button = new QPushButton(widget);
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    right_button->setIcon(this->theme->make_button_icon("arrow_right", right_button));
    connect(right_button, &QPushButton::clicked, [this, label, plugins]() {
        QString plugin = plugins[(plugins.indexOf(label->text()) + 1) % plugins.size()];
        label->setText(plugin);
        this->config->set_brightness_plugin(plugin);
    });

    layout->addStretch(1);
    layout->addWidget(left_button);
    layout->addWidget(label, 2);
    layout->addWidget(right_button);
    layout->addStretch(1);

    return widget;
}

QWidget *MainSettingsTab::brightness_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Brightness", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    layout->addWidget(this->brightness_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::brightness_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    layout->addStretch(1);
    layout->addWidget(Slider::brightness(true, this), 6);
    layout->addStretch(1);

    return widget;
}

QWidget *MainSettingsTab::si_units_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("SI Units", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->get_si_units());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) { config->set_si_units(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *MainSettingsTab::color_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Color", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    layout->addWidget(this->color_select_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::color_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    const QStringList colors = this->theme->get_colors().keys();

    ColorLabel *label = new ColorLabel(Theme::icon_16, widget);
    label->scale(this->config->get_scale());
    label->setFont(Theme::font_14);
    connect(this->config, &Config::scale_changed, [label](double scale) { label->scale(scale); });

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    left_button->setIcon(this->theme->make_button_icon("arrow_left", left_button));
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
    right_button->setIcon(this->theme->make_button_icon("arrow_right", right_button));
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

QWidget *MainSettingsTab::mouse_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Mouse", widget);
    label->setFont(Theme::font_14);
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

QWidget *MainSettingsTab::volume_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Volume", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    layout->addWidget(this->volume_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::volume_widget()
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
    lower_button->setIcon(this->theme->make_button_icon("volume_down", lower_button));
    connect(lower_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 10); });

    QPushButton *raise_button = new QPushButton(widget);
    raise_button->setFlat(true);
    raise_button->setIconSize(Theme::icon_32);
    raise_button->setIcon(this->theme->make_button_icon("volume_up", raise_button));
    connect(raise_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 10); });

    layout->addStretch(1);
    layout->addWidget(lower_button);
    layout->addWidget(slider, 4);
    layout->addWidget(raise_button);
    layout->addStretch(1);

    return widget;
}

LayoutSettingsTab::LayoutSettingsTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 0, 6, 0);

    layout->addWidget(this->settings_widget());
}

QWidget *LayoutSettingsTab::settings_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    layout->addWidget(this->pages_widget());
    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->controls_bar_widget(), 1);

    QWidget *controls_bar_row = this->quick_view_row_widget();
    controls_bar_row->setVisible(this->config->get_controls_bar());
    connect(this->config, &Config::controls_bar_changed, [controls_bar_row](bool controls_bar) { controls_bar_row->setVisible(controls_bar); });
    layout->addWidget(controls_bar_row, 1);

    layout->addWidget(Theme::br(widget), 1);
    layout->addWidget(this->scale_row_widget(), 1);

    QScrollArea *scroll_area = new QScrollArea(this);
    Theme::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QWidget *LayoutSettingsTab::pages_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Pages", widget);
    label->setFont(Theme::font_14);
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

QWidget *LayoutSettingsTab::controls_bar_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Controls Bar", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->config->get_scale());
    toggle->setChecked(this->config->get_controls_bar());
    connect(this->config, &Config::scale_changed, [toggle](double scale) { toggle->scale(scale); });
    connect(toggle, &Switch::stateChanged, [config = this->config](bool state) { config->set_controls_bar(state); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *LayoutSettingsTab::quick_view_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Quick View", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    layout->addWidget(this->quick_view_select_widget(), 1);

    return widget;
}

QWidget *LayoutSettingsTab::quick_view_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    const QStringList views = this->config->get_quick_views().keys();

    QLabel *label = new QLabel(this->config->get_quick_view(), widget);
    label->setAlignment(Qt::AlignCenter);
    label->setFont(Theme::font_14);

    QPushButton *left_button = new QPushButton(widget);
    left_button->setFlat(true);
    left_button->setIconSize(Theme::icon_32);
    left_button->setIcon(this->theme->make_button_icon("arrow_left", left_button));
    connect(left_button, &QPushButton::clicked, [this, label, views]() {
        int total_views = views.size();
        QString view = views[((views.indexOf(label->text()) - 1) % total_views + total_views) % total_views];
        label->setText(view);
        this->config->set_quick_view(view);
    });

    QPushButton *right_button = new QPushButton(widget);
    right_button->setFlat(true);
    right_button->setIconSize(Theme::icon_32);
    right_button->setIcon(this->theme->make_button_icon("arrow_right", right_button));
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

QWidget *LayoutSettingsTab::scale_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Scale", widget);
    label->setFont(Theme::font_14);
    layout->addWidget(label, 1);

    layout->addWidget(this->scale_widget(), 1);

    return widget;
}

QWidget *LayoutSettingsTab::scale_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(2, 6);
    slider->setValue(this->config->get_scale() * 4);
    connect(slider, &QSlider::valueChanged, [config = this->config, slider](int position) {
        slider->setEnabled(false);
        double scale = position / 4.0;
        config->set_scale(scale);
        slider->setEnabled(true);
        slider->setFocus();
    });

    QPushButton *lower_button = new QPushButton(widget);
    lower_button->setFlat(true);
    lower_button->setIconSize(Theme::icon_32);
    lower_button->setIcon(this->theme->make_button_icon("remove", lower_button));
    connect(lower_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() - 1); });

    QPushButton *raise_button = new QPushButton(widget);
    raise_button->setFlat(true);
    raise_button->setIconSize(Theme::icon_32);
    raise_button->setIcon(this->theme->make_button_icon("add", raise_button));
    connect(raise_button, &QPushButton::clicked, [slider]() { slider->setValue(slider->value() + 1); });

    layout->addStretch(1);
    layout->addWidget(lower_button);
    layout->addWidget(slider, 4);
    layout->addWidget(raise_button);
    layout->addStretch(1);

    return widget;
}

BluetoothSettingsTab::BluetoothSettingsTab(QWidget *parent) : QWidget(parent)
{
    this->bluetooth = Bluetooth::get_instance();
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 0, 6, 0);

    layout->addWidget(this->controls_widget(), 1);
    layout->addWidget(this->devices_widget(), 1);
}

QWidget *BluetoothSettingsTab::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("Media Player", widget);
    label->setFont(Theme::font_14);
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

QWidget *BluetoothSettingsTab::scanner_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QPushButton *button = new QPushButton("scan", widget);
    button->setFont(Theme::font_14);
    button->setFlat(true);
    button->setCheckable(true);
    button->setEnabled(this->bluetooth->has_adapter()); // this could potentially be tricky
    button->setIconSize(Theme::icon_36);
    button->setIcon(this->theme->make_button_icon("bluetooth_searching", button));
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

QWidget *BluetoothSettingsTab::devices_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    for (BluezQt::DevicePtr device : this->bluetooth->get_devices()) {
        if (device->address() == this->config->get_bluetooth_device()) device->connectToDevice();
        QPushButton *button = new QPushButton(device->name(), widget);
        button->setFont(Theme::font_14);
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
        button->setFont(Theme::font_14);
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

ActionsSettingsTab::ActionsSettingsTab(QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();
    this->config = Config::get_instance();
    this->shortcuts = Shortcuts::get_instance();
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 0, 6, 0);

    layout->addWidget(this->settings_widget());
}

QWidget *ActionsSettingsTab::settings_widget()
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

QWidget *ActionsSettingsTab::shortcut_row_widget(QString id, QString description, Shortcut *shortcut)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel(description, widget);
    label->setFont(Theme::font_14);

    layout->addWidget(label, 1);
    layout->addWidget(this->shortcut_input_widget(id, shortcut), 1);

    return widget;
}

QWidget *ActionsSettingsTab::shortcut_input_widget(QString id, Shortcut *shortcut)
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
    symbol->setIcon(this->theme->make_button_icon("keyboard", symbol, "developer_board"));

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
