include($$PWD/ssimrenderer_dependents.pri)

### shared
#DEFINES += SHARED_LIBRENDER_LIBRARY
#QMAKE_PRE_LINK += $${QMAKE_COPY} \"$$PWD/../ssimrenderer\\bin\\$$ARCH\\$$BUILD_MODE\\ssimrenderer.dll\" \"$$OUT_PWD\\$$BUILD_MODE\\ssimrenderer.dll\"
###
LIBS += -L"$$PWD/lib/$$ARCH/$$BUILD_MODE" -lssimrenderer #/NODEFAULTLIB:libcmt.lib
INCLUDEPATH += $$PWD/include/
DEPENDPATH += $$PWD/include/
PRE_TARGETDEPS += $$PWD/lib/$$ARCH/$$BUILD_MODE/ssimrenderer.lib
INCLUDEPATH += $$PWD/
