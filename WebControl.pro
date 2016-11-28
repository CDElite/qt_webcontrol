#-------------------------------------------------
#
# Project created by QtCreator 2016-11-21T15:22:58
#
#-------------------------------------------------



QT       += core gui
QT      += core


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WebControl
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialogabout.cpp \
    trayicon.cpp \
    dialogconfiguration.cpp \
    configuration.cpp

HEADERS  += mainwindow.h \
    dialogabout.h \
    trayicon.h \
    dialogconfiguration.h \
    configuration.h

FORMS    += mainwindow.ui \
    dialogabout.ui \
    dialogconfiguration.ui

RESOURCES += \
    icons.qrc

DISTFILES += \
    prog_icon.rc

RC_FILE += prog_icon.rc

include(qxtglobalshortcut5/qxt.pri)

CONFIG += qwt
CONFIG  += qxt
QXT     += core gui

TRANSLATIONS = languages/english.ts\
            languages/chinese.ts \
            languages/korean.ts
