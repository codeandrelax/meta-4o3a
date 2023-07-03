TEMPLATE = app
TARGET = testqtapp
INCLUDE += .

QT += gui widgets

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += TestQTApp.cpp TLC59731.cpp
