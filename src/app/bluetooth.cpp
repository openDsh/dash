#include <unistd.h>
#include <BluezQt/Adapter>
#include <BluezQt/Device>
#include <BluezQt/InitManagerJob>
#include <BluezQt/Manager>
#include <BluezQt/MediaPlayer>
#include <BluezQt/MediaPlayerTrack>
#include <QAbstractSlider>
#include <QApplication>
#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QTimer>

#include <app/bluetooth.hpp>
#include <app/widgets/progress.hpp>

Bluetooth::Bluetooth() : QObject(qApp)
{
    BluezQt::Manager *manager = new BluezQt::Manager();
    BluezQt::InitManagerJob *job = manager->init();
    job->exec();

    this->adapter = manager->usableAdapter();

    this->scan_timer = new QTimer(this);
    this->scan_timer->setSingleShot(true);
    connect(this->scan_timer, &QTimer::timeout, [this]() { this->stop_scan(); });

    if (this->has_adapter()) {
        for (auto device : this->get_devices()) {
            if (device->mediaPlayer() != nullptr) {
                this->media_player_device = device;
                break;
            }
        }

        connect(this->adapter.data(), &BluezQt::Adapter::deviceAdded,
                [this](BluezQt::DevicePtr device) { emit device_added(device); });
        connect(this->adapter.data(), &BluezQt::Adapter::deviceChanged, [this](BluezQt::DevicePtr device) {
            emit device_changed(device);
            this->update_media_player(device);
        });
        connect(this->adapter.data(), &BluezQt::Adapter::deviceRemoved,
                [this](BluezQt::DevicePtr device) { emit device_removed(device); });
    }
}

void Bluetooth::start_scan()
{
    if (this->has_adapter()) {
        if (!this->adapter->isDiscovering()) {
            emit scan_status(true);
            this->adapter->startDiscovery();
            this->scan_timer->start(15000);
        }
    }
}

void Bluetooth::stop_scan()
{
    if (this->has_adapter()) {
        if (this->adapter->isDiscovering()) {
            emit scan_status(false);
            this->adapter->stopDiscovery();
        }
    }
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
        emit media_player_changed(QString(), QSharedPointer<BluezQt::MediaPlayer>(nullptr));
    }
}

Bluetooth *Bluetooth::get_instance()
{
    static Bluetooth bluetooth;
    return &bluetooth;
}
