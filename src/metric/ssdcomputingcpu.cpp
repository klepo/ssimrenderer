/**
 * @file        ssdcomputingcpu.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the SSDComputingCPU class.
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

#include "metric/ssdcomputingcpu.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a SSDComputingCPU object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
SSDComputingCPU::SSDComputingCPU(OpenGLWrapper *parentOpenGLWrapper)
    : SSDWrapper(parentOpenGLWrapper)
{

}

/**
 * @brief Destructor of SSDComputingCPU object
 *
 * Does nothing.
 */
SSDComputingCPU::~SSDComputingCPU()
{
    //checkInitAndMakeCurrentContext();
}

/**
 * @brief Sets of input image
 * @param[in] image Input image
 */
void SSDComputingCPU::setInputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    inputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = image.width();
    imageHeight = image.height();

    inputImageLoaded = true;
}

/**
 * @brief Sets of rendering output image
 * @param[in] image Rendering output image
 */
void SSDComputingCPU::setRenderingOutputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    renderingOutputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = image.width();
    imageHeight = image.height();

    renderingOutputImageLoaded = true;
}

/**
 * @brief Sets shared OpenGL context and initializes other stuff
 */
void SSDComputingCPU::initialize()
{
    // Get shared resources
    if (hasSharedContext()) {
        MainRenderer* parentOpenGLWrapper = (MainRenderer *) getParentOpenGLWrapper();
        toRenderingOutput = parentOpenGLWrapper->getOutputTextureId();

        glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imageWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imageHeight);
        renderingOutputImage = QImage(imageWidth, imageHeight, QImage::Format_RGBA8888);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, renderingOutputImage.bits());
        glBindTexture(GL_TEXTURE_2D, 0);

        renderingOutputImageLoaded = true;
    }
}

/**
 * @brief Render function - main computation of metric
 */
void SSDComputingCPU::render()
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

    if (hasSharedContext()) {
        glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imageWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imageHeight);
        renderingOutputImage = QImage(imageWidth, imageHeight, QImage::Format_RGBA8888);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, renderingOutputImage.bits());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ssdFloat = renderCPU();

    //qDebug() << "SSD:" << ssdFloat;

    result = ssdFloat;
}

/**
 * @brief Computes SSD
 * @return SSD value
 */
float SSDComputingCPU::renderCPU()
{
    float ssd = 0;
    float *array = new float[imageHeight * imageWidth]();

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            float value0 = (float) qRed(inputImage.pixel(x, y)) / 255.0f;
            float value1 = (float) qRed(renderingOutputImage.pixel(x, y)) / 255.0f;
            float diff = value0 - value1;
            ssd += diff * diff;
        }
    }
    delete[] array;

    return ssd;
}
}
