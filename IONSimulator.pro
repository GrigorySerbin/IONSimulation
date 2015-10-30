#-------------------------------------------------
#
# Project created by QtCreator 2015-10-26T10:45:19
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IONSimulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    batterymodule.cpp \
    accumulator.cpp \
    controlmodule.cpp \
    informationmodule.cpp \
    accumulatorwidget.cpp \
    batterymodulewidget.cpp \
    controlmodulewidget.cpp \
    informationalmodulewidget.cpp \
    serialport.cpp \
    settingswindow.cpp

HEADERS  += mainwindow.h \
    batterymodule.h \
    accumulator.h \
    controlmodule.h \
    informationmodule.h \
    accumulatorwidget.h \
    batterymodulewidget.h \
    controlmodulewidget.h \
    informationalmodulewidget.h \
    serialport.h \
    settingswindow.h

FORMS    += mainwindow.ui

CONFIG  += C++11
