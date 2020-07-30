/****************************************************************************
** Meta object code from reading C++ file 'bluetooth.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "app/bluetooth.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bluetooth.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Bluetooth_t {
    QByteArrayData data[13];
    char stringdata0[240];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Bluetooth_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Bluetooth_t qt_meta_stringdata_Bluetooth = {
    {
QT_MOC_LITERAL(0, 0, 9), // "Bluetooth"
QT_MOC_LITERAL(1, 10, 12), // "device_added"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 18), // "BluezQt::DevicePtr"
QT_MOC_LITERAL(4, 43, 14), // "device_changed"
QT_MOC_LITERAL(5, 58, 14), // "device_removed"
QT_MOC_LITERAL(6, 73, 20), // "media_player_changed"
QT_MOC_LITERAL(7, 94, 23), // "BluezQt::MediaPlayerPtr"
QT_MOC_LITERAL(8, 118, 27), // "media_player_status_changed"
QT_MOC_LITERAL(9, 146, 28), // "BluezQt::MediaPlayer::Status"
QT_MOC_LITERAL(10, 175, 26), // "media_player_track_changed"
QT_MOC_LITERAL(11, 202, 25), // "BluezQt::MediaPlayerTrack"
QT_MOC_LITERAL(12, 228, 11) // "scan_status"

    },
    "Bluetooth\0device_added\0\0BluezQt::DevicePtr\0"
    "device_changed\0device_removed\0"
    "media_player_changed\0BluezQt::MediaPlayerPtr\0"
    "media_player_status_changed\0"
    "BluezQt::MediaPlayer::Status\0"
    "media_player_track_changed\0"
    "BluezQt::MediaPlayerTrack\0scan_status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Bluetooth[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       5,    1,   55,    2, 0x06 /* Public */,
       6,    2,   58,    2, 0x06 /* Public */,
       8,    1,   63,    2, 0x06 /* Public */,
      10,    1,   66,    2, 0x06 /* Public */,
      12,    1,   69,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 7,    2,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void Bluetooth::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Bluetooth *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->device_added((*reinterpret_cast< BluezQt::DevicePtr(*)>(_a[1]))); break;
        case 1: _t->device_changed((*reinterpret_cast< BluezQt::DevicePtr(*)>(_a[1]))); break;
        case 2: _t->device_removed((*reinterpret_cast< BluezQt::DevicePtr(*)>(_a[1]))); break;
        case 3: _t->media_player_changed((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< BluezQt::MediaPlayerPtr(*)>(_a[2]))); break;
        case 4: _t->media_player_status_changed((*reinterpret_cast< BluezQt::MediaPlayer::Status(*)>(_a[1]))); break;
        case 5: _t->media_player_track_changed((*reinterpret_cast< BluezQt::MediaPlayerTrack(*)>(_a[1]))); break;
        case 6: _t->scan_status((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BluezQt::MediaPlayerTrack >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Bluetooth::*)(BluezQt::DevicePtr );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bluetooth::device_added)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Bluetooth::*)(BluezQt::DevicePtr );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bluetooth::device_changed)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Bluetooth::*)(BluezQt::DevicePtr );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bluetooth::device_removed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Bluetooth::*)(QString , BluezQt::MediaPlayerPtr );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bluetooth::media_player_changed)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Bluetooth::*)(BluezQt::MediaPlayer::Status );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bluetooth::media_player_status_changed)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Bluetooth::*)(BluezQt::MediaPlayerTrack );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bluetooth::media_player_track_changed)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Bluetooth::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bluetooth::scan_status)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Bluetooth::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Bluetooth.data,
    qt_meta_data_Bluetooth,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Bluetooth::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Bluetooth::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Bluetooth.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Bluetooth::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Bluetooth::device_added(BluezQt::DevicePtr _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Bluetooth::device_changed(BluezQt::DevicePtr _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Bluetooth::device_removed(BluezQt::DevicePtr _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Bluetooth::media_player_changed(QString _t1, BluezQt::MediaPlayerPtr _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Bluetooth::media_player_status_changed(BluezQt::MediaPlayer::Status _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Bluetooth::media_player_track_changed(BluezQt::MediaPlayerTrack _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Bluetooth::scan_status(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
