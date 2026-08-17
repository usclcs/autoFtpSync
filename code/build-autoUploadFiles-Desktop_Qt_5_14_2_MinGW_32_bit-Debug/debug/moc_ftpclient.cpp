/****************************************************************************
** Meta object code from reading C++ file 'ftpclient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../autoFtpSync/ftpclient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ftpclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FtpClient_t {
    QByteArrayData data[24];
    char stringdata0[307];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FtpClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FtpClient_t qt_meta_stringdata_FtpClient = {
    {
QT_MOC_LITERAL(0, 0, 9), // "FtpClient"
QT_MOC_LITERAL(1, 10, 10), // "logMessage"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 5), // "level"
QT_MOC_LITERAL(4, 28, 6), // "source"
QT_MOC_LITERAL(5, 35, 3), // "msg"
QT_MOC_LITERAL(6, 39, 15), // "onCtrlConnected"
QT_MOC_LITERAL(7, 55, 15), // "onCtrlEncrypted"
QT_MOC_LITERAL(8, 71, 15), // "onCtrlSslErrors"
QT_MOC_LITERAL(9, 87, 16), // "QList<QSslError>"
QT_MOC_LITERAL(10, 104, 6), // "errors"
QT_MOC_LITERAL(11, 111, 15), // "onCtrlReadyRead"
QT_MOC_LITERAL(12, 127, 11), // "onCtrlError"
QT_MOC_LITERAL(13, 139, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(14, 168, 3), // "err"
QT_MOC_LITERAL(15, 172, 15), // "onDataConnected"
QT_MOC_LITERAL(16, 188, 15), // "onDataEncrypted"
QT_MOC_LITERAL(17, 204, 15), // "onDataSslErrors"
QT_MOC_LITERAL(18, 220, 15), // "onDataReadyRead"
QT_MOC_LITERAL(19, 236, 18), // "onDataBytesWritten"
QT_MOC_LITERAL(20, 255, 18), // "onDataDisconnected"
QT_MOC_LITERAL(21, 274, 11), // "onDataError"
QT_MOC_LITERAL(22, 286, 9), // "onTimeout"
QT_MOC_LITERAL(23, 296, 10) // "onRateTick"

    },
    "FtpClient\0logMessage\0\0level\0source\0"
    "msg\0onCtrlConnected\0onCtrlEncrypted\0"
    "onCtrlSslErrors\0QList<QSslError>\0"
    "errors\0onCtrlReadyRead\0onCtrlError\0"
    "QAbstractSocket::SocketError\0err\0"
    "onDataConnected\0onDataEncrypted\0"
    "onDataSslErrors\0onDataReadyRead\0"
    "onDataBytesWritten\0onDataDisconnected\0"
    "onDataError\0onTimeout\0onRateTick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FtpClient[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   96,    2, 0x08 /* Private */,
       7,    0,   97,    2, 0x08 /* Private */,
       8,    1,   98,    2, 0x08 /* Private */,
      11,    0,  101,    2, 0x08 /* Private */,
      12,    1,  102,    2, 0x08 /* Private */,
      15,    0,  105,    2, 0x08 /* Private */,
      16,    0,  106,    2, 0x08 /* Private */,
      17,    1,  107,    2, 0x08 /* Private */,
      18,    0,  110,    2, 0x08 /* Private */,
      19,    0,  111,    2, 0x08 /* Private */,
      20,    0,  112,    2, 0x08 /* Private */,
      21,    1,  113,    2, 0x08 /* Private */,
      22,    0,  116,    2, 0x08 /* Private */,
      23,    0,  117,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    3,    4,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FtpClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FtpClient *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 1: _t->onCtrlConnected(); break;
        case 2: _t->onCtrlEncrypted(); break;
        case 3: _t->onCtrlSslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 4: _t->onCtrlReadyRead(); break;
        case 5: _t->onCtrlError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 6: _t->onDataConnected(); break;
        case 7: _t->onDataEncrypted(); break;
        case 8: _t->onDataSslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 9: _t->onDataReadyRead(); break;
        case 10: _t->onDataBytesWritten(); break;
        case 11: _t->onDataDisconnected(); break;
        case 12: _t->onDataError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 13: _t->onTimeout(); break;
        case 14: _t->onRateTick(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FtpClient::*)(const QString & , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FtpClient::logMessage)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FtpClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FtpClient.data,
    qt_meta_data_FtpClient,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FtpClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FtpClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FtpClient.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FtpClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void FtpClient::logMessage(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
