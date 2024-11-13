# Specify the project type and configuration
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
CONFIG += dll

# Specify the Qt modules required
QT += core gui widgets

# Specify the target name
TARGET = impulse_noise_plugin

# Include directories for OpenCV
INCLUDEPATH += /usr/include/opencv4

# Source and header files
HEADERS += \
    cvplugininterface.h \
    impulse_noise_global.h \
    impulse_noise.h \
    cvplugininterface.h

SOURCES += \
    impulse_noise.cpp

# Linker settings for Qt and OpenCV
LIBS += -L/usr/lib \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_imgcodecs \
        -lopencv_features2d \
        -lopencv_flann \
        -lopencv_dnn \
        -lopencv_calib3d \
        -lopencv_photo \
        -lopencv_objdetect \
        -lopencv_stitching \
        -lopencv_videoio \
        -lopencv_gapi \
        -lopencv_video \
        -lQt5Core \
        -lQt5Gui \
        -lQt5Widgets

# Define preprocessor macro
DEFINES += IMPULSE_NOISE_LIBRARY

# Set the output directory for the plugin
DESTDIR = $$OUT_PWD/plugins
