TEMPLATE = lib
CONFIG += dll c++17

# Set output directories for release and debug builds
DESTDIR_RELEASE = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/release
DESTDIR_DEBUG = C:/AgustinTortolero_repos/Qt_gui_projects/Imaging_Toolbox/MVC_imaging_toolbox2/libs/debug

# Choose the destination directory based on the configuration
win32: {
    debug:  # Debug build
        DESTDIR = $$DESTDIR_DEBUG
    !debug:  # Release build
        DESTDIR = $$DESTDIR_RELEASE
}

# CUDA installation path
CUDA_INSTALL_PATH = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.4"
INCLUDEPATH += $$CUDA_INSTALL_PATH/include

# CUDA libraries
LIBS += -L$$CUDA_INSTALL_PATH/lib/x64 \
        -lcudart \
        -lcuda

# CUDA-related compiler flags
QMAKE_CXXFLAGS += -arch=compute_80  # Set to compute capability for RTX 4060
QMAKE_CXXFLAGS += -Xcompiler -fopenmp

# Source and header files for the CUDA library
SOURCES += \
    filtering_lib_cuda.cu

HEADERS += \
    filtering_lib_cuda.cuh

# Use nvcc to compile CUDA files
nvcc_path = $$CUDA_INSTALL_PATH/bin/nvcc.exe
cuda_objects = $$CUDA_SOURCES:.cu=.o

cuda.commands = $$nvcc_path -c $$SOURCE -o $$OBJECT -I"$$INCLUDEPATH"
cuda.output = $$OBJECTS_DIR/$${QMAKE_FILE_BASE}_cuda.o
cuda.dependency_type = TYPE_C
QMAKE_EXTRA_COMPILERS += cuda


