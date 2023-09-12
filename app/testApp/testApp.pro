QT       += core gui serialport multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testApp
TEMPLATE = app

INCLUDEPATH += main

HEADERS += \
    arecord.h \
    cameratest.h \
    commoncomponent.h \
    mainwindow.h \
    memtest.h \
    rgbtest.h \
    uarttest.h \
    v4l2_service.h \
    wifitest.h \
    _4gtest.h \
    autoreboot.h \
    bluetooth.h \
    ethnettest.h \
    gpiotest.h \
    ledtest.h \
    otg_host.h \
    pwmtest.h \
    relaytest.h \
    rtctest.h \
    tfcardtest.h \
    usbtest.h

SOURCES += \
    arecord.cpp \
    cameratest.cpp \
    commoncomponent.cpp \
    main.cpp \
    mainwindow.cpp \
    memtest.cpp \
    rgbtest.cpp \
    uarttest.cpp \
    v4l2_service.c \
    wifitest.cpp \
    _4gtest.cpp \
    autoreboot.cpp \
    bluetooth.cpp \
    ethnettest.cpp \
    gpiotest.cpp \
    ledtest.cpp \
    otg_host.cpp \
    pwmtest.cpp \
    relaytest.cpp \
    rtctest.cpp \
    tfcardtest.cpp \
    usbtest.cpp

INSTALLS += target

RESOURCES += \
    res.qrc
