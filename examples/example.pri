#-------------------------------------------------
#
# Qt project file
#
# SSIMRenderer examples template
#
#-------------------------------------------------

#----Settings----

# Enable OpenCL?
#DEFINES += OPENCL

DEFINES += DATA_PATH=\"\\\"$$PWD/data\\\"\"

#----------------

CONFIG += console qt
CONFIG += c++11
# Don't create new debug/release dirs in destination build directory
CONFIG -= debug_and_release

# Disable some Microsoft warnings
DEFINES += _CRT_SECURE_NO_WARNINGS

# Include widgets - required for QApplication
QT += widgets

# Ignore warning LNK4099: PDB 'vc*.pdb' was not found with...
QMAKE_LFLAGS += /ignore:4099

TEMPLATE = app

# Include SSIMRenderer library
include($$PWD/../ssimrenderer.pri)

# Copy built example to bin dir
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/../bin/$$ARCH\" $(MKDIR) \"$$PWD/../bin/$$ARCH\") &
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/../bin/$$ARCH/$$BUILD_MODE\" $(MKDIR) \"$$PWD/../bin/$$ARCH/$$BUILD_MODE\") &
QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$${TARGET}.exe\" \"$$PWD\\..\\bin\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.exe\"
