#pragma once

#include <BluezQt/Adapter>
#include <BluezQt/Device>
#include <BluezQt/MediaPlayer>
#include <BluezQt/MediaPlayerTrack>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QPushButton>
#include <QString>

#include "app/widgets/progress.hpp"

class Bluetooth : public QObject {
    Q_OBJECT

   public:
    Bluetooth();

    void start_scan();
    void stop_scan();

    inline QList<BluezQt::DevicePtr> get_devices()
    {
        return this->has_adapter() ? this->adapter->devices() : QList<BluezQt::DevicePtr>();
    }
    inline QPair<QString, BluezQt::MediaPlayerPtr> get_media_player()
    {
        if (this->media_player_device != nullptr)
            return {this->media_player_device->name(), this->media_player_device->mediaPlayer()};

        return {QString(), QSharedPointer<BluezQt::MediaPlayer>(nullptr)};
    }
    inline bool has_adapter() { return this->adapter != nullptr; }

    static Bluetooth *get_instance();

   private:
    void update_media_player(BluezQt::DevicePtr device);

    BluezQt::AdapterPtr adapter;
    BluezQt::DevicePtr media_player_device;
    QTimer *scan_timer;

   signals:
    void device_added(BluezQt::DevicePtr);
    void device_changed(BluezQt::DevicePtr);
    void device_removed(BluezQt::DevicePtr);
    void media_player_changed(QString, BluezQt::MediaPlayerPtr);
    void media_player_status_changed(BluezQt::MediaPlayer::Status);
    void media_player_track_changed(BluezQt::MediaPlayerTrack);
    void scan_status(bool);
};
