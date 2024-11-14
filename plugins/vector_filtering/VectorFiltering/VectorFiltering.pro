TEMPLATE = lib
CONFIG += plugin c++17

QT += core

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

    DESTDIR_RELEASE = /home/agustin/ImagingInstruments-Projects/Imaging-Instruments/libs/release
    DESTDIR_DEBUG = /home/agustin/ImagingInstruments-Projects/Imaging-Instruments/build/Desktop_Qt_6_7_3-Debug/bin/libs/debug


    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

# Define preprocessor macro for the plugin build
DEFINES += VECTORFILTERING_LIBRARY

# Specify sources and headers
SOURCES += \
    vector_filtering.cpp \
    vector_filtering_plugin.cpp

HEADERS += \
    plugin_interface_filtering.h \
    vector_filtering.h \
    vector_filtering_global.h \
    vector_filtering_plugin.h



CONFIG(debug, debug|release) {
    DESTDIR = $$DESTDIR_DEBUG
} else {
    DESTDIR = $$DESTDIR_RELEASE
}


