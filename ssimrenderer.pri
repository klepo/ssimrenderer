# Check Qt version
include($$PWD/scripts/check_qt_version.pri)
# Detect architecture and build mode (ARCH = x86/x64, BUILD_MODE = debug/release)
include($$PWD/scripts/detect_arch_and_build_mode.pri)

include($$PWD/ssimrenderer_dependents.pri)

### shared
#DEFINES += SHARED_LIBRENDER_LIBRARY
#QMAKE_PRE_LINK += $${QMAKE_COPY} \"$$PWD/../ssimrenderer\\bin\\$$ARCH\\$$BUILD_MODE\\ssimrenderer.dll\" \"$$OUT_PWD\\$$BUILD_MODE\\ssimrenderer.dll\"
###

### static
LIBS += -L"$$PWD/lib/$$ARCH/$$BUILD_MODE" -lssimrenderer #/NODEFAULTLIB:libcmt.lib
INCLUDEPATH += $$PWD/include/
DEPENDPATH += $$PWD/include/
PRE_TARGETDEPS += $$PWD/lib/$$ARCH/$$BUILD_MODE/ssimrenderer.lib
INCLUDEPATH += $$PWD/
###
