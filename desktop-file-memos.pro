#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T16:39:17
#
#-------------------------------------------------

QT       += core gui x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = desktop-file-memos

#CONFIG+=debug_and_release

#CONFIG(debug, debug|release){
#    TARGET = desktop-file-memos_debug
#} else {
#    TARGET = desktop-file-memos
#}

TEMPLATE = app

PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0
LIBS +=-lgio-2.0 -lglib-2.0 -lX11
CONFIG += c++11 link_pkgconfig no_keywords

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    iconview.cpp \
    filemodel.cpp \
    iconprovider.cpp \
    splitter.cpp \
    titlewidget.cpp \
    settingshelper.cpp \
    systemtrayicon.cpp \
    fileoperationjob.cpp \
    delegate.cpp

HEADERS += \
    iconview.h \
    filemodel.h \
    iconprovider.h \
    splitter.h \
    titlewidget.h \
    settingshelper.h \
    systemtrayicon.h \
    fileoperationjob.h \
    delegate.h
