#include <QPoint>
#include <QRadioTuner>
#include <QThread>
#include <QtWidgets>

#include <iostream>
#include <regex>
#include <thread>

#include <aasdk_proto/ButtonCodeEnum.pb.h>
#include <aasdk_proto/VideoFPSEnum.pb.h>
#include <aasdk_proto/VideoResolutionEnum.pb.h>
#include <f1x/openauto/autoapp/Configuration/AudioOutputBackendType.hpp>
#include <f1x/openauto/autoapp/Configuration/BluetootAdapterType.hpp>
#include <f1x/openauto/autoapp/Configuration/HandednessOfTrafficType.hpp>

#include <BluezQt/Device>
#include <BluezQt/PendingCall>

#include "app/bluetooth.hpp"
#include "app/config.hpp"
#include "app/progress.hpp"
#include "app/switch.hpp"
#include "app/tabs.hpp"
#include "app/theme.hpp"
#include "app/tuner.hpp"
#include "app/window.hpp"

#include <stdio.h>
#include <stdlib.h>

namespace aasdk = f1x::aasdk;
namespace autoapp = f1x::openauto::autoapp;

QFont f("Montserrat", 18);
QFont ff("Montserrat", 14);
QFont fff("Montserrat", 36);

SettingsTab::SettingsTab(QWidget *parent) : QWidget(parent)
{
    this->app = qobject_cast<MainWindow *>(parent);
    this->config = Config::get_instance();

    this->theme = Theme::get_instance();
    this->bluetooth = Bluetooth::get_instance();

    connect(this->bluetooth, SIGNAL(media_player_changed(QString, BluezQt::MediaPlayerPtr)), this,
            SLOT(media_player_changed(QString, BluezQt::MediaPlayerPtr)));

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(24, 24, 24, 24);
    Switch *dark_mode = new Switch;
    dark_mode->setChecked(this->config->get_dark_mode());
    connect(dark_mode, &Switch::stateChanged, [this](bool state) {
        this->theme->set_mode(state);
        this->config->set_dark_mode(state);
    });
    QLabel *l;
    l = new QLabel("Dark Mode");
    l->setFont(f);
    QHBoxLayout *ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(dark_mode, 1, Qt::AlignHCenter);
    layout->addStretch(1);
    layout->addLayout(ll);
    layout->addStretch(1);
    l = new QLabel("Brightness");
    l->setFont(f);
    QHBoxLayout *bl = new QHBoxLayout;
    QPushButton *bd = new QPushButton;
    bd->setFlat(true);
    bd->setIconSize(QSize(36 * RESOLUTION, 36 * RESOLUTION));
    this->theme->add_button_icon("brightness_low", bd);
    connect(bd, &QPushButton::clicked, this, [this] {
        int position = this->brightness_control->sliderPosition() - 18;
        this->brightness_control->setSliderPosition(position);
        emit brightness_updated(position);
    });
    QPushButton *bu = new QPushButton();
    bu->setFlat(true);
    bu->setIconSize(QSize(36 * RESOLUTION, 36 * RESOLUTION));
    this->theme->add_button_icon("brightness_high", bu);
    connect(bu, &QPushButton::clicked, this, [this] {
        int position = this->brightness_control->sliderPosition() + 18;
        this->brightness_control->setSliderPosition(position);
        emit brightness_updated(position);
    });
    this->brightness_control = new QSlider(Qt::Orientation::Horizontal);
    this->brightness_control->setRange(76, 255);
    this->brightness_control->setSliderPosition(this->config->get_brightness());
    connect(this->brightness_control, &QSlider::sliderMoved,
            [this](int position) { emit brightness_updated(position); });
    connect(this->brightness_control, &QSlider::valueChanged,
            [this](int position) { this->config->set_brightness(position); });
    bl->addStretch(1);
    bl->addWidget(bd);
    bl->addWidget(this->brightness_control, 6);
    bl->addWidget(bu);
    bl->addStretch(1);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addLayout(bl, 1);
    layout->addLayout(ll);
    layout->addStretch(1);
    Switch *units = new Switch;
    units->setChecked(this->config->get_si_units());
    connect(units, &Switch::stateChanged, [this](bool state) {
        emit si_units_changed(state);
        this->config->set_si_units(state);
    });
    l = new QLabel("SI Units");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(units, 1, Qt::AlignHCenter);
    layout->addLayout(ll);
    layout->addStretch(1);
    l = new QLabel("Color");
    l->setFont(f);
    QComboBox *box = new QComboBox;
    box->setItemDelegate(new QStyledItemDelegate());
    box->setFont(ff);

    auto colors = this->theme->get_colors();
    QMap<QString, QColor>::iterator i;
    QPixmap pixmap(16, 16);
    for (i = colors.begin(); i != colors.end(); i++) {
        pixmap.fill(i.value());
        box->addItem(QIcon(pixmap), i.key());
    }
    box->setCurrentText(this->config->get_color());

    connect(box, QOverload<const QString &>::of(&QComboBox::activated), [this](const QString &color) {
        this->theme->set_color(color);
        this->config->set_color(color);
    });
    connect(this->theme, &Theme::color_updated, [box](QMap<QString, QColor> &colors) {
        QMap<QString, QColor>::iterator i;
        QPixmap pixmap(16, 16);
        int idx;
        for (i = colors.begin(); i != colors.end(); i++) {
            idx = box->findText(i.key());
            pixmap.fill(i.value());
            box->setItemIcon(idx, QIcon(pixmap));
        }
    });

    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    bl = new QHBoxLayout;
    bl->addStretch(1);
    bl->addWidget(box, 6);
    bl->addStretch(1);
    ll->addLayout(bl, 1);
    layout->addLayout(ll);
    layout->addStretch(1);
    QPushButton *save = new QPushButton("save");
    save->setFont(f);
    save->setFlat(true);
    save->setIconSize(QSize(48 * RESOLUTION, 48 * RESOLUTION));
    this->theme->add_button_icon("save", save);
    connect(save, &QPushButton::clicked, [this]() { this->config->save(); });
    ll = new QHBoxLayout;
    ll->addStretch(1);
    ll->addWidget(save);
    layout->addLayout(ll);
    tab->setLayout(layout);

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->tabBar()->setFont(f);
    tabWidget->addTab(tab, "General");

    QWidget *bw = new QWidget;
    QHBoxLayout *holder = new QHBoxLayout;
    holder->setContentsMargins(24, 24, 24, 24);
    QVBoxLayout *left_layout = new QVBoxLayout;
    QHBoxLayout *lleft_layout = new QHBoxLayout;
    QPushButton *scan = new QPushButton("scan");
    scan->setFont(f);
    scan->setFlat(true);
    scan->setIconSize(QSize(48 * RESOLUTION, 48 * RESOLUTION));
    this->theme->add_button_icon("bluetooth_searching", scan);
    connect(scan, &QPushButton::clicked, [this]() { this->bluetooth->scan(); });
    lleft_layout->addWidget(scan, 0, Qt::AlignLeft);
    ProgressIndicator *waiting = new ProgressIndicator;
    connect(this->bluetooth, &Bluetooth::scan_status, [scan, waiting](bool status) {
        scan->setEnabled(!status);
        if (status)
            waiting->startAnimation();
        else
            waiting->stopAnimation();
    });
    lleft_layout->addWidget(waiting);
    QLabel *mp = new QLabel("Media Player");
    mp->setFont(f);
    auto media_player_name = this->bluetooth->get_media_player().first;
    this->media_player = new QLabel(media_player_name.isEmpty() ? "not connected" : media_player_name);
    this->media_player->setStyleSheet("padding-left: 16px;");
    this->media_player->setFont(ff);
    left_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    left_layout->addWidget(mp);
    left_layout->addWidget(this->media_player);
    left_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    left_layout->addLayout(lleft_layout);
    this->bluetooth_devices = new QVBoxLayout;
    for (auto device : this->bluetooth->get_devices()) {
        if (device->address() == this->config->get_bluetooth_device()) device->connectToDevice()->waitForFinished();
        QPushButton *device_button = new QPushButton(device->name());
        device_button->setFont(f);
        device_button->setCheckable(true);
        if (device->isConnected()) device_button->setChecked(true);
        connect(device_button, &QPushButton::clicked, [this, device_button, device](bool checked = false) {
            device_button->setChecked(!checked);
            if (checked) {
                device->connectToDevice()->waitForFinished();
                this->config->set_bluetooth_device(device->address());
            }
            else {
                device->disconnectFromDevice()->waitForFinished();
                this->config->set_bluetooth_device(QString());
            }
        });

        this->bluetooth_device_buttons[device] = device_button;
        this->bluetooth_devices->addWidget(device_button);
    };
    holder->addLayout(left_layout);
    holder->addLayout(this->bluetooth_devices);
    bw->setLayout(holder);

    QWidget *oaw = new QWidget;
    layout = new QVBoxLayout;
    layout->setContentsMargins(24, 24, 24, 24);
    Switch *handedness = new Switch;
    handedness->setChecked(this->config->open_auto_config->getHandednessOfTrafficType() ==
                           autoapp::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE);
    connect(handedness, &Switch::stateChanged, [this](bool state) {
        this->config->open_auto_config->setHandednessOfTrafficType(
            state ? autoapp::configuration::HandednessOfTrafficType::RIGHT_HAND_DRIVE
                  : autoapp::configuration::HandednessOfTrafficType::LEFT_HAND_DRIVE);
    });
    l = new QLabel("Right-Hand-Drive");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(handedness, 1, Qt::AlignHCenter);
    layout->addLayout(ll);
    QFrame *line = new QFrame;
    line->setLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    layout->addWidget(line);
    l = new QLabel("Frame Rate");
    l->setFont(f);
    QGroupBox *groupBox = new QGroupBox;
    QRadioButton *radio1 = new QRadioButton("30fps", groupBox);
    radio1->setFont(ff);
    radio1->setChecked(this->config->open_auto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    connect(radio1, &QRadioButton::clicked,
            [this]() { this->config->open_auto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_30); });
    QRadioButton *radio2 = new QRadioButton("60fps", groupBox);
    radio2->setFont(ff);
    radio2->setChecked(this->config->open_auto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_60);
    radio1->setChecked(this->config->open_auto_config->getVideoFPS() == aasdk::proto::enums::VideoFPS::_30);
    connect(radio2, &QRadioButton::clicked,
            [this]() { this->config->open_auto_config->setVideoFPS(aasdk::proto::enums::VideoFPS::_60); });
    QVBoxLayout *lll = new QVBoxLayout;
    lll->addWidget(radio1);
    lll->addWidget(radio2);
    groupBox->setLayout(lll);
    ll = new QHBoxLayout;
    ll->addWidget(l);
    ll->addWidget(groupBox, 0, Qt::AlignHCenter);
    layout->addLayout(ll);
    l = new QLabel("Resolution");
    l->setFont(f);
    groupBox = new QGroupBox;
    radio1 = new QRadioButton("480p", groupBox);
    radio1->setFont(ff);
    radio1->setChecked(this->config->open_auto_config->getVideoResolution() ==
                       aasdk::proto::enums::VideoResolution::_480p);
    connect(radio1, &QRadioButton::clicked, [this]() {
        this->config->open_auto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_480p);
    });
    radio2 = new QRadioButton("720p", groupBox);
    radio2->setFont(ff);
    radio2->setChecked(this->config->open_auto_config->getVideoResolution() ==
                       aasdk::proto::enums::VideoResolution::_720p);
    connect(radio2, &QRadioButton::clicked, [this]() {
        this->config->open_auto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_720p);
    });
    QRadioButton *radio3 = new QRadioButton("1080p", groupBox);
    radio3->setFont(ff);
    radio3->setChecked(this->config->open_auto_config->getVideoResolution() ==
                       aasdk::proto::enums::VideoResolution::_1080p);
    connect(radio3, &QRadioButton::clicked, [this]() {
        this->config->open_auto_config->setVideoResolution(aasdk::proto::enums::VideoResolution::_1080p);
    });
    lll = new QVBoxLayout;
    lll->addWidget(radio1);
    lll->addWidget(radio2);
    lll->addWidget(radio3);
    groupBox->setLayout(lll);
    ll = new QHBoxLayout;
    ll->addWidget(l);
    ll->addWidget(groupBox, 0, Qt::AlignHCenter);
    layout->addLayout(ll);
    l = new QLabel("DPI");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    bl = new QHBoxLayout;
    QSlider *dpi = new QSlider(Qt::Orientation::Horizontal);
    dpi->setRange(0, 512);
    dpi->setSliderPosition(this->config->open_auto_config->getScreenDPI());
    bl->addStretch(1);
    bl->addWidget(dpi, 4);
    l = new QLabel(QString::number(dpi->sliderPosition()));
    l->setFont(ff);
    bl->addWidget(l, 1, Qt::AlignHCenter);
    bl->addStretch(1);
    ll->addLayout(bl, 1);
    layout->addLayout(ll);
    connect(dpi, &QSlider::valueChanged, [this, l](int value) {
        l->setText(QString::number(value));
        this->config->open_auto_config->setScreenDPI(value);
    });
    line = new QFrame;
    line->setLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    layout->addWidget(line);
    Switch *audio_backend = new Switch;
    audio_backend->setChecked(this->config->open_auto_config->getAudioOutputBackendType() ==
                              autoapp::configuration::AudioOutputBackendType::RTAUDIO);
    connect(audio_backend, &Switch::stateChanged, [this](bool state) {
        this->config->open_auto_config->setAudioOutputBackendType(
            state ? autoapp::configuration::AudioOutputBackendType::RTAUDIO
                  : autoapp::configuration::AudioOutputBackendType::QT);
    });
    l = new QLabel("RtAudio");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(audio_backend, 1, Qt::AlignHCenter);
    layout->addLayout(ll);
    l = new QLabel("Audio Channels");
    l->setFont(f);
    groupBox = new QGroupBox;
    QCheckBox *checkBox1 = new QCheckBox("Music");
    checkBox1->setFont(ff);
    checkBox1->setChecked(this->config->open_auto_config->musicAudioChannelEnabled());
    connect(checkBox1, &QCheckBox::toggled,
            [this](bool checked) { this->config->open_auto_config->setMusicAudioChannelEnabled(checked); });
    QCheckBox *checkBox2 = new QCheckBox("Speech");
    checkBox2->setFont(ff);
    checkBox2->setChecked(this->config->open_auto_config->speechAudioChannelEnabled());
    connect(checkBox2, &QCheckBox::toggled,
            [this](bool checked) { this->config->open_auto_config->setSpeechAudioChannelEnabled(checked); });
    lll = new QVBoxLayout;
    lll->addWidget(checkBox1);
    lll->addWidget(checkBox2);
    groupBox->setLayout(lll);
    ll = new QHBoxLayout;
    ll->addWidget(l);
    ll->addWidget(groupBox, 0, Qt::AlignHCenter);
    layout->addLayout(ll);
    line = new QFrame;
    line->setLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    layout->addWidget(line);
    Switch *bluetooth = new Switch;
    bluetooth->setChecked(this->config->open_auto_config->getBluetoothAdapterType() ==
                          autoapp::configuration::BluetoothAdapterType::LOCAL);
    connect(bluetooth, &Switch::stateChanged, [this](bool state) {
        this->config->open_auto_config->setBluetoothAdapterType(
            state ? autoapp::configuration::BluetoothAdapterType::LOCAL
                  : autoapp::configuration::BluetoothAdapterType::NONE);
    });
    l = new QLabel("Bluetooth");
    l->setFont(f);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    ll->addWidget(bluetooth, 1, Qt::AlignHCenter);
    layout->addLayout(ll);
    /* l = new QLabel("Wireless");
    l->setFont(f);
    box = new QComboBox;
    box->setItemDelegate(new QStyledItemDelegate());
    box->setFont(ff);
    ll = new QHBoxLayout;
    ll->addWidget(l, 1);
    bl = new QHBoxLayout;
    bl->addStretch(1);
    bl->addWidget(box, 6);
    bl->addStretch(1);
    ll->addLayout(bl, 1);
    layout->addLayout(ll); */
    oaw->setLayout(layout);

    connect(this->bluetooth, SIGNAL(device_added(BluezQt::DevicePtr)), this,
            SLOT(bluetooth_device_added(BluezQt::DevicePtr)));
    connect(this->bluetooth, SIGNAL(device_changed(BluezQt::DevicePtr)), this,
            SLOT(bluetooth_device_changed(BluezQt::DevicePtr)));
    connect(this->bluetooth, SIGNAL(device_removed(BluezQt::DevicePtr)), this,
            SLOT(bluetooth_device_removed(BluezQt::DevicePtr)));

    tabWidget->addTab(bw, "Bluetooth");
    tabWidget->addTab(oaw, "OpenAuto");
    main_layout->addWidget(tabWidget);

    setLayout(main_layout);
}

