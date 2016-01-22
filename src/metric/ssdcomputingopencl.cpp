/**
 * @file        ssdcomputingopencl.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the SSDComputingOpenCL class.
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

#include "metric/ssdcomputingopencl.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a SSDComputingOpenCL object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
SSDComputingOpenCL::SSDComputingOpenCL(OpenGLWrapper *parentOpenGLWrapper)
    : SSDWrapper(parentOpenGLWrapper)
{
    clImageFormat.image_channel_order = CL_RGBA;
    clImageFormat.image_channel_data_type = CL_UNORM_INT8;

    local = 256;
    global = 0;
}

/**
 * @brief Destructor of SSDComputingOpenCL object
 *
 * Releases some OpenCL memory objects
 */
SSDComputingOpenCL::~SSDComputingOpenCL()
{
    //checkInitAndMakeCurrentContext();

    //@todo TODO
    clReleaseMemObject(clMemInputImage);
    clReleaseMemObject(clMemRenderingOutputImage);
    clReleaseMemObject(clMemResult);
}

/**
 * @brief Sets of input image
 * @param[in] image Input image
 */
void SSDComputingOpenCL::setInputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    inputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = image.width();
    imageHeight = image.height();

    clReleaseMemObject(clMemInputImage);
    clMemInputImage = clCreateImage2D(getNativeContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &clImageFormat, imageWidth, imageHeight, 0, (void*) inputImage.bits(), &error);
    checkError(error, "clCreateImage2D()");

    global = iCeilTo(imageHeight, local);

    int size = imageHeight;

    error = clSetKernelArg(kernelSSD(), 0, sizeof(cl_mem), &clMemInputImage);
    checkError(error, "clSetKernelArg()");
    error = clSetKernelArg(kernelSSD(), 3, sizeof(int), &size);
    checkError(error, "clSetKernelArg()");

    inputImageLoaded = true;
}

/**
 * @brief Sets of rendering output image
 * @param[in] image Rendering output image
 */
void SSDComputingOpenCL::setRenderingOutputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    renderingOutputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = image.width();
    imageHeight = image.height();

    clReleaseMemObject(clMemRenderingOutputImage);
    clMemRenderingOutputImage = clCreateImage2D(getNativeContext(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &clImageFormat, imageWidth, imageHeight, 0, (void*) renderingOutputImage.bits(), &error);
    checkError(error, "clCreateImage2D()");

    global = iCeilTo(imageHeight, local);

    int size = imageHeight;

    error = clSetKernelArg(kernelSSD(), 1, sizeof(cl_mem), &clMemRenderingOutputImage);
    checkError(error, "clSetKernelArg()");
    error = clSetKernelArg(kernelSSD(), 3, sizeof(int), &size);
    checkError(error, "clSetKernelArg()");

    renderingOutputImageLoaded = true;
}

/**
 * @brief Initializes OpenCL resources, sets shared OpenGL context and initializes other stuff
 */
