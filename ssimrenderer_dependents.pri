INCLUDEPATH += \
    $$(HDF5_ROOT)/include \
    $$(LIBMESHB_ROOT)/include \
    $$(MATIO_ROOT)/include \
    $$(ZLIB_ROOT)/include \
    $$(OPENEXR_ROOT)/include \
    $$(OPENEXR_ROOT)/include/Imath \
    $$PWD/3rdparty/ply/include \
    $$PWD/include \
    $$PWD \

LIBS += \
    -L"$$(HDF5_ROOT)/lib" -lhdf5 \
    -L"$$(LIBMESHB_ROOT)/lib" -lMeshb.7 \
    -L"$$(MATIO_ROOT)/lib" -llibmatio \
    -L"$$(ZLIB_ROOT)/lib" -lzlib \
    -L"$$(OPENEXR_ROOT)/lib" -lOpenEXR-3_2 -lIlmThread-3_2 -lOpenEXRCore-3_2 -lOpenEXRUtil-3_2 -lIex-3_2 -lImath-3_2 \
    -L"$$PWD/3rdparty/ply/lib/$$ARCH/$$BUILD_MODE" -lply \

contains(OPENCL, true) {
    include("$$PWD/../opencl.pri")
}
