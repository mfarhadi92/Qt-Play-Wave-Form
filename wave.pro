QT += core
QT -= gui
QT += multimedia
CONFIG += c++11

TARGET = wave
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    waveform_manager.cpp

HEADERS += \
    waveform_manager.h
