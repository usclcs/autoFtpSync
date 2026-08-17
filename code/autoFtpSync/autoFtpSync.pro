QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = autoFtpSync

DEFINES += QT_DEPRECATED_WARNINGS

# 应用图标（嵌入 exe + 资源）
RC_ICONS = icons/app.ico
RESOURCES += icons.qrc

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ftpclient.cpp \
    configmanager.cpp \
    synctask.cpp \
    taskdialog.cpp \
    webserver.cpp \
    versionmanager.cpp

HEADERS += \
    mainwindow.h \
    ftpclient.h \
    configmanager.h \
    synctask.h \
    taskdialog.h \
    webserver.h \
    versionmanager.h

FORMS += \
    mainwindow.ui \
    taskdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
