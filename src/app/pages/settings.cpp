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
#include "app/session.hpp"
#include "app/pages/settings.hpp"
#include "app/widgets/color_picker.hpp"
#include "app/widgets/selector.hpp"
#include "app/widgets/switch.hpp"
#include "app/window.hpp"


SettingsPage::SettingsPage(Arbiter &arbiter, QWidget *parent)
    : QTabWidget(parent)
    , Page(arbiter, "Settings", "tune", false, this)
{
}

void SettingsPage::init()
{
    this->addTab(new MainSettingsTab(this->arbiter), "Main");
    this->addTab(new LayoutSettingsTab(this->arbiter), "Layout");
    this->addTab(new BluetoothSettingsTab(this->arbiter, this), "Bluetooth");
    this->addTab(new ActionsSettingsTab(this->arbiter, this), "Actions");
}

MainSettingsTab::MainSettingsTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
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
    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(this->cursor_row_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(this->volume_row_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(this->brightness_plugin_row_widget(), 1);
    layout->addWidget(this->brightness_row_widget(), 1);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(this->controls_row_widget(), 1);

    QScrollArea *scroll_area = new QScrollArea(this);
    Session::Forge::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QWidget *MainSettingsTab::dark_mode_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Dark Mode", widget);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->arbiter.theme().mode == Session::Theme::Dark);
    connect(&this->arbiter, &Arbiter::mode_toggled, [toggle, this](){
        toggle->setChecked(this->arbiter.theme().mode == Session::Theme::Dark);
    });
    connect(toggle, &Switch::stateChanged, [this](bool){ this->arbiter.toggle_mode(); });
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

QWidget *MainSettingsTab::brightness_plugin_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Brightness Plugin", widget);
    layout->addWidget(label, 1);

    layout->addWidget(this->brightness_plugin_select_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::brightness_plugin_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    auto plugins = this->arbiter.system().brightness.plugins();
    Selector *selector = new Selector(plugins, this->arbiter.system().brightness.plugin, this->arbiter.forge().font(14), this->arbiter, widget);
    connect(selector, &Selector::item_changed, [this](QString item){ this->arbiter.set_brightness_plugin(item); });

    layout->addStretch(1);
    layout->addWidget(selector, 10);
    layout->addStretch(1);

    return widget;
}

QWidget *MainSettingsTab::brightness_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Brightness", widget);
    layout->addWidget(label, 1);

    layout->addWidget(this->brightness_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::brightness_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    layout->addStretch(1);
    layout->addWidget(this->arbiter.forge().brightness_slider(), 6);
    layout->addStretch(1);

    return widget;
}

QWidget *MainSettingsTab::color_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Color", widget);
    layout->addWidget(label, 1);

    layout->addWidget(this->color_select_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::color_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    ColorPicker *label = new ColorPicker(this->arbiter, 16, this->arbiter.forge().font(14), widget);
    label->update(this->arbiter.theme().color());
    connect(label, &ColorPicker::color_changed, [this](QColor color){ this->arbiter.set_color(color); });
    connect(&this->arbiter, &Arbiter::color_changed, [label](QColor color){ label->update(color); });

    layout->addWidget(label);

    return widget;
}

QWidget *MainSettingsTab::cursor_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Cursor", widget);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->arbiter.core().cursor);
    connect(toggle, &Switch::stateChanged, [this](bool){
        this->arbiter.toggle_cursor();
    });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *MainSettingsTab::volume_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Volume", widget);
    layout->addWidget(label, 1);

    layout->addWidget(this->volume_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::volume_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    layout->addStretch(1);
    layout->addWidget(this->arbiter.forge().volume_slider(), 6);
    layout->addStretch(1);

    return widget;
}

QWidget *MainSettingsTab::controls_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Controls", widget);
    layout->addWidget(label, 1);

    layout->addWidget(this->controls_widget(), 1);

    return widget;
}

QWidget *MainSettingsTab::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QPushButton *exit_button = new QPushButton("exit", widget);
    connect(exit_button, &QPushButton::clicked, []() { qApp->exit(); });
    layout->addWidget(exit_button);
    layout->addWidget(Session::Forge::br());
    QPushButton *shut_down_button = new QPushButton("shut down", widget);
    connect(shut_down_button, &QPushButton::clicked, [this]{
        this->arbiter.settings().sync();
        sync();
        system(Session::System::SHUTDOWN_CMD);
    });
    layout->addWidget(shut_down_button);
    QPushButton *reboot_button = new QPushButton("reboot", widget);
    connect(reboot_button, &QPushButton::clicked, [this]{
        this->arbiter.settings().sync();
        sync();
        system(Session::System::REBOOT_CMD);
    });
    layout->addWidget(reboot_button);

    return widget;
}

