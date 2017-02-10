INCLUDEPATH += \
    $$PWD/3rdparty/HDF5/include \
    $$PWD/3rdparty/lm6/include \
    $$PWD/3rdparty/ply/include \
    $$PWD/3rdparty/matio/include \
    $$PWD/3rdparty/zlib/include \
    $$PWD/include \
    $$PWD \

LIBS += \
    -L"$$PWD/3rdparty/HDF5/lib/$$ARCH/$$BUILD_MODE" -lhdf5 \
    -L"$$PWD/3rdparty/lm6/lib/$$ARCH/$$BUILD_MODE" -llm6 \
    -L"$$PWD/3rdparty/ply/lib/$$ARCH/$$BUILD_MODE" -lply \
    -L"$$PWD/3rdparty/matio/lib/$$ARCH/$$BUILD_MODE" -lmatio \
    -L"$$PWD/3rdparty/zlib/lib/$$ARCH/$$BUILD_MODE" -lzlibstatic \

#contains(DEFINES, OPENCL) {
#    include("$$PWD/../helper_scripts/opencl.pri")
#}
