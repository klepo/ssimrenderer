/**
 * @file        ssdcomputingopencl.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with SSDComputingOpenCL class declaration.
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
#ifndef SSIMR_SSDCOMPUTINGOPENCL_H
#define SSIMR_SSDCOMPUTINGOPENCL_H

#include "../ssimrenderer_global.h"

#include "ssdwrapper.h"
#include "../opencl/openclwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The SSDComputingOpenCL class represents the structure for SSD metric computing on GPU with OpenCL
 */
class SHARED_EXPORT SSDComputingOpenCL : public SSDWrapper, public OpenCLWrapper
{
public:
    // Creates SSDComputingOpenCL object with optional parental OpenGLWrapper
    SSDComputingOpenCL(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of SSDComputingOpenCL object
    ~SSDComputingOpenCL();

    // Setting of input image
    virtual void setInputImage(const QImage &image);

    // Setting of rendering output image
    virtual void setRenderingOutputImage(const QImage &image);

protected:
    // Initialize function
    virtual void initialize();

    // Render function
    virtual void render();

private:
    size_t iCeilTo(size_t size, size_t alignSize) const;

    // Other stuff
    QImage inputImage;
    QImage renderingOutputImage;

    cl::Program programSSD;
    cl::Kernel kernelSSD;
    cl::CommandQueue commandQueue;

    cl_mem clMemInputImage;
    cl_mem clMemRenderingOutputImage;
    cl_mem clMemResult;

    cl_image_format clImageFormat;

    size_t local;
    size_t global;

    Q_DISABLE_COPY(SSDComputingOpenCL)
};
}

#endif // SSIMR_SSDCOMPUTINGOPENCL_H
#endif // USE_OPENCL
