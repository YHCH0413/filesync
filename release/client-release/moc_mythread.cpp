/****************************************************************************
** Meta object code from reading C++ file 'mythread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../filesync/mythread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mythread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_mythread_t {
    QByteArrayData data[8];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mythread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mythread_t qt_meta_stringdata_mythread = {
    {
QT_MOC_LITERAL(0, 0, 8), // "mythread"
QT_MOC_LITERAL(1, 9, 9), // "stoptimer"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 7), // "getsize"
QT_MOC_LITERAL(4, 28, 8), // "sendover"
QT_MOC_LITERAL(5, 37, 9), // "refrashpb"
QT_MOC_LITERAL(6, 47, 11), // "delelastone"
QT_MOC_LITERAL(7, 59, 12) // "clearextrdir"

    },
    "mythread\0stoptimer\0\0getsize\0sendover\0"
    "refrashpb\0delelastone\0clearextrdir"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mythread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    1,   45,    2, 0x06 /* Public */,
       4,    0,   48,    2, 0x06 /* Public */,
       5,    0,   49,    2, 0x06 /* Public */,
       6,    0,   50,    2, 0x06 /* Public */,
       7,    0,   51,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void mythread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        mythread *_t = static_cast<mythread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stoptimer(); break;
        case 1: _t->getsize((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 2: _t->sendover(); break;
        case 3: _t->refrashpb(); break;
        case 4: _t->delelastone(); break;
        case 5: _t->clearextrdir(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (mythread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mythread::stoptimer)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (mythread::*_t)(qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mythread::getsize)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (mythread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mythread::sendover)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (mythread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mythread::refrashpb)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (mythread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mythread::delelastone)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (mythread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mythread::clearextrdir)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject mythread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_mythread.data,
      qt_meta_data_mythread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *mythread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mythread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_mythread.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int mythread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void mythread::stoptimer()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void mythread::getsize(qint64 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void mythread::sendover()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void mythread::refrashpb()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void mythread::delelastone()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void mythread::clearextrdir()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
