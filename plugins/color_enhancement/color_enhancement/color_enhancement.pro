TEMPLATE = lib
CONFIG += plugin c++17

QT += core

# Include OpenCV headers
INCLUDEPATH += C:/opencv/opencv/build/include

# Source and header files
HEADERS += \
    color_enhancement.h \
    color_enhancement_global.h \
    plugin_interface.h

SOURCES += \
    color_enhancement.cpp

# Set the output directory for both debug and release to match impulse_noise2
DESTDIR_RELEASE = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/release
DESTDIR_DEBUG = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/debug

# Define different libraries and configurations for release and debug
CONFIG(debug, debug|release) {
    LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
            -lopencv_world490d   # Debug version of OpenCV
    DESTDIR = $$DESTDIR_DEBUG
} else {
    LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
            -lopencv_world490    # Release version of OpenCV
    DESTDIR = $$DESTDIR_RELEASE
}

# Define preprocessor macro for the plugin build
DEFINES += COLOR_ENHANCEMENT_LIBRARY

# Enable OpenMP for MSVC (Windows)
win32: QMAKE_CXXFLAGS += /openmp
win32: QMAKE_LFLAGS += /openmp

# Enable OpenMP for GCC (Linux/Ubuntu)
unix: QMAKE_CXXFLAGS += -fopenmp
unix: QMAKE_LFLAGS += -fopenmp

