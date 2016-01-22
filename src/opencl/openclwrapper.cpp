/**
 * @file        openclwrapper.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 December 2015
 *
 * @brief       The implementation file containing the OpenCLWrapper class.
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
#include "opencl/openclwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief Default constructor
 *
 * Creates a OpenCLWrapper object, calls init() function.
 */
OpenCLWrapper::OpenCLWrapper() : error(CL_SUCCESS)
{
    init();
}

/**
 * @brief Initializes OpenCL
 */
void OpenCLWrapper::init()
{
    try {
        cl::Platform::get(&platforms);
    } catch (cl::Error error) {
        checkError(error.err(), QString(error.what()));
    }
    checkError(platforms.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get()");

    if (DEBUG_OPENCL_INFO) {
        qDebug() << "Number of platforms:" << platforms.size();

        for (uint i = 0; i < platforms.size(); i++) {
            qDebug() << "Platform:" << i;
            platforms[i].getInfo((cl_platform_info) CL_PLATFORM_VENDOR, &platformString);
            qDebug() << "\t" << QString::fromStdString(platformString);
            platforms[i].getInfo((cl_platform_info) CL_PLATFORM_NAME, &platformString);
            qDebug() << "\t" << QString::fromStdString(platformString);
            platforms[i].getInfo((cl_platform_info) CL_PLATFORM_PROFILE, &platformString);
            qDebug() << "\t" << QString::fromStdString(platformString);
            platforms[i].getInfo((cl_platform_info) CL_PLATFORM_VERSION, &platformString);
            qDebug() << "\t" << QString::fromStdString(platformString);

            std::vector<cl::Device> devices;

            platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);
            checkError(devices.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::getDevices()");
            qDebug() << "    Number of devices (CL_DEVICE_TYPE_ALL) on platform:" << devices.size();

            for (uint i = 0; i < devices.size(); i++) {
                qDebug() << "    Device:" << i;
                devices[i].getInfo((cl_device_info) CL_DEVICE_TYPE, &defaultDeviceType);
                qDebug() << "\t    " << getDeviceTypeString(defaultDeviceType);
                devices[i].getInfo((cl_device_info) CL_DEVICE_VENDOR, &deviceString);
                qDebug() << "\t    " << QString::fromStdString(deviceString);
                devices[i].getInfo((cl_device_info) CL_DEVICE_NAME, &deviceString);
                qDebug() << "\t    " << QString::fromStdString(deviceString);
                devices[i].getInfo((cl_device_info) CL_DEVICE_PROFILE, &deviceString);
                qDebug() << "\t    " << QString::fromStdString(deviceString);
                devices[i].getInfo((cl_device_info) CL_DEVICE_VERSION, &deviceString);
                qDebug() << "\t    " << QString::fromStdString(deviceString);
            }
        }

        qDebug() << "";
    }

    defaultPlatform = platforms[0];

    try {
        defaultPlatform.getDevices(CL_DEVICE_TYPE_ALL, &defaultDevices);
    } catch (cl::Error error) {
        checkError(error.err(), QString(error.what()));
    }
    checkError(defaultDevices.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::getDevices()");

    defaultDevice = defaultDevices[0];
}

/**
 * @brief Creates shared OpenCL context with HGLRC and HDC
 * @param[in] hGLRC Handle to native OpenGL rendering contex
 * @param[in] hDC Handle to the device context
 */
void OpenCLWrapper::createContext(HGLRC hGLRC, HDC hDC)
{
    createContext((cl_context_properties) hGLRC, (cl_context_properties) hDC);
}

/**
 * @brief Creates shared OpenCL context with HGLRC and HDC
 * @param[in] hGLRC Handle to native OpenGL rendering contex
 * @param[in] hDC Handle to the device context
 */
void OpenCLWrapper::createContext(cl_context_properties hGLRC, cl_context_properties hDC)
{
    std::string extensionsString;
    bool success = false;
    std::string deviceName;

    for (uint p = 0; p < platforms.size(); p++) {
        std::vector<cl::Device> devices;

        try {
            platforms[p].getDevices(SELECTED_DEVICE_TYPE, &devices);
        } catch (cl::Error) {
            if (DEBUG_OPENCL_INFO) {
                qDebug() << "Number of devices (" << getDeviceTypeString(SELECTED_DEVICE_TYPE) << ") on platform" << p << ":" << devices.size();
            }
            continue;
        }

        if (DEBUG_OPENCL_INFO) {
            qDebug() << "Number of devices (" << getDeviceTypeString(SELECTED_DEVICE_TYPE) << ") on platform" << p << ":" << devices.size();
        }

        // Check for GL sharing support
        for (uint i = 0; i < devices.size(); i++) {
            devices[i].getInfo((cl_device_info) CL_DEVICE_EXTENSIONS, &extensionsString);
            if (QString::fromStdString(extensionsString).contains("cl_khr_gl_sharing")) {
                success = true;
                if (DEBUG_OPENCL_INFO) {
                    qDebug() << "\tdevice:" << i << ":" << "Check for GL sharing support - OK";
                }
            } else {
                if (DEBUG_OPENCL_INFO) {
                    qDebug() << "\t" << i << ":" << "Check for GL sharing support - FALSE";
                }
            }
        }

        if (!success)
            checkError(-1, "Check for GL sharing support");

        // Create CL context properties, add WGL context & handle to DC
        cl_context_properties properties[] = {
            CL_GL_CONTEXT_KHR,      (cl_context_properties) hGLRC,              // WGL Context
            CL_WGL_HDC_KHR,         (cl_context_properties) hDC,                // WGL HDC
            CL_CONTEXT_PLATFORM,    (cl_context_properties) platforms[p](),     // OpenCL platform
            0
        };

        success = false;
        for (uint i = 0; i < devices.size(); i++) {
            try {
                context = cl::Context(devices[i], properties);
                success = true;
                defaultDevice = devices[i];
                defaultDevice.getInfo((cl_device_info) CL_DEVICE_NAME, &deviceName);
                qDebug() << "Shared CL/GL context created with platform" << p << "and device" << i << ":" << QString::fromStdString(deviceName);
                break;
            } catch (cl::Error error) {
                devices[i].getInfo((cl_device_info) CL_DEVICE_NAME, &deviceName);
                qWarning() << "Cannot create shared CL/GL context with platform" << p << "and device" << i << ":" << QString::fromStdString(deviceName) << "(" << getCLErrorString(error.err()) + " " + QString::number(error.err()) << ")";;
            }
        }

        if (success)
            break;
    }

    if (!success) {
        checkError(-1, "Cannot create shared CL/GL context with any device");
    } else {
        //defaultDevice.getInfo((cl_device_info) CL_DEVICE_NAME, &deviceName);
        //qDebug() << "Shared CL/GL context created with device" << ":" << QString::fromStdString(deviceName);
    }

    qDebug() << "";
}

/**
 * @brief Creates OpenCL context
 */
void OpenCLWrapper::createContext()
{
    bool success = false;
    std::string deviceName;

    for (uint p = 0; p < platforms.size(); p++) {
        std::vector<cl::Device> devices;

        try {
            platforms[p].getDevices(SELECTED_DEVICE_TYPE, &devices);
        } catch (cl::Error) {
            if (DEBUG_OPENCL_INFO) {
                qDebug() << "Number of devices (" << getDeviceTypeString(SELECTED_DEVICE_TYPE) << ") on platform" << p << ":" << devices.size();
            }
            continue;
        }

        if (DEBUG_OPENCL_INFO) {
            qDebug() << "Number of devices (" << getDeviceTypeString(SELECTED_DEVICE_TYPE) << ") on platform" << p << ":" << devices.size();
        }

        // Create CL context properties
        cl_context_properties properties[] = {
            CL_CONTEXT_PLATFORM,    (cl_context_properties) platforms[p](),    // OpenCL platform
            0
        };

        for (uint i = 0; i < devices.size(); i++) {
            try {
                context = cl::Context(devices[i], properties);
                success = true;
                defaultDevice = devices[i];
                defaultDevice.getInfo((cl_device_info) CL_DEVICE_NAME, &deviceName);
                qDebug() << "CL context created with platform" << p << "and device" << i << ":" << QString::fromStdString(deviceName);
                break;
            } catch (cl::Error error) {
                devices[i].getInfo((cl_device_info) CL_DEVICE_NAME, &deviceName);
                qWarning() << "Cannot create CL context with platform" << p << "and device" << i << ":" << QString::fromStdString(deviceName) << "(" << getCLErrorString(error.err()) + " " + QString::number(error.err()) << ")";
            }
        }

        if (success)
            break;
    }

    if (!success) {
        checkError(-1, "Cannot create CL context with any device");
    } else {
        //defaultDevice.getInfo((cl_device_info) CL_DEVICE_NAME, &deviceName);
        //qDebug() << "CL context created with device" << ":" << QString::fromStdString(deviceName);
    }
}

/**
 * @brief Checks error code and may prints error message
 * @param[in] error OpenCL error code or 1 for custom error
 * @param[in] name Error message string
 */
void OpenCLWrapper::checkError(cl_int error, const QString &name) const
{
    if (error == 1) {
        QString msg = "OpenCL error: " + name;
        qCritical() << msg;
        throw std::runtime_error(msg.toStdString());
    } else if (error != CL_SUCCESS) {
        QString msg = "OpenCL error: " + name + " " + getCLErrorString(error) + " " + QString::number(error);
        qCritical() << msg;
        throw std::runtime_error(msg.toStdString());
        //exit(EXIT_FAILURE);
    }
}

/**
 * @brief Checks error code and may prints debug message
 * @param[in] error OpenCL error code or 1 for custom error
 * @param[in] name Warning message string
 */
void OpenCLWrapper::checkWarning(cl_int error, const QString &name)
{
    if (error == 1) {
        QString msg = "OpenCL warning: " + name;
        qWarning() << msg;
    } else if (error != CL_SUCCESS) {
        QString msg = "OpenCL warning: " + name + " " + getCLErrorString(error) + " " + QString::number(error);
        qWarning() << msg;
    }
}

/**
 * @brief Creates OpenCL program form file
 * @param[in] filename OpenCL program src filename (filepath)
 * @return OpenCL program
 */
cl::Program OpenCLWrapper::createProgramFromSourceFile(const QString &filename) const
{
    // Try open the file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        checkError(1, "Error opening OpenCL program file " + filename);

    QTextStream fileContent(&file);
    QString sourceCode = fileContent.readAll();
    cl_int error;

    cl::Program program = cl::Program(context, sourceCode.toStdString(), false, &error);
    checkError(error, "cl::Program()");
    return program;
}

/**
 * @brief Builds OpenCL program
 * @param[in, out] program OpenCL program to build
 */
void OpenCLWrapper::buildProgram(cl::Program *program)
{
    try {
        //const char options[] = "-Werror -cl-std=CL1.1 -cl-opt-disable";
        const char options[] = "-Werror -cl-std=CL1.1";
        program->build(options);
    } catch (cl::Error error) {
        std::string buildlog;
        buildlog = program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(getDevice());
        qDebug() << QString::fromStdString(buildlog);
        checkError(error.err(), QString(error.what()));
    }
}

/**
 * @brief Returns OpenCL context
 * @return OpenCL context
 */
cl::Context OpenCLWrapper::getContext() const
{
    return context;
}

/**
 * @brief Returns native OpenCL context
 * @return Native OpenCL context
 */
cl_context OpenCLWrapper::getNativeContext() const
{
    return context();
}

/**
 * @brief Returns selected OpenCL device
 * @return Selected OpenCL device
 */
cl::Device OpenCLWrapper::getDevice() const
{
    return defaultDevice;
}

/**
 * @brief Converts error code to string
 * @param[in] errorCode Error code
 * @return Error string
 */
QString OpenCLWrapper::getCLErrorString(cl_int errorCode) const
{
    QString ret = "UNKNOWN ERROR";
    switch (errorCode) {
    case CL_SUCCESS:
        ret = "CL_SUCCESS";
        break;
    case CL_DEVICE_NOT_FOUND:
        ret = "CL_DEVICE_NOT_FOUND";
        break;
    case CL_DEVICE_NOT_AVAILABLE:
        ret = "CL_DEVICE_NOT_AVAILABLE";
        break;
    case CL_COMPILER_NOT_AVAILABLE:
        ret = "CL_COMPILER_NOT_AVAILABLE";
        break;
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        ret = "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        break;
    case CL_OUT_OF_RESOURCES:
        ret = "CL_OUT_OF_RESOURCES";
        break;
    case CL_OUT_OF_HOST_MEMORY:
        ret = "CL_OUT_OF_HOST_MEMORY";
        break;
    case CL_PROFILING_INFO_NOT_AVAILABLE:
        ret = "CL_PROFILING_INFO_NOT_AVAILABLE";
        break;
    case CL_MEM_COPY_OVERLAP:
        ret = "CL_MEM_COPY_OVERLAP";
        break;
    case CL_IMAGE_FORMAT_MISMATCH:
        ret = "CL_IMAGE_FORMAT_MISMATCH";
        break;
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        ret = "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        break;
    case CL_BUILD_PROGRAM_FAILURE:
        ret = "CL_BUILD_PROGRAM_FAILURE";
        break;
    case CL_MAP_FAILURE:
        ret = "CL_MAP_FAILURE";
        break;
    case CL_INVALID_VALUE:
        ret = "CL_INVALID_VALUE";
        break;
    case CL_INVALID_DEVICE_TYPE:
        ret = "CL_INVALID_DEVICE_TYPE";
        break;
    case CL_INVALID_PLATFORM:
        ret = "CL_INVALID_PLATFORM";
        break;
    case CL_INVALID_DEVICE:
        ret = "CL_INVALID_DEVICE";
        break;
    case CL_INVALID_CONTEXT:
        ret = "CL_INVALID_CONTEXT";
        break;
    case CL_INVALID_QUEUE_PROPERTIES:
        ret = "CL_INVALID_QUEUE_PROPERTIES";
        break;
    case CL_INVALID_COMMAND_QUEUE:
        ret = "CL_INVALID_COMMAND_QUEUE";
        break;
    case CL_INVALID_HOST_PTR:
        ret = "CL_INVALID_HOST_PTR";
        break;
    case CL_INVALID_MEM_OBJECT:
        ret = "CL_INVALID_MEM_OBJECT";
        break;
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        ret = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        break;
    case CL_INVALID_IMAGE_SIZE:
        ret = "CL_INVALID_IMAGE_SIZE";
        break;
    case CL_INVALID_SAMPLER:
        ret = "CL_INVALID_SAMPLER";
        break;
    case CL_INVALID_BINARY:
        ret = "CL_INVALID_BINARY";
        break;
    case CL_INVALID_BUILD_OPTIONS:
        ret = "CL_INVALID_BUILD_OPTIONS";
        break;
    case CL_INVALID_PROGRAM:
        ret = "CL_INVALID_PROGRAM";
        break;
    case CL_INVALID_PROGRAM_EXECUTABLE:
        ret = "CL_INVALID_PROGRAM_EXECUTABLE";
        break;
    case CL_INVALID_KERNEL_NAME:
        ret = "CL_INVALID_KERNEL_NAME";
        break;
    case CL_INVALID_KERNEL_DEFINITION:
        ret = "CL_INVALID_KERNEL_DEFINITION";
        break;
    case CL_INVALID_KERNEL:
        ret = "CL_INVALID_KERNEL";
        break;
    case CL_INVALID_ARG_INDEX:
        ret = "CL_INVALID_ARG_INDEX";
        break;
    case CL_INVALID_ARG_VALUE:
        ret = "CL_INVALID_ARG_VALUE";
        break;
    case CL_INVALID_ARG_SIZE:
        ret = "CL_INVALID_ARG_SIZE";
        break;
    case CL_INVALID_KERNEL_ARGS:
        ret = "CL_INVALID_KERNEL_ARGS";
        break;
    case CL_INVALID_WORK_DIMENSION:
        ret = "CL_INVALID_WORK_DIMENSION";
        break;
    case CL_INVALID_WORK_GROUP_SIZE:
        ret = "CL_INVALID_WORK_GROUP_SIZE";
        break;
    case CL_INVALID_WORK_ITEM_SIZE:
        ret = "CL_INVALID_WORK_ITEM_SIZE";
        break;
    case CL_INVALID_GLOBAL_OFFSET:
        ret = "CL_INVALID_GLOBAL_OFFSET";
        break;
    case CL_INVALID_EVENT_WAIT_LIST:
        ret = "CL_INVALID_EVENT_WAIT_LIST";
        break;
    case CL_INVALID_EVENT:
        ret = "CL_INVALID_EVENT";
        break;
    case CL_INVALID_OPERATION:
        ret = "CL_INVALID_OPERATION";
        break;
    case CL_INVALID_GL_OBJECT:
        ret = "CL_INVALID_GL_OBJECT";
        break;
    case CL_INVALID_BUFFER_SIZE:
        ret = "CL_INVALID_BUFFER_SIZE";
        break;
    case CL_INVALID_MIP_LEVEL:
        ret = "CL_INVALID_MIP_LEVEL";
        break;
    case CL_INVALID_GLOBAL_WORK_SIZE:
        ret = "CL_INVALID_GLOBAL_WORK_SIZE";
        break;
    default:
        break;
    }
    return ret;
}

/**
 * @brief Converts device type code to string
 * @param[in] type Device type code
 * @return Device type string
 */
QString OpenCLWrapper::getDeviceTypeString(cl_device_type type) const
{
    switch (type) {
    case CL_DEVICE_TYPE_ACCELERATOR:
        return "CL_DEVICE_TYPE_ACCELERATOR";
    case CL_DEVICE_TYPE_ALL:
        return "CL_DEVICE_TYPE_ALL";
    case CL_DEVICE_TYPE_CPU:
        return "CL_DEVICE_TYPE_CPU";
    case CL_DEVICE_TYPE_CUSTOM:
        return "CL_DEVICE_TYPE_CUSTOM";
    case CL_DEVICE_TYPE_DEFAULT:
        return "CL_DEVICE_TYPE_DEFAULT";
    case CL_DEVICE_TYPE_GPU:
        return "CL_DEVICE_TYPE_GPU";
      default :
        return "";
    }
}
}

#endif // USE_OPENCL