LayoutSettingsTab::LayoutSettingsTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
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
    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(this->control_bar_widget(), 1);

    QWidget *controls_bar_row = this->quick_view_row_widget();
    controls_bar_row->setVisible(this->arbiter.layout().control_bar.enabled);
    connect(&this->arbiter, &Arbiter::control_bar_toggled, [this, controls_bar_row]{
        controls_bar_row->setVisible(this->arbiter.layout().control_bar.enabled);
    });
    layout->addWidget(controls_bar_row, 1);

    layout->addWidget(Session::Forge::br(), 1);
    layout->addWidget(this->scale_row_widget(), 1);

    QScrollArea *scroll_area = new QScrollArea(this);
    Session::Forge::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QWidget *LayoutSettingsTab::pages_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Pages", widget);
    layout->addWidget(label, 1);

    QGroupBox *group = new QGroupBox(widget);
    QVBoxLayout *group_layout = new QVBoxLayout(group);

    for (auto page : this->arbiter.layout().pages()) {
        if (page->toggleale()) {
            QCheckBox *button = new QCheckBox(page->pretty_name(), group);
            button->setChecked(page->enabled());
            connect(button, &QCheckBox::toggled, [this, page]{ this->arbiter.toggle_page(page); });
            group_layout->addWidget(button);
        }
    }

    layout->addWidget(group, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *LayoutSettingsTab::control_bar_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Control Bar", widget);
    layout->addWidget(label, 1);

    Switch *toggle = new Switch(widget);
    toggle->scale(this->arbiter.layout().scale);
    toggle->setChecked(this->arbiter.layout().control_bar.enabled);
    connect(toggle, &Switch::stateChanged, [this](bool){ this->arbiter.toggle_control_bar(); });
    layout->addWidget(toggle, 1, Qt::AlignHCenter);

    return widget;
}

QWidget *LayoutSettingsTab::quick_view_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Quick View", widget);
    layout->addWidget(label, 1);

    layout->addWidget(this->quick_view_select_widget(), 1);

    return widget;
}

QWidget *LayoutSettingsTab::quick_view_select_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QStringList quick_views;
    for (auto quick_view : this->arbiter.layout().control_bar.quick_views())
        quick_views.append(quick_view->pretty_name());
    Selector *selector = new Selector(quick_views, this->arbiter.layout().control_bar.curr_quick_view->pretty_name(), this->arbiter.forge().font(14), this->arbiter, widget);
    connect(selector, &Selector::idx_changed, [this](int idx){
        this->arbiter.set_curr_quick_view(this->arbiter.layout().control_bar.quick_view(idx));
    });

    layout->addStretch(1);
    layout->addWidget(selector, 10);
    layout->addStretch(1);

    return widget;
}

