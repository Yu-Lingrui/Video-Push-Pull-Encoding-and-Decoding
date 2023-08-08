#-------------------------------------------------
#
# Project created by QtCreator 2014-02-28T16:16:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RTSPClient
TEMPLATE = app

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:{
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

SOURCES += main.cpp\
    FFmpeg.cpp \
    login.cpp \
    video.cpp

HEADERS  += \
    FFmpeg.h \
    login.h \
    video.h

FORMS    += login.ui \
    video.ui

INCLUDEPATH += ./include

LIBS += -L$$PWD/./ -lavcodec -lavformat -lswscale -lavutil -lavcodec-ffmpeg -lswresample