void SettingsTab::bluetooth_device_added(BluezQt::DevicePtr device)
{
    QPushButton *device_button = new QPushButton(device->name());
    device_button->setFont(f);
    device_button->setCheckable(true);
    if (device->isConnected()) device_button->setChecked(true);
    connect(device_button, &QPushButton::clicked, [device_button, device](bool checked = false) {
        device_button->setChecked(!checked);
        if (checked)
            device->connectToDevice()->waitForFinished();
        else
            device->disconnectFromDevice()->waitForFinished();
    });
    this->bluetooth_device_buttons[device] = device_button;
    this->bluetooth_devices->addWidget(device_button);
}

void SettingsTab::bluetooth_device_changed(BluezQt::DevicePtr device)
{
    this->bluetooth_device_buttons[device]->setText(device->name());
    this->bluetooth_device_buttons[device]->setChecked(device->isConnected());
}

void SettingsTab::bluetooth_device_removed(BluezQt::DevicePtr device)
{
    this->bluetooth_devices->removeWidget(this->bluetooth_device_buttons[device]);
    delete this->bluetooth_device_buttons[device];
    this->bluetooth_device_buttons.remove(device);
}

void SettingsTab::media_player_changed(QString name, BluezQt::MediaPlayerPtr)
{
    this->media_player->setText(name.isEmpty() ? "not connected" : name);
}
