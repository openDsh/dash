/****************************************************************************
** Meta object code from reading C++ file 'theme.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/app/theme.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'theme.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Theme_t {
    QByteArrayData data[11];
    char stringdata0[123];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Theme_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Theme_t qt_meta_stringdata_Theme = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Theme"
QT_MOC_LITERAL(1, 6, 12), // "mode_updated"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 4), // "mode"
QT_MOC_LITERAL(4, 25, 13), // "icons_updated"
QT_MOC_LITERAL(5, 39, 18), // "QList<tab_icon_t>&"
QT_MOC_LITERAL(6, 58, 9), // "tab_icons"
QT_MOC_LITERAL(7, 68, 21), // "QList<button_icon_t>&"
QT_MOC_LITERAL(8, 90, 12), // "button_icons"
QT_MOC_LITERAL(9, 103, 5), // "scale"
QT_MOC_LITERAL(10, 109, 13) // "color_updated"

    },
    "Theme\0mode_updated\0\0mode\0icons_updated\0"
    "QList<tab_icon_t>&\0tab_icons\0"
    "QList<button_icon_t>&\0button_icons\0"
    "scale\0color_updated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Theme[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    3,   32,    2, 0x06 /* Public */,
      10,    0,   39,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 7, QMetaType::Double,    6,    8,    9,
    QMetaType::Void,

       0        // eod
};

void Theme::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Theme *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mode_updated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->icons_updated((*reinterpret_cast< QList<tab_icon_t>(*)>(_a[1])),(*reinterpret_cast< QList<button_icon_t>(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 2: _t->color_updated(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Theme::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Theme::mode_updated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Theme::*)(QList<tab_icon_t> & , QList<button_icon_t> & , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Theme::icons_updated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Theme::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Theme::color_updated)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Theme::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Theme.data,
    qt_meta_data_Theme,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Theme::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Theme::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Theme.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Theme::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Theme::mode_updated(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Theme::icons_updated(QList<tab_icon_t> & _t1, QList<button_icon_t> & _t2, double _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Theme::color_updated()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
