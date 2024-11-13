TEMPLATE = lib
CONFIG += plugin c++17

QT += core

# Include OpenCV headers
INCLUDEPATH += C:/opencv/opencv/build/include

# Specify sources and headers
SOURCES += \
    vector_filtering.cpp \
    vector_filtering_plugin.cpp

HEADERS += \
    plugin_interface_filtering.h \
    vector_filtering.h \
    vector_filtering_global.h \
    vector_filtering_plugin.h

# Correct OpenCV library paths
LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
        -lopencv_world490d

# Define preprocessor macro for the plugin build
DEFINES += VECTORFILTERING_LIBRARY

# Set the output directory for the plugin
DESTDIR = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs


win32: {
    QMAKE_CXXFLAGS += /openmp
    QMAKE_LFLAGS += /openmp
}

# Enable OpenMP for GCC (Linux/Ubuntu)
#unix: QMAKE_CXXFLAGS += -fopenmp
#unix: QMAKE_LFLAGS += -fopenmp
