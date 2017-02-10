DEFINES += USE_OPENCL

OPENCL_TYPE = AMD

equals(OPENCL_TYPE, CUDA) {
    #message(Using Cuda OpenCL)
    contains(QMAKE_TARGET.arch, x86_64) {
        CUDA_ARCH = x64
    } else {
        CUDA_ARCH = Win32
    }

    INCLUDEPATH += \
        "$$(CUDA_PATH_V7_5)/include"

    LIBS += \
        -L"$$(CUDA_PATH_V7_5)/lib/$$CUDA_ARCH" -lopencl
    #message("$$(CUDA_PATH_V7_5)/lib/$$CUDA_ARCH")
} else:equals(OPENCL_TYPE, AMD){
    #message(Using AMD OpenCL)
    INCLUDEPATH += \
        "$$(AMDAPPSDKROOT)/include"

    LIBS += \
        -L"$$(AMDAPPSDKROOT)/lib/$$QMAKE_TARGET.arch" -lopencl
    #message("$$(AMDAPPSDKROOT)/lib/$$QMAKE_TARGET.arch")
} else {
    error(Wrong OPENCL_TYPE)
}
