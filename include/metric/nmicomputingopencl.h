/**
 * @file        nmicomputingopencl.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with NMIComputingOpenCL class declaration.
 *
 * @todo TODO - Not implemented yet
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
#ifndef SSIMR_NMICOMPUTINGOPENCL_H
#define SSIMR_NMICOMPUTINGOPENCL_H

#include "../ssimrenderer_global.h"

#include "nmiwrapper.h"
#include "../opencl/openclwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The NMIComputingOpenCL class represents the structure for NMI metric computing on GPU with OpenCL
 *
 * @todo TODO - Not implemented yet
 */
class SHARED_EXPORT NMIComputingOpenCL : public NMIWrapper, public OpenCLWrapper
{
public:
    // Creates NMIComputingOpenCL object with bins count and optional parental OpenGLWrapper
    NMIComputingOpenCL(GLuint binsCount, OpenGLWrapper *parentOpenGLWrapper = 0);

    // Creates NMIComputingOpenCL object with optional parental OpenGLWrapper
    NMIComputingOpenCL(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of NMIComputingOpenCL object
    virtual ~NMIComputingOpenCL();

    // Setting of input image
    virtual void setInputImage(const QImage &image);

    // Setting of rendering output image
    virtual void setRenderingOutputImage(const QImage &image);

    // Returns joint histrogram image
    virtual QImage getJointHistogramImage();

    // Returns histrogram  of input image
    virtual QImage getInputImageHistogramImage();

    // Returns histrogram of rendering output image
    virtual QImage getRenderingOutputImageHistogramImage();

protected:
    // Initialize function
    virtual void initialize();

    // Render function
    virtual void render();

private:
    void renderJointHistogramGPU();

    void renderHistogramGPU(bool, GLuint);

    float renderEntropyGPU(GLuint, bool = false);

    // Other stuff
    QImage inputImage;
    QImage renderingOutputImage;

    cl::Program programNMI;
    cl::Kernel kernelJointHistogram;
    cl::Kernel kernelHistogram;
    cl::Kernel kernelEntropy;
    cl::Program programReduceSum;
    cl::Kernel kernelReduceSum;
    cl::CommandQueue commandQueue;

    cl_mem clMemInputImage;
    cl_mem clMemRenderingOutputImage;

    cl_image_format clImageFormat;

    Q_DISABLE_COPY(NMIComputingOpenCL)
};
}

#endif // SSIMR_NMICOMPUTINGOPENCL_H
#endif // USE_OPENCL
