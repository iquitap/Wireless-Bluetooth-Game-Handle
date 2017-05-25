#-------------------------------------------------
#
# Project created by QtCreator 2017-05-13T13:03:20
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uart
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    portlistener.cpp

HEADERS  += mainwindow.h \
    portlistener.h

FORMS    += mainwindow.ui
