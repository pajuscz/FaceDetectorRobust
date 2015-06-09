TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp \
    detector.cpp \
    lib_src/cvSupport.cpp \
    lib_src/support.cpp

include(deployment.pri)
qtcAddDeployment()

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib \
-lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_video \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_contrib \
-lopencv_legacy \
-lopencv_flann

HEADERS += \
    detector.h \
    lib_src/cvSupport.h \
    lib_src/support.h

