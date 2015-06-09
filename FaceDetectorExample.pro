TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp \
    detector.cpp \
    lib_src/cvSupport.cpp \
    lib_src/support.cpp \
    libflandmark/liblbp.cpp \
    libflandmark/flandmark_detector.cpp \
    lib_stasm/stasm_lib.cpp \
    lib_stasm/stasm.cpp \
    lib_stasm/startshape.cpp \
    lib_stasm/shapemod.cpp \
    lib_stasm/shapehacks.cpp \
    lib_stasm/shape17.cpp \
    lib_stasm/print.cpp \
    lib_stasm/pinstart.cpp \
    lib_stasm/misc.cpp \
    lib_stasm/landmarks.cpp \
    lib_stasm/hatdesc.cpp \
    lib_stasm/hat.cpp \
    lib_stasm/faceroi.cpp \
    lib_stasm/eyedist.cpp \
    lib_stasm/eyedet.cpp \
    lib_stasm/err.cpp \
    lib_stasm/convshape.cpp \
    lib_stasm/classicdesc.cpp \
    lib_stasm/asm.cpp \
    lib_stasm/MOD_1/initasm.cpp \
    lib_stasm/MOD_1/facedet.cpp

include(deployment.pri)
qtcAddDeployment()

INCLUDEPATH += /usr/local/include \
/usr/local/include/opencv
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
    lib_src/support.h \
    libflandmark/msvc-compat.h \
    libflandmark/liblbp.h \
    libflandmark/flandmark_detector.h \
    lib_stasm/stasm_landmarks.h \
    lib_stasm/stasm.h \
    lib_stasm/stasm_lib_ext.h \
    lib_stasm/stasm_lib.h \
    lib_stasm/startshape.h \
    lib_stasm/shapemod.h \
    lib_stasm/shapehacks.h \
    lib_stasm/shape17.h \
    lib_stasm/print.h \
    lib_stasm/pinstart.h \
    lib_stasm/misc.h \
    lib_stasm/landtab_muct77.h \
    lib_stasm/landmarks.h \
    lib_stasm/hatdesc.h \
    lib_stasm/hat.h \
    lib_stasm/faceroi.h \
    lib_stasm/eyedist.h \
    lib_stasm/eyedet.h \
    lib_stasm/err.h \
    lib_stasm/convshape.h \
    lib_stasm/classicdesc.h \
    lib_stasm/basedesc.h \
    lib_stasm/atface.h \
    lib_stasm/asm.h \
    lib_stasm/MOD_1/initasm.h \
    lib_stasm/MOD_1/facedet.h

