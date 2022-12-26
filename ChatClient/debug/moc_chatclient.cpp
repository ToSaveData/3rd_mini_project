/****************************************************************************
** Meta object code from reading C++ file 'chatclient.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../chatclient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chatclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ChatClient_t {
    const uint offsetsAndSize[22];
    char stringdata0[114];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ChatClient_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ChatClient_t qt_meta_stringdata_ChatClient = {
    {
QT_MOC_LITERAL(0, 10), // "ChatClient"
QT_MOC_LITERAL(11, 20), // "connectButtonClicked"
QT_MOC_LITERAL(32, 0), // ""
QT_MOC_LITERAL(33, 11), // "receiveData"
QT_MOC_LITERAL(45, 8), // "sendData"
QT_MOC_LITERAL(54, 10), // "disconnect"
QT_MOC_LITERAL(65, 12), // "sendProtocol"
QT_MOC_LITERAL(78, 11), // "Chat_Status"
QT_MOC_LITERAL(90, 5), // "char*"
QT_MOC_LITERAL(96, 8), // "sendFile"
QT_MOC_LITERAL(105, 8) // "goOnSend"

    },
    "ChatClient\0connectButtonClicked\0\0"
    "receiveData\0sendData\0disconnect\0"
    "sendProtocol\0Chat_Status\0char*\0sendFile\0"
    "goOnSend"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ChatClient[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x08,    1 /* Private */,
       3,    0,   63,    2, 0x08,    2 /* Private */,
       4,    0,   64,    2, 0x08,    3 /* Private */,
       5,    0,   65,    2, 0x08,    4 /* Private */,
       6,    3,   66,    2, 0x08,    5 /* Private */,
       6,    2,   73,    2, 0x28,    9 /* Private | MethodCloned */,
       9,    0,   78,    2, 0x08,   12 /* Private */,
      10,    1,   79,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 8, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 8,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    2,

       0        // eod
};

void ChatClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ChatClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connectButtonClicked(); break;
        case 1: _t->receiveData(); break;
        case 2: _t->sendData(); break;
        case 3: _t->disconnect(); break;
        case 4: _t->sendProtocol((*reinterpret_cast< std::add_pointer_t<Chat_Status>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<char*>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 5: _t->sendProtocol((*reinterpret_cast< std::add_pointer_t<Chat_Status>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<char*>>(_a[2]))); break;
        case 6: _t->sendFile(); break;
        case 7: _t->goOnSend((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ChatClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ChatClient.offsetsAndSize,
    qt_meta_data_ChatClient,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ChatClient_t
, QtPrivate::TypeAndForceComplete<ChatClient, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Chat_Status, std::false_type>, QtPrivate::TypeAndForceComplete<char *, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Chat_Status, std::false_type>, QtPrivate::TypeAndForceComplete<char *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>


>,
    nullptr
} };


const QMetaObject *ChatClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChatClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ChatClient.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ChatClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
