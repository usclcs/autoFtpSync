/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../autoFtpSync/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[48];
    char stringdata0[623];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 15), // "onTrayActivated"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 33), // "QSystemTrayIcon::ActivationRe..."
QT_MOC_LITERAL(4, 62, 6), // "reason"
QT_MOC_LITERAL(5, 69, 10), // "onTrayQuit"
QT_MOC_LITERAL(6, 80, 26), // "onServerListCurrentChanged"
QT_MOC_LITERAL(7, 107, 11), // "onAddServer"
QT_MOC_LITERAL(8, 119, 12), // "onSaveServer"
QT_MOC_LITERAL(9, 132, 14), // "onDeleteServer"
QT_MOC_LITERAL(10, 147, 12), // "onTestServer"
QT_MOC_LITERAL(11, 160, 17), // "onClearServerForm"
QT_MOC_LITERAL(12, 178, 15), // "onImportServers"
QT_MOC_LITERAL(13, 194, 15), // "onExportServers"
QT_MOC_LITERAL(14, 210, 9), // "onAddTask"
QT_MOC_LITERAL(15, 220, 10), // "onEditTask"
QT_MOC_LITERAL(16, 231, 12), // "onDeleteTask"
QT_MOC_LITERAL(17, 244, 12), // "onToggleTask"
QT_MOC_LITERAL(18, 257, 9), // "onSyncNow"
QT_MOC_LITERAL(19, 267, 12), // "onLogMessage"
QT_MOC_LITERAL(20, 280, 5), // "level"
QT_MOC_LITERAL(21, 286, 4), // "task"
QT_MOC_LITERAL(22, 291, 3), // "msg"
QT_MOC_LITERAL(23, 295, 13), // "onRetryFailed"
QT_MOC_LITERAL(24, 309, 10), // "onClearLog"
QT_MOC_LITERAL(25, 320, 10), // "onBrowseGo"
QT_MOC_LITERAL(26, 331, 10), // "onBrowseUp"
QT_MOC_LITERAL(27, 342, 15), // "onBrowseRefresh"
QT_MOC_LITERAL(28, 358, 12), // "onBrowsePrev"
QT_MOC_LITERAL(29, 371, 12), // "onBrowseNext"
QT_MOC_LITERAL(30, 384, 16), // "onBrowseDownload"
QT_MOC_LITERAL(31, 401, 12), // "onBrowseView"
QT_MOC_LITERAL(32, 414, 12), // "onBrowseCopy"
QT_MOC_LITERAL(33, 427, 12), // "onBrowseMove"
QT_MOC_LITERAL(34, 440, 14), // "onBrowseRename"
QT_MOC_LITERAL(35, 455, 14), // "onBrowseDelete"
QT_MOC_LITERAL(36, 470, 19), // "onBrowseContextMenu"
QT_MOC_LITERAL(37, 490, 3), // "pos"
QT_MOC_LITERAL(38, 494, 17), // "onToggleWebServer"
QT_MOC_LITERAL(39, 512, 12), // "onCopyWebUrl"
QT_MOC_LITERAL(40, 525, 15), // "onRegenWebToken"
QT_MOC_LITERAL(41, 541, 13), // "onCheckUpdate"
QT_MOC_LITERAL(42, 555, 18), // "onTaskStateChanged"
QT_MOC_LITERAL(43, 574, 6), // "taskId"
QT_MOC_LITERAL(44, 581, 6), // "status"
QT_MOC_LITERAL(45, 588, 18), // "onTaskStatsChanged"
QT_MOC_LITERAL(46, 607, 8), // "uploaded"
QT_MOC_LITERAL(47, 616, 6) // "failed"

    },
    "MainWindow\0onTrayActivated\0\0"
    "QSystemTrayIcon::ActivationReason\0"
    "reason\0onTrayQuit\0onServerListCurrentChanged\0"
    "onAddServer\0onSaveServer\0onDeleteServer\0"
    "onTestServer\0onClearServerForm\0"
    "onImportServers\0onExportServers\0"
    "onAddTask\0onEditTask\0onDeleteTask\0"
    "onToggleTask\0onSyncNow\0onLogMessage\0"
    "level\0task\0msg\0onRetryFailed\0onClearLog\0"
    "onBrowseGo\0onBrowseUp\0onBrowseRefresh\0"
    "onBrowsePrev\0onBrowseNext\0onBrowseDownload\0"
    "onBrowseView\0onBrowseCopy\0onBrowseMove\0"
    "onBrowseRename\0onBrowseDelete\0"
    "onBrowseContextMenu\0pos\0onToggleWebServer\0"
    "onCopyWebUrl\0onRegenWebToken\0onCheckUpdate\0"
    "onTaskStateChanged\0taskId\0status\0"
    "onTaskStatsChanged\0uploaded\0failed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      36,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  194,    2, 0x08 /* Private */,
       5,    0,  197,    2, 0x08 /* Private */,
       6,    0,  198,    2, 0x08 /* Private */,
       7,    0,  199,    2, 0x08 /* Private */,
       8,    0,  200,    2, 0x08 /* Private */,
       9,    0,  201,    2, 0x08 /* Private */,
      10,    0,  202,    2, 0x08 /* Private */,
      11,    0,  203,    2, 0x08 /* Private */,
      12,    0,  204,    2, 0x08 /* Private */,
      13,    0,  205,    2, 0x08 /* Private */,
      14,    0,  206,    2, 0x08 /* Private */,
      15,    0,  207,    2, 0x08 /* Private */,
      16,    0,  208,    2, 0x08 /* Private */,
      17,    0,  209,    2, 0x08 /* Private */,
      18,    0,  210,    2, 0x08 /* Private */,
      19,    3,  211,    2, 0x08 /* Private */,
      23,    0,  218,    2, 0x08 /* Private */,
      24,    0,  219,    2, 0x08 /* Private */,
      25,    0,  220,    2, 0x08 /* Private */,
      26,    0,  221,    2, 0x08 /* Private */,
      27,    0,  222,    2, 0x08 /* Private */,
      28,    0,  223,    2, 0x08 /* Private */,
      29,    0,  224,    2, 0x08 /* Private */,
      30,    0,  225,    2, 0x08 /* Private */,
      31,    0,  226,    2, 0x08 /* Private */,
      32,    0,  227,    2, 0x08 /* Private */,
      33,    0,  228,    2, 0x08 /* Private */,
      34,    0,  229,    2, 0x08 /* Private */,
      35,    0,  230,    2, 0x08 /* Private */,
      36,    1,  231,    2, 0x08 /* Private */,
      38,    0,  234,    2, 0x08 /* Private */,
      39,    0,  235,    2, 0x08 /* Private */,
      40,    0,  236,    2, 0x08 /* Private */,
      41,    0,  237,    2, 0x08 /* Private */,
      42,    2,  238,    2, 0x08 /* Private */,
      45,    3,  243,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   20,   21,   22,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   37,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   43,   44,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,   43,   46,   47,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onTrayActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 1: _t->onTrayQuit(); break;
        case 2: _t->onServerListCurrentChanged(); break;
        case 3: _t->onAddServer(); break;
        case 4: _t->onSaveServer(); break;
        case 5: _t->onDeleteServer(); break;
        case 6: _t->onTestServer(); break;
        case 7: _t->onClearServerForm(); break;
        case 8: _t->onImportServers(); break;
        case 9: _t->onExportServers(); break;
        case 10: _t->onAddTask(); break;
        case 11: _t->onEditTask(); break;
        case 12: _t->onDeleteTask(); break;
        case 13: _t->onToggleTask(); break;
        case 14: _t->onSyncNow(); break;
        case 15: _t->onLogMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 16: _t->onRetryFailed(); break;
        case 17: _t->onClearLog(); break;
        case 18: _t->onBrowseGo(); break;
        case 19: _t->onBrowseUp(); break;
        case 20: _t->onBrowseRefresh(); break;
        case 21: _t->onBrowsePrev(); break;
        case 22: _t->onBrowseNext(); break;
        case 23: _t->onBrowseDownload(); break;
        case 24: _t->onBrowseView(); break;
        case 25: _t->onBrowseCopy(); break;
        case 26: _t->onBrowseMove(); break;
        case 27: _t->onBrowseRename(); break;
        case 28: _t->onBrowseDelete(); break;
        case 29: _t->onBrowseContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 30: _t->onToggleWebServer(); break;
        case 31: _t->onCopyWebUrl(); break;
        case 32: _t->onRegenWebToken(); break;
        case 33: _t->onCheckUpdate(); break;
        case 34: _t->onTaskStateChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 35: _t->onTaskStatsChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 36)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 36;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
