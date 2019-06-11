/****************************************************************************
** Meta object code from reading C++ file 'transfile.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../filesync-server/transfile.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'transfile.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_transfile_t {
    QByteArrayData data[6];
    char stringdata0[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_transfile_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_transfile_t qt_meta_stringdata_transfile = {
    {
QT_MOC_LITERAL(0, 0, 9), // "transfile"
QT_MOC_LITERAL(1, 10, 9), // "stoptimer"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 11), // "startreport"
QT_MOC_LITERAL(4, 33, 10), // "stopreport"
QT_MOC_LITERAL(5, 44, 12) // "clearextrdir"

    },
    "transfile\0stoptimer\0\0startreport\0"
    "stopreport\0clearextrdir"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_transfile[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,
       4,    0,   36,    2, 0x06 /* Public */,
       5,    0,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void transfile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        transfile *_t = static_cast<transfile *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stoptimer(); break;
        case 1: _t->startreport(); break;
        case 2: _t->stopreport(); break;
        case 3: _t->clearextrdir(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (transfile::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&transfile::stoptimer)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (transfile::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&transfile::startreport)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (transfile::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&transfile::stopreport)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (transfile::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&transfile::clearextrdir)) {
                *result = 3;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject transfile::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_transfile.data,
      qt_meta_data_transfile,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *transfile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *transfile::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_transfile.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int transfile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void transfile::stoptimer()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void transfile::startreport()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void transfile::stopreport()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void transfile::clearextrdir()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
