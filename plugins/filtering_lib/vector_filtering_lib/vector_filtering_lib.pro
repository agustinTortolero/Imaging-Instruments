TEMPLATE = lib
CONFIG += c++17

QT += core gui

# Include OpenCV headers
INCLUDEPATH += C:/opencv/opencv/build/include

# Uncomment to disable APIs deprecated before Qt 6.0.0
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    main.cpp \
    vector_filtering_lib.cpp

HEADERS += \
    vector_filtering_lib.h

# Set output directories for release and debug builds
DESTDIR_RELEASE = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/release
DESTDIR_DEBUG = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/debug

# OpenCV library paths
CONFIG(debug, debug|release) {
    LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
            -lopencv_world490d   # Debug version of OpenCV
} else {
    LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
            -lopencv_world490    # Release version of OpenCV
}

# Enable OpenMP for MSVC compiler
QMAKE_CXXFLAGS_RELEASE += /openmp
QMAKE_CXXFLAGS_DEBUG += /openmp

# Ensure release and debug modes are enabled
CONFIG(debug, debug|release) {
    DESTDIR = $$DESTDIR_DEBUG
} else {
    DESTDIR = $$DESTDIR_RELEASE
}
