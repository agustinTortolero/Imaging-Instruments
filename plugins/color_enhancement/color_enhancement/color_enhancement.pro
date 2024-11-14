TEMPLATE = lib
CONFIG += plugin c++17

QT += core

# Platform-specific configurations

# Windows specific
win32: {
    INCLUDEPATH += C:/opencv/opencv/build/include
    LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
            -lopencv_world490  # Release version of OpenCV
    DESTDIR_RELEASE = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/release
    DESTDIR_DEBUG = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/debug
    # OpenMP for MSVC (Windows)
    QMAKE_CXXFLAGS += /openmp
    QMAKE_LFLAGS += /openmp
}

# Linux specific
linux: {
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -L/usr/lib/x86_64-linux-gnu \
            -lopencv_core -lopencv_imgproc -lopencv_highgui  # Adjust this according to the OpenCV version installed
    DESTDIR_RELEASE = /home/agustin/ImagingInstruments-Projects/Imaging-Instruments/build/Desktop_Qt_6_7_3-Release/bin/libs/

    DESTDIR_DEBUG = /home/agustin/ImagingInstruments-Projects/Imaging-Instruments/build/Desktop_Qt_6_7_3-Debug/bin/libs/debug
    # OpenMP for GCC (Linux)
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

# Define preprocessor macro for the plugin build
DEFINES += COLOR_ENHANCEMENT_LIBRARY

# Source and header files
HEADERS += \
    color_enhancement.h \
    color_enhancement_global.h \
    plugin_interface.h

SOURCES += \
    color_enhancement.cpp

# Platform-specific `DESTDIR` handling
CONFIG(debug, debug|release) {
    DESTDIR = $$DESTDIR_DEBUG
} else {
    DESTDIR = $$DESTDIR_RELEASE
}
