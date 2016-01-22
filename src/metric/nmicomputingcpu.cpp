/**
 * @file        nmicomputingcpu.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the NMIComputingCPU class.
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

#include "metric/nmicomputingcpu.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a NMIComputingCPU object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
NMIComputingCPU::NMIComputingCPU(OpenGLWrapper *parentOpenGLWrapper)
    : NMIWrapper(parentOpenGLWrapper)
{
    jointHistogram = 0;
    histogramInputImage = 0;
    histogramRenderingOutputImage = 0;
}

/**
 * @brief Creates a NMIComputingCPU object with optional parental OpenGLWrapper
 * @param[in] binsCount Bins count for histograms
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
NMIComputingCPU::NMIComputingCPU(GLuint binsCount, OpenGLWrapper *parentOpenGLWrapper)
    : NMIWrapper(binsCount, parentOpenGLWrapper)
{
    jointHistogram = 0;
    histogramInputImage = 0;
    histogramRenderingOutputImage = 0;
}

/**
 * @brief Destructor of NMIComputingCPU object
 *
 * Deletes memory for histograms
 */
NMIComputingCPU::~NMIComputingCPU()
{
    //checkInitAndMakeCurrentContext();

    delete[] jointHistogram;
    delete[] histogramInputImage;
    delete[] histogramRenderingOutputImage;
}

/**
 * @brief Sets of input image
 * @param[in] image Input image
 */
void NMIComputingCPU::setInputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    inputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = inputImage.width();
    imageHeight = inputImage.height();

    setNormalizedOutputUnit();

    inputImageLoaded = true;
}

/**
 * @brief Sets of rendering output image
 * @param[in] image Rendering output image
 */
void NMIComputingCPU::setRenderingOutputImage(const QImage &image)
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
 * @brief Sets bins count for histograms
 * @param[in] binsCount Bins count for histograms
 */
void NMIComputingCPU::setHistogramBinsCount(int binsCount)
{
    this->binsCount = binsCount;

    delete[] jointHistogram;
    delete[] histogramInputImage;
    delete[] histogramRenderingOutputImage;
    jointHistogram = new float[this->binsCount * this->binsCount]();
    histogramInputImage = new float[this->binsCount]();
    histogramRenderingOutputImage = new float[this->binsCount]();
}

/**
 * @brief Returns joint histrogram image
 * @return Joint histogram image
 */
QImage NMIComputingCPU::getJointHistogramImage()
{
    QImage image = QImage(binsCount, binsCount, QImage::Format_RGB888);

    for (unsigned int x = 0; x < binsCount; x++) {
        for (unsigned int y = 0; y < binsCount; y++) {
            /*if (jointHistogram[y * binsCount + x] != 0)
                qDebug() << x << y << jointHistogram[y * binsCount + x];*/
            float value = jointHistogram[y * binsCount + x] * imageWidth * imageHeight;
            image.setPixel(x, y, qRgb(value, value, value));
        }
    }

    return image;
}

/**
 * @brief Returns histogram of input image
 * @return Histogram of input image
 */
QImage NMIComputingCPU::getInputImageHistogramImage()
{
    QImage image = QImage(binsCount, binsCount, QImage::Format_RGB888);

    for (unsigned int x = 0; x < binsCount; x++) {
        if (jointHistogram[x] != 0)
            qDebug() << x << jointHistogram[x];
        float value = histogramInputImage[x] * imageWidth * imageHeight;
        image.setPixel(x, 0, qRgb(value, value, value));
    }

    return image;
}

/**
 * @brief Returns histogram of rendering output image
 * @return Histogram of rendering output image
 */
QImage NMIComputingCPU::getRenderingOutputImageHistogramImage()
{
    QImage image = QImage(binsCount, binsCount, QImage::Format_RGB888);

    for (unsigned int x = 0; x < binsCount; x++) {
        if (jointHistogram[x] != 0)
            qDebug() << x << jointHistogram[x];
        float value = histogramRenderingOutputImage[x] * imageWidth * imageHeight;
        image.setPixel(x, 0, qRgb(value, value, value));
    }

    return image;
}

/**
 * @brief Sets shared OpenGL context and initializes other stuff
 */
void NMIComputingCPU::initialize()
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

        setNormalizedOutputUnit();

        renderingOutputImageLoaded = true;
    }

    setHistogramBinsCount(binsCount);
}

/**
 * @brief Render function - main computation of metric
 */
void NMIComputingCPU::render()
{
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

        setNormalizedOutputUnit();
    }

    renderJointHistogramCPU();
    renderHistogramCPU(0, histogramInputImage);
    renderHistogramCPU(1, histogramRenderingOutputImage);

    jH = renderEntropyCPU(jointHistogram, true);
    h0 = renderEntropyCPU(histogramInputImage);
    h1 = renderEntropyCPU(histogramRenderingOutputImage);

    //qDebug() << "joint entropy:" << jH;
    //qDebug() << "entropy 0:" << h0;
    //qDebug() << "entropy 1:" << h1;

    result = (h0 + h1) / jH;
    //qDebug() << "NMI:" << nmi;
}

/**
 * @brief Makes joint histogram
 */
void NMIComputingCPU::renderJointHistogramCPU()
{
    std::fill(jointHistogram, jointHistogram + (binsCount * binsCount), 0.0f);

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            int i = qRed(inputImage.pixel(x, y)) * (float(binsCount - 1) / 255.0f);
            int j = qRed(renderingOutputImage.pixel(x, y)) * (float(binsCount - 1) / 255.0f);
            jointHistogram[binsCount * j + i] += normalizedOutputUnit;
        }
    }
}

/**
 * @brief Makes histogram from joint histogram
 * @param[in] flag Compute first or second histogram
 * @param[out] histogram Made histogram
 */
void NMIComputingCPU::renderHistogramCPU(bool flag, float *histogram)
{
    std::fill(histogram, histogram + binsCount, 0.0f);

    for (unsigned int y = 0; y < binsCount; y++) {
        float sum = 0;
        for (unsigned int x = 0; x < binsCount; x++) {
            if (flag)
                sum += jointHistogram[y * binsCount + x];
            else
                sum += jointHistogram[x * binsCount + y];
        }
        histogram[y] = sum;
    }
}

/**
 * @brief Computes entropy
 * @param[in] histogram Input histogram
 * @param[in] jointFlag Flag for joint histogram
 * @return Computed entropy
 */
float NMIComputingCPU::renderEntropyCPU(float *histogram, bool jointFlag)
{
    GLuint binsCount = this->binsCount;
    float h = 0;

    if (jointFlag)
        binsCount = this->binsCount * this->binsCount;

    for (unsigned int i = 0; i < binsCount; i++) {
        if (histogram[i] > 0)
            h += histogram[i] * qLn(histogram[i]) / qLn(2);
    }

    return -h;
}
}
