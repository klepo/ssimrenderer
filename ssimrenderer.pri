include($$PWD/ssimrenderer/ssimrenderer_dependents.pri)

### shared
#DEFINES += SHARED_LIBRENDER_LIBRARY
#QMAKE_PRE_LINK += $${QMAKE_COPY} \"$$PWD/../ssimrenderer\\bin\\$$ARCH\\$$BUILD_MODE\\ssimrenderer.dll\" \"$$OUT_PWD\\$$BUILD_MODE\\ssimrenderer.dll\"
###
LIBS += -L"$$PWD/ssimrenderer/lib/$$ARCH/$$BUILD_MODE" -lssimrenderer #/NODEFAULTLIB:libcmt.lib
INCLUDEPATH += $$PWD/ssimrenderer/include/
DEPENDPATH += $$PWD/ssimrenderer/include/
PRE_TARGETDEPS += $$PWD/ssimrenderer/lib/$$ARCH/$$BUILD_MODE/ssimrenderer.lib
INCLUDEPATH += $$PWD/ssimrenderer/
