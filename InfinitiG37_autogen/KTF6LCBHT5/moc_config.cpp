/****************************************************************************
** Meta object code from reading C++ file 'config.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "app/config.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'config.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Config_t {
    QByteArrayData data[20];
    char stringdata0[224];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Config_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Config_t qt_meta_stringdata_Config = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Config"
QT_MOC_LITERAL(1, 7, 14), // "volume_changed"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 6), // "volume"
QT_MOC_LITERAL(4, 30, 18), // "brightness_changed"
QT_MOC_LITERAL(5, 49, 10), // "brightness"
QT_MOC_LITERAL(6, 60, 16), // "si_units_changed"
QT_MOC_LITERAL(7, 77, 8), // "si_units"
QT_MOC_LITERAL(8, 86, 18), // "quick_view_changed"
QT_MOC_LITERAL(9, 105, 10), // "quick_view"
QT_MOC_LITERAL(10, 116, 20), // "controls_bar_changed"
QT_MOC_LITERAL(11, 137, 12), // "controls_bar"
QT_MOC_LITERAL(12, 150, 13), // "scale_changed"
QT_MOC_LITERAL(13, 164, 5), // "scale"
QT_MOC_LITERAL(14, 170, 12), // "page_changed"
QT_MOC_LITERAL(15, 183, 8), // "QWidget*"
QT_MOC_LITERAL(16, 192, 4), // "page"
QT_MOC_LITERAL(17, 197, 7), // "enabled"
QT_MOC_LITERAL(18, 205, 11), // "save_status"
QT_MOC_LITERAL(19, 217, 6) // "status"

    },
    "Config\0volume_changed\0\0volume\0"
    "brightness_changed\0brightness\0"
    "si_units_changed\0si_units\0quick_view_changed\0"
    "quick_view\0controls_bar_changed\0"
    "controls_bar\0scale_changed\0scale\0"
    "page_changed\0QWidget*\0page\0enabled\0"
    "save_status\0status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Config[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,
       6,    1,   60,    2, 0x06 /* Public */,
       8,    1,   63,    2, 0x06 /* Public */,
      10,    1,   66,    2, 0x06 /* Public */,
      12,    1,   69,    2, 0x06 /* Public */,
      14,    2,   72,    2, 0x06 /* Public */,
      18,    1,   77,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::UInt,    5,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, 0x80000000 | 15, QMetaType::Bool,   16,   17,
    QMetaType::Void, QMetaType::Bool,   19,

       0        // eod
};

void Config::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Config *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->volume_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->brightness_changed((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 2: _t->si_units_changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->quick_view_changed((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->controls_bar_changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->scale_changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->page_changed((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->save_status((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Config::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Config::volume_changed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Config::*)(unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Config::brightness_changed)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Config::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Config::si_units_changed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Config::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Config::quick_view_changed)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Config::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Config::controls_bar_changed)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Config::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Config::scale_changed)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Config::*)(QWidget * , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Config::page_changed)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Config::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Config::save_status)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Config::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Config.data,
    qt_meta_data_Config,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Config::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Config::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Config.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Config::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Config::volume_changed(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Config::brightness_changed(unsigned int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Config::si_units_changed(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Config::quick_view_changed(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Config::controls_bar_changed(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Config::scale_changed(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Config::page_changed(QWidget * _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Config::save_status(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