void SSDComputingOpenCL::initialize()
{
    Q_INIT_RESOURCE(programs);

    // Get shared resources
    if (hasSharedContext()) {
        MainRenderer* parentOpenGLWrapper = (MainRenderer *) getParentOpenGLWrapper();
        toRenderingOutput = parentOpenGLWrapper->getOutputTextureId();
        glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imageWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imageHeight);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Create shared context
        OpenCLWrapper::createContext(parentOpenGLWrapper->getHGLRC(), parentOpenGLWrapper->getHDC());

        clMemRenderingOutputImage = clCreateFromGLTexture2D(getNativeContext(), CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, toRenderingOutput, &error);
        checkError(error, "clCreateFromGLTexture2D()");

        renderingOutputImageLoaded = true;
    } else {
        // Create context
        OpenCLWrapper::createContext();

        clMemRenderingOutputImage = clCreateImage2D(getNativeContext(), 0, &clImageFormat, imageWidth, imageHeight, 0, 0, &error);
        checkError(error, "clCreateImage2D()");
    }

    clMemInputImage = clCreateImage2D(getNativeContext(), 0, &clImageFormat, imageWidth, imageHeight, 0, 0, &error);
    checkError(error, "clCreateImage2D()");

    programSSD = createProgramFromSourceFile(":/programSSD");
    buildProgram(&programSSD);

    commandQueue = cl::CommandQueue(getContext(), getDevice(), 0, &error);
    checkError(error, "cl::CommandQueue()");

    // Result buffer
    clMemResult = clCreateBuffer(getNativeContext(), CL_MEM_READ_WRITE, sizeof(float), 0, &error);
    checkError(error, "clCreateBuffer()");

    kernelSSD = cl::Kernel(programSSD, "ssd", &error);
    checkError(error, "cl::Kernel()");

    global = iCeilTo(imageHeight, local);
    int size = imageHeight;

    error = clSetKernelArg(kernelSSD(), 0, sizeof(cl_mem), &clMemInputImage);
    checkError(error, "clSetKernelArg()");
    error = clSetKernelArg(kernelSSD(), 1, sizeof(cl_mem), &clMemRenderingOutputImage);
    checkError(error, "clSetKernelArg()");
    error = clSetKernelArg(kernelSSD(), 2, sizeof(cl_mem), &clMemResult);
    checkError(error, "clSetKernelArg()");
    error = clSetKernelArg(kernelSSD(), 3, sizeof(int), &size);
    checkError(error, "clSetKernelArg()");
    error = clSetKernelArg(kernelSSD(), 4, sizeof(float) * local, NULL);
    checkError(error, "clSetKernelArg()");

}

/**
 * @brief Render function - main computation of metric
 */
void SSDComputingOpenCL::render()
{
    float ssdFloat = 0;

    if (!inputImageLoaded) {
        qCritical() << "First image is not loaded!";
        return;
    }

    if (!renderingOutputImageLoaded) {
        qCritical() << "Second image is not loaded!";
        return;
    }

    // Computing

    if (hasSharedContext()) {
        clReleaseMemObject(clMemRenderingOutputImage);
        clMemRenderingOutputImage = clCreateFromGLTexture2D(getNativeContext(), CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, toRenderingOutput, &error);
        checkError(error, "clCreateFromGLTexture2D()");
        error = clSetKernelArg(kernelSSD(), 1, sizeof(cl_mem), &clMemRenderingOutputImage);
        checkError(error, "clSetKernelArg()");
    }

    glFlush();

    error = clEnqueueAcquireGLObjects(commandQueue(), 1, &clMemRenderingOutputImage, 0, 0, 0);
    checkError(error, "clEnqueueAcquireGLObjects()");

    try {
        clEnqueueWriteBuffer(commandQueue(), clMemResult, CL_TRUE, 0, sizeof(float), &ssdFloat, 0, NULL, NULL);
    } catch (cl::Error error) {
        checkError(error.err(), QString(error.what()));
    }

    try {
        commandQueue.enqueueNDRangeKernel(kernelSSD, cl::NullRange, cl::NDRange(global), cl::NDRange(local));
    } catch (cl::Error error) {
        checkError(error.err(), QString(error.what()));
    }

    try {
        clEnqueueReadBuffer(commandQueue(), clMemResult, CL_TRUE, 0, sizeof(float), &ssdFloat, 0, NULL, NULL);
    } catch (cl::Error error) {
        checkError(error.err(), QString(error.what()));
    }

    error = clEnqueueReleaseGLObjects(commandQueue(), 1, &clMemRenderingOutputImage, 0, 0, 0);
    checkError(error, "clEnqueueReleaseGLObjects()");

    error = clFinish(commandQueue());
    checkError(error, "clFinish()");

    //qDebug() << "SSD:" << ssdFloat;

    result = ssdFloat;
}

/**
 * @brief Returns padded size of global memory
 * @param[in] size Size of memory to be padded
 * @param[in] alignSize Size of local memory
 * @return Padded size of global memory
 */
size_t SSDComputingOpenCL::iCeilTo(size_t size, size_t alignSize) const
{
    return (size_t) (((size - 1 + alignSize) / alignSize) * alignSize);
}
}
