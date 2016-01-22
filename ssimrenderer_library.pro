#-------------------------------------------------
#
# Qt project file
#
# SSIMRenderer library
#
#-------------------------------------------------

#----Settings----

# Select type of library: shared/static
TYPE = static

# Enable or disable OpenCL
OPENCL = true

#----------------

CONFIG += console
CONFIG += c++11
# Don't create new debug/release dirs in destination build directory
CONFIG -= debug_and_release

# Disable some Microsoft warnings
DEFINES += _CRT_SECURE_NO_WARNINGS

# Check Qt version
include($$PWD/check_qt_version.pri)
# Detect architecture and build mode (ARCH = x86/x64, BUILD_MODE = debug/release)
include($$PWD/detect_arch_and_build_mode.pri)

# Ignore warning LNK4099: PDB 'vc*.pdb' was not found with...
QMAKE_LFLAGS += /ignore:4099

TARGET = ssimrenderer
TEMPLATE = lib

# Set shared or static config/defines
contains(TYPE, shared) {
    DEFINES += SHARED_SSIMRENDERER_LIBRARY
    DEFINES += SHARED_LIBRARY_EXPORT
} else {
    CONFIG += staticlib
}

INCLUDEPATH += \
    $$PWD \
    $$PWD/include

DEPENDPATH += $$PWD

SOURCES += \
    \# I/O files interface, structures
    src/input/csvwrapper.cpp \
    src/input/hdf5wrapper.cpp \
    src/input/pyramid.cpp \
    src/input/mesh.cpp \
    src/input/statisticaldata.cpp \
    src/input/csvcoeffsfile.cpp \
    src/input/csvpyramidfile.cpp \
    src/input/csvgeneralfile.cpp \
    src/input/matstatisticaldatafile.cpp \
    src/input/lm6meshfile.cpp \
    src/input/hdf5meshfile.cpp \
    src/input/hdf5statisticaldatafile.cpp \
    src/input/plymeshfile.cpp \
    src/input/xmlcalibsfile.cpp \
    \# OpenGL
    src/opengl/openglwrapper.cpp \
    \# Rendering
    src/rendering/mainrenderer.cpp \
    src/rendering/offscreenrenderer.cpp \
    src/rendering/sharedwindow.cpp \
    src/rendering/window.cpp \
    src/rendering/shaders/densityfsgenerator/densityfsgenerator.cpp \
    \# OpenCL
    src/opencl/openclwrapper.cpp \
    \# Metrics
    src/metric/metricwrapper.cpp \
    src/metric/nmiwrapper.cpp \
    src/metric/ssdwrapper.cpp \
    \
    src/metric/nmicomputingopengl.cpp \
    src/metric/nmicomputingopencl.cpp \
    src/metric/nmicomputingcpu.cpp \
    src/metric/ssdcomputingopengl.cpp \
    src/metric/ssdcomputingopencl.cpp \
    src/metric/ssdcomputingcpu.cpp

HEADERS += \
    \
    include/input/csvwrapper.h \
    include/input/hdf5wrapper.h \
    include/input/pyramid.h \
    include/input/mesh.h \
    include/input/statisticaldata.h \
    include/input/csvcoeffsfile.h \
    include/input/csvpyramidfile.h \
    include/input/csvgeneralfile.h \
    include/input/matstatisticaldatafile.h \
    include/input/lm6meshfile.h \
    include/input/hdf5meshfile.h \
    include/input/hdf5statisticaldatafile.h \
    include/input/plymeshfile.h \
    include/input/xmlcalibsfile.h \
    \
    include/opengl/openglwrapper.h \
    \
    include/rendering/mainrenderer.h \
    include/rendering/offscreenrenderer.h \
    include/rendering/sharedwindow.h \
    include/rendering/window.h \
    include/rendering/densityfsgenerator/densityfsgenerator.h \
    \
    include/opencl/openclwrapper.h \
    \
    include/metric/metricwrapper.h \
    include/metric/nmiwrapper.h \
    include/metric/ssdwrapper.h \
    \
    include/metric/nmicomputingopengl.h \
    include/metric/nmicomputingopencl.h \
    include/metric/nmicomputingcpu.h \
    include/metric/ssdcomputingopengl.h \
    include/metric/ssdcomputingopencl.h \
    include/metric/ssdcomputingcpu.h \
    include/ssimrenderer.h \
    include/ssimrenderer_global.h

# Almost shaders and programs
OTHER_FILES += \
    src/rendering/shaders/vertexshaderdensity.vert \
    src/rendering/shaders/geometryshaderdensity.geom \
    \
    src/rendering/shaders/vertexshadersilhouettes.vert \
    src/rendering/shaders/geometryshadersilhouettes.geom \
    src/rendering/shaders/fragmentshadersilhouettes.frag \
    \
    src/rendering/shaders/vertexshadercomputing.vert \
    src/rendering/shaders/fragmentshadercomputing.frag \
    \
    src/rendering/shaders/vertexshaderpyramid.vert \
    src/rendering/shaders/fragmentshaderpyramid.frag \
    \
    src/rendering/shaders/vertexshaderpolygonal.vert \
    \#src/rendering/shaders/geometryshaderpolygonal.geom \
    src/rendering/shaders/fragmentshaderpolygonal.frag \
    \
    src/rendering/shaders/vertexshaderpostprocessing.vert \
    src/rendering/shaders/vertexshaderpostprocessingsimple.vert \
    src/rendering/shaders/fragmentshaderpostprocessing.frag \
    src/rendering/shaders/fragmentshaderpostprocessingsimple.frag \
    \
    src/metric/shaders/vertexshaderhistogram.vert \
    src/metric/shaders/fragmentshaderhistogram.frag \
    \
    src/metric/shaders/vertexshaderjointhistogram.vert \
    src/metric/shaders/fragmentshaderjointhistogram.frag \
    \
    src/metric/shaders/vertexshaderentropy.vert \
    src/metric/shaders/fragmentshaderentropy.frag \
    \
    src/metric/shaders/vertexshaderssd.vert \
    src/metric/shaders/fragmentshaderssd.frag \
    \
    src/metric/programs/ssd.cl

RESOURCES += \
    resources/shaders.qrc \
    resources/programs.qrc

# Dependents
include(ssimrenderer_dependents.pri)

# Copy built library files to destination
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/lib/$$ARCH\" $(MKDIR) \"$$PWD/lib/$$ARCH\") &
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/lib/$$ARCH/$$BUILD_MODE\" $(MKDIR) \"$$PWD/lib/$$ARCH/$$BUILD_MODE\") &
#QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$$BUILD_MODE\\$${TARGET}.lib\" \"$$PWD\\lib\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.lib\" &
QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$${TARGET}.lib\" \"$$PWD\\lib\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.lib\" &

contains(TYPE, shared) {
    QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/bin/$$ARCH\" $(MKDIR) \"$$PWD/bin/$$ARCH\") &
    QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/bin/$$ARCH/$$BUILD_MODE\" $(MKDIR) \"$$PWD/bin/$$ARCH/$$BUILD_MODE\") &
    #QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$$BUILD_MODE\\$${TARGET}.dll\" \"$$PWD\\bin\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.dll\"
    QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$${TARGET}.dll\" \"$$PWD\\bin\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.dll\"
}
