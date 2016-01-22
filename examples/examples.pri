#-------------------------------------------------
#
# Qt project file
#
# SSIMRenderer examples template
#
#-------------------------------------------------

#----Settings----

# Enable OpenCL
#OPENCL = true
DEFINES += DATA_PATH=\"\\\"$$PWD/data\\\"\"

#----------------

CONFIG += console qt
CONFIG += c++11
# Don't create new debug/release dirs in destination build directory
CONFIG -= debug_and_release

# Disable some Microsoft warnings
DEFINES += _CRT_SECURE_NO_WARNINGS

# Check Qt version
include($$PWD/../check_qt_version.pri)
# Detect architecture and build mode (ARCH = x86/x64, BUILD_MODE = debug/release)
include($$PWD/../detect_arch_and_build_mode.pri)

# Include widgets - required for QApplication
QT += widgets

# Ignore warning LNK4099: PDB 'vc*.pdb' was not found with...
QMAKE_LFLAGS += /ignore:4099

TEMPLATE = app

# Inlude SSIMRenderer library
include($$PWD/../../ssimrenderer.pri)

# Copy built example to bin dir
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/../bin/$$ARCH\" $(MKDIR) \"$$PWD/../bin/$$ARCH\") &
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/../bin/$$ARCH/$$BUILD_MODE\" $(MKDIR) \"$$PWD/../bin/$$ARCH/$$BUILD_MODE\") &
QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$${TARGET}.exe\" \"$$PWD\\..\\bin\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.exe\"
