/**
 * @file        openclwrapper.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with OpenCLWrapper class declaration.
 *
 * @copyright   Copyright (C) 2015 Petr Kleparnik, Ondrej Klima. All Rights Reserved.
 *
 * @license     This file may be used, distributed and modified under the terms of the LGPL version 3
 *              open source license. A copy of the LGPL license should have
 *              been recieved with this file. Otherwise, it can be found at:
 *              http://www.gnu.org/copyleft/lesser.html
 *              This file has been created as a part of the Traumatech project:
 *              http://www.fit.vutbr.cz/research/grants/index.php.en?id=733.
 *
 */

#ifdef USE_OPENCL
#ifndef SSIMR_OPENCLWRAPPER_H
#define SSIMR_OPENCLWRAPPER_H

#include "../ssimrenderer_global.h"

#include <QDebug>
#include <QFile>

/// Helper OpenCL macro
#define __CL_ENABLE_EXCEPTIONS

/// Helper OpenCL macro
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

/// Helper OpenCL macro
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.hpp>
#include <CL/cl_gl.h>

namespace SSIMRenderer
{
/**
 * @brief The OpenCLWrapper class represents the wrapper for OpenCL
 */
class SHARED_EXPORT OpenCLWrapper
{
public:
    // Default constructor
    OpenCLWrapper();

    /// Selected device type macro
    static const int SELECTED_DEVICE_TYPE  = CL_DEVICE_TYPE_GPU;

    /// Enable OpenCL initialization debug info
    static const int DEBUG_OPENCL_INFO  = 0;

protected:
    // Initialization
    virtual void init() final;

    // Creates context
    virtual void createContext(HGLRC hGLRC, HDC hDC) final;
    virtual void createContext(cl_context_properties hGLRC, cl_context_properties hDC) final;
    virtual void createContext() final;

    // Helper error and warning functions
    virtual void checkError(cl_int error, const QString &name) const final;
    virtual void checkWarning(cl_int error, const QString &name) final;

    // OpenCL program creating
    virtual cl::Program createProgramFromSourceFile(const QString &filename) const final;
    virtual void buildProgram(cl::Program *program) final;

    // Context getters
    virtual cl::Context getContext() const final;
    virtual cl_context getNativeContext() const final;

    // Device getter
    virtual cl::Device getDevice() const final;

    /// Pure virtual initialize function
    virtual void initialize() = 0;

    // Helper functions for convert some enums to string
    virtual QString getCLErrorString(cl_int errorCode) const final;
    virtual QString getDeviceTypeString(cl_device_type type) const final;

    /// Helper variable for errors
    cl_int error;

private:
    std::vector<cl::Platform> platforms;
    cl::Platform defaultPlatform;
    std::string platformString;

    std::vector<cl::Device> defaultDevices;
    cl::Device defaultDevice;
    std::string deviceString;
    cl_device_type defaultDeviceType;

    cl::Context context;

    Q_DISABLE_COPY(OpenCLWrapper)
};
}

#endif // SSIMR_OPENCLWRAPPER_H
#endif // USE_OPENCL
