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
    Settings.cpp

HEADERS  += mainwindow.h \
    Settings.h

FORMS    += mainwindow.ui
