#include <unistd.h>
#include <BluezQt/Adapter>
#include <BluezQt/Device>
#include <BluezQt/InitManagerJob>
#include <BluezQt/Manager>
#include <BluezQt/MediaPlayer>
#include <BluezQt/MediaPlayerTrack>
#include <QApplication>
#include <QAbstractSlider>
#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QThread>
#include <QTimer>
#include <iostream>

#include "app/bluetooth.hpp"
#include "app/progress.hpp"

Bluetooth::Bluetooth() : QObject(qApp)
{
    BluezQt::Manager *manager = new BluezQt::Manager();
    BluezQt::InitManagerJob *job = manager->init();
    job->exec();

    this->adapter = manager->usableAdapter().get();

    for (auto device : this->adapter->devices()) {
        if (device->mediaPlayer() != nullptr) {
            this->media_player_device = device;
            break;
        }
    }

    connect(this->adapter, &BluezQt::Adapter::deviceAdded,
            [this](BluezQt::DevicePtr device) { emit device_added(device); });
    connect(this->adapter, &BluezQt::Adapter::deviceChanged, [this](BluezQt::DevicePtr device) {
        emit device_changed(device);
        this->update_media_player(device);
    });
    connect(this->adapter, &BluezQt::Adapter::deviceRemoved,
            [this](BluezQt::DevicePtr device) { emit device_removed(device); });
}

void Bluetooth::scan()
{
    emit scan_status(true);
    this->adapter->startDiscovery();
    QTimer::singleShot(10000, [this]() {
        this->adapter->stopDiscovery();
        emit scan_status(false);
    });
}

void Bluetooth::update_media_player(BluezQt::DevicePtr device)
{
    if (device->mediaPlayer() != nullptr) {
        emit media_player_status_changed(device->mediaPlayer()->status());
        emit media_player_track_changed(device->mediaPlayer()->track());
        emit media_player_changed(device->name(), device->mediaPlayer());
        this->media_player_device = device;
    }
    else if (this->media_player_device == device) {
        emit media_player_status_changed(BluezQt::MediaPlayer::Status::Paused);
        emit media_player_track_changed(BluezQt::MediaPlayerTrack());
        emit media_player_changed(QString(), nullptr);
    }
}

Bluetooth *Bluetooth::get_instance()
{
    static Bluetooth bluetooth;
    return &bluetooth;
}
