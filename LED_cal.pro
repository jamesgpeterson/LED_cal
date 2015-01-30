#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T09:47:15
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LED_cal
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Settings.cpp \
    SerialPortDialog.cpp \
    SerialBuffer.cpp \
    Snooze.cpp \
    Monitors.cpp

HEADERS  += mainwindow.h \
    Settings.h \
    SerialPortDialog.h \
    SerialBuffer.h \
    Snooze.h

FORMS    += mainwindow.ui \
    SerialPortDialog.ui
