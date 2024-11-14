TEMPLATE = lib  # This specifies that the output is a library
CONFIG += c++17

QT += core gui

# Windows-specific configurations
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

# Linux-specific configurations
linux: {
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -L/usr/lib/x86_64-linux-gnu \
            -lopencv_core -lopencv_imgproc -lopencv_highgui  # Adjust this according to the OpenCV version installed
    DESTDIR_RELEASE = /home/agustin/ImagingInstruments-Projects/Imaging-Instruments/libs_arm64/release



    DESTDIR_DEBUG = /home/agustin/ImagingInstruments-Projects/Imaging-Instruments/build/Desktop_Qt_6_7_3-Debug/bin/libs/debug
    # OpenMP for GCC (Linux)
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
    # Set the name of the output shared library
    TARGET = vector_filtering  # This will create libvector_filtering.so
}

SOURCES += \
    main.cpp \
    vector_filtering_lib.cpp

HEADERS += \
    vector_filtering_lib.h

# Platform-specific `DESTDIR` handling
CONFIG(debug, debug|release) {
    DESTDIR = $$DESTDIR_DEBUG
} else {
    DESTDIR = $$DESTDIR_RELEASE
}
