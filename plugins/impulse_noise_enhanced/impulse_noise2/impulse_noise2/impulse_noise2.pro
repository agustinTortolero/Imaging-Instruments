TEMPLATE = lib
CONFIG += plugin c++17

QT += core

# Include OpenCV headers
INCLUDEPATH += C:/opencv/opencv/build/include

# Specify sources and headers
SOURCES += \
    impulse_noise2.cpp \
    impulse_noise2_plugin.cpp

HEADERS += \
    impulse_noise2_global.h \
    impulse_noise2.h \
    impulse_noise2_plugin.h \
    plugin_interface_noise.h

# Define preprocessor macro for the plugin build
DEFINES += IMPULSE_NOISE_LIBRARY

# Set output directories consistent with vector_filtering_lib
DESTDIR_RELEASE = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/release
DESTDIR_DEBUG = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/debug

# OpenCV library paths and different linking for release and debug
CONFIG(debug, debug|release) {
    LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
            -lopencv_world490d   # Debug version of OpenCV
    DESTDIR = $$DESTDIR_DEBUG
} else {
    LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
            -lopencv_world490    # Release version of OpenCV
    DESTDIR = $$DESTDIR_RELEASE
}

# Enable OpenMP for both release and debug
QMAKE_CXXFLAGS_RELEASE += /openmp
QMAKE_CXXFLAGS_DEBUG += /openmp
QMAKE_LFLAGS_RELEASE += /openmp
QMAKE_LFLAGS_DEBUG += /openmp