QWidget *LayoutSettingsTab::scale_row_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel("Scale", widget);
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
    slider->setValue(this->arbiter.layout().scale * 4);
    connect(slider, &QSlider::valueChanged, [this, slider](int position){ this->arbiter.set_scale(position / 4.0); });

    QPushButton *lower_button = new QPushButton(widget);
    lower_button->setFlat(true);
    this->arbiter.forge().iconize("remove", lower_button, 32);
    connect(lower_button, &QPushButton::clicked, [slider]{ slider->setValue(slider->value() - 1); });

    QPushButton *raise_button = new QPushButton(widget);
    raise_button->setFlat(true);
    this->arbiter.forge().iconize("add", raise_button, 32);
    connect(raise_button, &QPushButton::clicked, [slider]{ slider->setValue(slider->value() + 1); });

    layout->addStretch(1);
    layout->addWidget(lower_button);
    layout->addWidget(slider, 4);
    layout->addWidget(raise_button);
    layout->addStretch(1);

    return widget;
}

BluetoothSettingsTab::BluetoothSettingsTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
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
    layout->addStretch();
    layout->addWidget(label);

    QLabel *connected_device = new QLabel(this->arbiter.system().bluetooth.get_media_player().first, widget);
    connected_device->setIndent(16);
    connect(&this->arbiter.system().bluetooth, &Bluetooth::media_player_changed, [connected_device](QString name, BluezQt::MediaPlayerPtr){
        connected_device->setText(name);
    });
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
    button->setFlat(true);
    button->setCheckable(true);
    button->setEnabled(this->arbiter.system().bluetooth.has_adapter()); // this could potentially be tricky
    this->arbiter.forge().iconize("bluetooth_searching", button, 36);
    connect(button, &QPushButton::clicked, [this](bool checked){
        if (checked)
            this->arbiter.system().bluetooth.start_scan();
        else
            this->arbiter.system().bluetooth.stop_scan();
    });
    layout->addWidget(button);

    ProgressIndicator *loader = new ProgressIndicator(widget);
    loader->scale(this->arbiter.layout().scale);
    connect(&this->arbiter.system().bluetooth, &Bluetooth::scan_status, [button, loader](bool status){
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

    for (BluezQt::DevicePtr device : this->arbiter.system().bluetooth.get_devices()) {
        QPushButton *button = new QPushButton(device->name(), widget);
        button->setCheckable(true);
        if (device->isConnected())
            button->setChecked(true);
        connect(button, &QPushButton::clicked, [this, button, device](bool checked = false){
            button->setChecked(!checked);
            this->arbiter.set_bluetooth_device(device);
        });

        this->devices[device] = button;
        layout->addWidget(button);
    }
    connect(&this->arbiter.system().bluetooth, &Bluetooth::device_added, [this, layout, widget](BluezQt::DevicePtr device){
        QPushButton *button = new QPushButton(device->name(), widget);
        button->setCheckable(true);
        if (device->isConnected())
            button->setChecked(true);
        connect(button, &QPushButton::clicked, [this, button, device](bool checked = false){
            button->setChecked(!checked);
            this->arbiter.set_bluetooth_device(device);
        });
        this->devices[device] = button;
        layout->addWidget(button);
    });
    connect(&this->arbiter.system().bluetooth, &Bluetooth::device_changed, [this](BluezQt::DevicePtr device){
        this->devices[device]->setText(device->name());
        this->devices[device]->setChecked(device->isConnected());
    });
    connect(&this->arbiter.system().bluetooth, &Bluetooth::device_removed, [this, layout](BluezQt::DevicePtr device){
        layout->removeWidget(devices[device]);
        delete this->devices[device];
        this->devices.remove(device);
    });

    QScrollArea *scroll_area = new QScrollArea(this);
    Session::Forge::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

ActionsSettingsTab::ActionsSettingsTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
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
    Session::Forge::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(widget);

    return scroll_area;
}

QWidget *ActionsSettingsTab::shortcut_row_widget(QString id, QString description, Shortcut *shortcut)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *label = new QLabel(description, widget);

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
    this->arbiter.forge().iconize("keyboard", "developer_board", symbol, 32);

    ShortcutInput *input = new ShortcutInput(shortcut->to_str(), widget);
    input->setFlat(true);
    QFont font(this->arbiter.forge().font(18));
    font.setFamily("Titillium Web");
    input->setFont(font);
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
