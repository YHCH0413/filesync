#-------------------------------------------------
#
# Project created by QtCreator 2019-03-14T15:37:08
#
#-------------------------------------------------

QT       += core gui network
QT+=xml
QMAKE_CFLAGS += -std=c99

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = filesync
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    main.cpp \
    mythread.cpp \
    mythread2.cpp \
    mywidget.cpp \
    docxml.cpp \
    dialogwd.cpp \
    filetime.cpp \
    adddir.cpp \
    ipdialog.cpp

HEADERS += \
    mythread.h \
    mythread2.h \
    mywidget.h \
    docxml.h \
    dialogwd.h \
    filetime.h \
    adddir.h \
    ipdialog.h
FORMS += \
        mywidget.ui \
    dialog.ui \
    adddir.ui \
    ipdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
