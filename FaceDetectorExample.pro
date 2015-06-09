TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    detector.cpp \
    lib_src/cvSupport.cpp \
    lib_src/support.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    detector.h \
    lib_src/cvSupport.h \
    lib_src/support.h

