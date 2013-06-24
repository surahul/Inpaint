#-------------------------------------------------
#
# Project created by QtCreator 2013-06-22T18:59:30
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Inpaint-qt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    inpainter.cpp \
    gradientcalculator.cpp


INCLUDEPATH += C:\opencv\build\include

LIBS +=-LC:\opencv\release\lib\
-lopencv_core245\
-lopencv_highgui245\
-lopencv_imgproc245\
-lopencv_video245\

HEADERS += \
    inpainter.h \
    gradientcalculator.h
