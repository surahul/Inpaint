#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T15:19:17
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ExamplarInpaint
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    inpainter.cpp

INCLUDEPATH += C:\opencv\build\include

LIBS +=-LC:\opencv\release\lib\
-lopencv_core245\
-lopencv_highgui245\
-lopencv_imgproc245\
-lopencv_video245\

HEADERS += \
    inpainter.h
