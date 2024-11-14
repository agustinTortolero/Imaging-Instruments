TEMPLATE = app
CONFIG += c++17

QT += core gui widgets multimedia multimediawidgets charts

TARGET = Imaging_Instruments

RESOURCES += resources.qrc

RC_ICONS += resources/icon_camera4.ico


linux: {
    message("Linux")
    message("$$PWD/libs_arm64/release")
    INCLUDEPATH += /usr/include/opencv4
    CUDA_PATH = "/usr/local/cuda"

    # Check if the system is ARM64 or x86_64
    is_arm64: {
        message("ARM64 CPU ")
        # ARM64 specific library paths
        LIBS += -L$$PWD/libs_arm64/release \
                -lgpu_filtering \
                -lvector_filtering \
                -lcolor_enhancement \
                -limpulse_noise2
    }
    is_x86_64: {
        message("x86_64 CPU")
        #
        LIBS += -L$$PWD/libs_x86_64/release \
                -lgpu_filtering \
                -lvector_filtering \
                -lcolor_enhancement \
                -limpulse_noise2

    }
}

# Source and header files
HEADERS += \
    #custom_graphics_view.h \
    gpu_filtering.h \
    image_view.h \
    vector_filtering.h \
    mainwindow.h \
    controller.h \
    model.h \
    paint_on_img.h \
    plugin_interface.h \
    video_player.h \
    video_settings.h

SOURCES += \
    #custom_graphics_view.cpp \
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
    LIBS += -L$$PWD/libs_arm64/release
    INCLUDEPATH += $$PWD/libs_arm64/release
}

debug {
    LIBS += -L$$PWD/libs_arm64/debug
    INCLUDEPATH += $$PWD/libs_arm64/debug
}

# Link against custom libraries
LIBS += -L$$PWD/libs_arm64/release \
        -lgpu_filtering \
        -lvector_filtering \
        -lcolor_enhancement \
        -limpulse_noise2

# Ensure that the Qt MOC and UIC tools run automatically
CONFIG += automoc autouic

# Output directory for the build
DESTDIR = $$OUT_PWD/bin

# Platform-specific post-link actions
win32: {
    # Copy DLLs and other necessary files
    QMAKE_POST_LINK += \
        copy $$PWD/libs/release/gpu_filtering.dll $$OUT_PWD/ && \
        copy $$PWD/libs/release/vector_filtering.dll $$OUT_PWD/ && \
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
    # Linux-specific post-link actions for ARM64 or x86_64
    is_arm64: {
        # For ARM64 systems, copy the ARM64 libraries
        QMAKE_POST_LINK += \
            cp $$PWD/libs_arm64/release/libgpu_filtering.so $$DESTDIR/ && \
            cp $$PWD/libs_arm64/release/libvector_filtering.so $$DESTDIR/ && \
            cp $$PWD/libs_arm64/release/libcolor_enhancement.so $$DESTDIR/ && \
            cp $$PWD/libs_arm64/release/libimpulse_noise2.so $$DESTDIR/ && \
            cp $$CUDA_PATH/lib64/stubs/libcuda.so $$DESTDIR/

        # Release-specific shared libraries for ARM64
        release {
            QMAKE_POST_LINK += \
                cp /usr/lib/aarch64-linux-gnu/libopencv_world.so $$DESTDIR
        }

        # Debug-specific shared libraries for ARM64
        debug {
            QMAKE_POST_LINK += \
                cp /usr/lib/aarch64-linux-gnu/libopencv_world.so.$$QT_BUILD $$DESTDIR
        }
    }

    is_x86_64: {
        # For x86_64 systems, copy the x86_64 libraries
        QMAKE_POST_LINK += \
            cp $$PWD/libs_x86_64/release/libgpu_filtering.so $$DESTDIR/ && \
            cp $$PWD/libs_x86_64/release/libvector_filtering.so $$DESTDIR/ && \
            cp $$PWD/libs_x86_64/release/libcolor_enhancement.so $$DESTDIR/ && \
            cp $$PWD/libs_x86_64/release/libimpulse_noise2.so $$DESTDIR/ && \
            cp $$CUDA_PATH/lib64/stubs/libcuda.so $$DESTDIR/

        # Release-specific shared libraries for x86_64
        release {
            QMAKE_POST_LINK += \
                cp /usr/lib/x86_64-linux-gnu/libopencv_world.so $$DESTDIR
        }

        # Debug-specific shared libraries for x86_64
        debug {
            QMAKE_POST_LINK += \
                cp /usr/lib/x86_64-linux-gnu/libopencv_world.so.$$QT_BUILD $$DESTDIR
        }
    }
}


