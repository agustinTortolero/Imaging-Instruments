# Specify the project type and configuration
TEMPLATE = app
CONFIG += c++17

# Specify the Qt modules required
QT += core gui widgets multimedia multimediawidgets charts

# Specify the target name
TARGET = Imaging_Instruments

# Add resources
RESOURCES += resources.qrc

RC_ICONS += resources/icon_camera4.ico

# Platform-specific configurations
win32: {
    # Windows specific include paths
    INCLUDEPATH += C:/opencv/opencv/build/include
    CUDA_PATH = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.4"
}

linux: {
    # Linux specific include paths
    INCLUDEPATH += /usr/include/opencv4
    CUDA_PATH = "/usr/local/cuda"
}

# Source and header files
HEADERS += \
    custom_graphics_view.h \
    gpu_filtering.h \
    image_view.h \
    mainwindow.h \
    controller.h \
    model.h \
    paint_on_img.h \
    plugin_interface.h \
    video_player.h \
    video_settings.h

SOURCES += \
    custom_graphics_view.cpp \
    image_view.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    controller.cpp \
    paint_on_img.cpp \
    video_player.cpp \
    video_settings.cpp

# UI files
FORMS += mainwindow.ui

# Linker settings for Qt and OpenCV
win32: {
    LIBS += -LC:/opencv/opencv/build/x64/vc16/lib \
            -lopencv_world490 \
            -ladvapi32
}

linux: {
    LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_video -lopencv_ml -lopencv_imgcodecs
}

# Specify the CUDA installation path for both platforms
INCLUDEPATH += $$CUDA_PATH/include

# Link against CUDA libraries
win32: {
    LIBS += -L$$CUDA_PATH/lib/x64 -lcudart -lcuda
}

linux: {
    LIBS += -L$$CUDA_PATH/lib64 -lcudart -lcuda
}

# Specify the library search path
release {
    LIBS += -L$$PWD/libs/release
    INCLUDEPATH += $$PWD/libs/release
}

debug {
    LIBS += -L$$PWD/libs/debug
    INCLUDEPATH += $$PWD/libs/debug
}

# Link against custom libraries
LIBS += -L$$PWD/libs \
        -lgpu_filtering \
        -lvector_filtering_lib \
        -lcolor_enhancement \
        -limpulse_noise2 \
        -lVectorFiltering

# Ensure that the Qt MOC and UIC tools run automatically
CONFIG += automoc autouic

# Output directory for the build
DESTDIR = $$OUT_PWD/bin

# Platform-specific post-link actions
win32: {
    # Copy DLLs and other necessary files
    QMAKE_POST_LINK += \
        copy $$PWD/libs/release/gpu_filtering.dll $$OUT_PWD/ && \
        copy $$PWD/libs/release/vector_filtering_lib.dll $$OUT_PWD/ && \
        copy $$PWD/libs/release/color_enhancement.dll $$OUT_PWD/ && \
        copy $$PWD/libs/release/impulse_noise2.dll $$OUT_PWD/ && \
        copy "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.4/bin/cudart64_12.dll" $$DESTDIR && \
        copy "C:/Windows/System32/ucrtbase.dll" $$DESTDIR

    # Release-specific DLLs
    release {
        QMAKE_POST_LINK += \
            copy C:/opencv/opencv/build/x64/vc16/bin/opencv_world490.dll $$DESTDIR
    }

    # Debug-specific DLLs
    debug {
        QMAKE_POST_LINK += \
            copy C:/opencv/opencv/build/x64/vc16/bin/opencv_world490d.dll $$DESTDIR
    }
}

linux: {
    # Linux-specific post-link actions
    QMAKE_POST_LINK += \
        cp $$PWD/libs/release/libgpu_filtering.so $$DESTDIR/ && \
        cp $$PWD/libs/release/libvector_filtering_lib.so $$DESTDIR/ && \
        cp $$PWD/libs/release/libcolor_enhancement.so $$DESTDIR/ && \
        cp $$PWD/libs/release/libimpulse_noise2.so $$DESTDIR/ && \
        cp $$CUDA_PATH/lib64/stubs/libcuda.so $$DESTDIR/

    # Release-specific shared libraries
    release {
        QMAKE_POST_LINK += \
            cp /usr/lib/x86_64-linux-gnu/libopencv_world.so $$DESTDIR
    }

    # Debug-specific shared libraries
    debug {
        QMAKE_POST_LINK += \
            cp /usr/lib/x86_64-linux-gnu/libopencv_world.so.$$QT_BUILD $$DESTDIR
    }
}

# Load the current build number from the file
BUILD_NUMBER_FILE = $$PWD/build_number.txt

# Check if the build number file exists and read/write commands for cross-platform compatibility
macx: {
    exists($$BUILD_NUMBER_FILE) {
        BUILD_NUMBER = $$system(cat $$BUILD_NUMBER_FILE)
    } else {
        BUILD_NUMBER = 0
    }

    BUILD_NUMBER_INCREMENTED = $$eval($$BUILD_NUMBER + 1)
    QMAKE_POST_LINK += echo $$BUILD_NUMBER_INCREMENTED > $$BUILD_NUMBER_FILE
} else: {
    exists($$BUILD_NUMBER_FILE) {
        BUILD_NUMBER = $$system(type $$BUILD_NUMBER_FILE)
    } else {
        BUILD_NUMBER = 0
    }

    BUILD_NUMBER_INCREMENTED = $$eval($$BUILD_NUMBER + 1)
    QMAKE_POST_LINK += echo $$BUILD_NUMBER_INCREMENTED > $$BUILD_NUMBER_FILE
}

# Define a macro to be used in the application
DEFINES += BUILD_NUMBER=\\\"$$BUILD_NUMBER_INCREMENTED\\\"

# Optional: Print the current build number in the output directory for reference
message(Build number is: $$BUILD_NUMBER_INCREMENTED)
