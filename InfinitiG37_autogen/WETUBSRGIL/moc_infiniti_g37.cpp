/****************************************************************************
** Meta object code from reading C++ file 'infiniti_g37.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../plugins/infiniti_g37.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'infiniti_g37.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_InfinitiG37_t {
    QByteArrayData data[1];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_InfinitiG37_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_InfinitiG37_t qt_meta_stringdata_InfinitiG37 = {
    {
QT_MOC_LITERAL(0, 0, 11) // "InfinitiG37"

    },
    "InfinitiG37"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_InfinitiG37[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void InfinitiG37::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject InfinitiG37::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_InfinitiG37.data,
    qt_meta_data_InfinitiG37,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *InfinitiG37::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *InfinitiG37::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_InfinitiG37.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "VehicleInterface"))
        return static_cast< VehicleInterface*>(this);
    if (!strcmp(_clname, "org.dash.canbus.VehicleInterface"))
        return static_cast< VehicleInterface*>(this);
    return QObject::qt_metacast(_clname);
}

int InfinitiG37::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}

QT_PLUGIN_METADATA_SECTION
static constexpr unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x20,  'o',  'r',  'g',  '.',  'd', 
    'a',  's',  'h',  '.',  'c',  'a',  'n',  'b', 
    'u',  's',  '.',  'V',  'e',  'h',  'i',  'c', 
    'l',  'e',  'I',  'n',  't',  'e',  'r',  'f', 
    'a',  'c',  'e', 
    // "className"
    0x03,  0x6b,  'I',  'n',  'f',  'i',  'n',  'i', 
    't',  'i',  'G',  '3',  '7', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(InfinitiG37, InfinitiG37)

QT_WARNING_POP
QT_END_MOC_NAMESPACE
