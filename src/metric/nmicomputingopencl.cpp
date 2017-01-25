/**
 * @file        nmicomputingopencl.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the NMIComputingOpenCL class.
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
#include "metric/nmicomputingopencl.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a NMIComputingOpenCL object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
NMIComputingOpenCL::NMIComputingOpenCL(OpenGLWrapper *parentOpenGLWrapper)
    : NMIWrapper(parentOpenGLWrapper)
{

}

/**
 * @brief Creates a NMIComputingOpenCL object with optional parental OpenGLWrapper
 * @param[in] binsCount Bins count for histograms
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
NMIComputingOpenCL::NMIComputingOpenCL(GLuint binsCount, OpenGLWrapper *parentOpenGLWrapper)
    : NMIWrapper(binsCount, parentOpenGLWrapper)
{

}

/**
 * @brief Destructor of NMIComputingOpenCL object
 *
 * Releases some OpenCL memory objects
 */
NMIComputingOpenCL::~NMIComputingOpenCL()
{
    //checkInitAndMakeCurrentContext();

    //@todo TODO
    //clReleaseMemObject(clMemInputImage);
    clReleaseMemObject(clMemRenderingOutputImage);
}

/**
 * @brief Sets of input image
 * @param[in] image Input image
 */
void NMIComputingOpenCL::setInputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    inputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = image.width();
    imageHeight = image.height();

    setNormalizedOutputUnit();

    inputImageLoaded = true;
}

/**
 * @brief Sets of rendering output image
 * @param[in] image Rendering output image
 */
void NMIComputingOpenCL::setRenderingOutputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    renderingOutputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = renderingOutputImage.width();
    imageHeight = renderingOutputImage.height();

    setNormalizedOutputUnit();

    renderingOutputImageLoaded = true;
}

/**
 * @brief Returns joint histrogram image
 * @return Joint histogram image
 * @todo TODO
 */
QImage NMIComputingOpenCL::getJointHistogramImage()
{
    QImage image = QImage(binsCount, binsCount, QImage::Format_RGB888);

    //@todo TODO

    return image;
}

/**
 * @brief Returns histogram of input image
 * @return Histogram of input image
 *
 * @todo TODO
 */
QImage NMIComputingOpenCL::getInputImageHistogramImage()
{
    QImage image = QImage(binsCount, 1, QImage::Format_RGB888);

    //@todo TODO

    return image;
}

/**
 * @brief Returns histogram of rendering output image
 * @return Histogram of rendering output image
 *
 * @todo TODO
 */
QImage NMIComputingOpenCL::getRenderingOutputImageHistogramImage()
{
    QImage image = QImage(binsCount, 1, QImage::Format_RGB888);

    //@todo TODO

    return image;
}

/**
 * @brief Initializes OpenCL resources, sets shared OpenGL context and initializes other stuff
 *
 * @todo TODO
 */
void NMIComputingOpenCL::initialize()
{
    // Important for resources in library
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
        checkError(error, "clCreateFromGLTexture()");

        renderingOutputImageLoaded = true;
    } else {
        // Create context
        OpenCLWrapper::createContext();

        clMemRenderingOutputImage = clCreateImage2D(getNativeContext(), 0, &clImageFormat, imageWidth, imageHeight, 0, 0, &error);
        checkError(error, "clCreateImage2D()");
    }

    setHistogramBinsCount(binsCount);

    //@todo TODO

}

/**
 * @brief Render function - main computation of metric
 *
 * @todo TODO
 */
void NMIComputingOpenCL::render()
{
    if (!inputImageLoaded) {
        qCritical() << "First image is not loaded!";
        return;
    }

    if (!renderingOutputImageLoaded) {
        qCritical() << "Second image is not loaded!";
        return;
    }

    //@todo TODO

    //qDebug() << "joint entropy:" << jH;
    //qDebug() << "entropy 0:" << h0;
    //qDebug() << "entropy 1:" << h1;

    result = (h0 + h1) / jH;
    //qDebug() << "NMI:" << result;
}

/**
 * @brief Makes joint histogram
 *
 * @todo TODO
 */
void NMIComputingOpenCL::renderJointHistogramGPU()
{
    //@todo TODO
}

/**
 * @brief Makes histogram from joint histogram
 *
 * @todo TODO
 */
void NMIComputingOpenCL::renderHistogramGPU(bool /*flag*/, GLuint /*textureHistogramId*/)
{
    //@todo TODO
}

/**
 * @brief Computes entropy
 * @param[in] histogram Input histogram
 * @param[in] jointFlag Flag for joint histogram
 * @return Computed entropy
 *
 * @todo TODO
 */
float NMIComputingOpenCL::renderEntropyGPU(GLuint /*textureHistogramId*/, bool /*jointFlag*/)
{
    //@todo TODO
    return 0;
}
}

#endif // USE_OPENCL
