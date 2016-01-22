/**
 * @file        ssdcomputingopengl.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the SSDComputingOpenGL class.
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

#include "metric/ssdcomputingopengl.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a SSDComputingOpenGL object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
SSDComputingOpenGL::SSDComputingOpenGL(OpenGLWrapper *parentOpenGLWrapper)
    : SSDWrapper(parentOpenGLWrapper)
{

}

/**
 * @brief Destructor of SSDComputingOpenCL object
 *
 * Releases some OpenGL memory objects
 */
SSDComputingOpenGL::~SSDComputingOpenGL()
{
    checkInitAndMakeCurrentContext();

    //sumOfSquaredDifferences->program->release();
    delete sumOfSquaredDifferences->program;
    delete sumOfSquaredDifferences;

    glDeleteVertexArrays(1, &vao);
    glDeleteFramebuffers(1, &fbo);

    glDeleteTextures(1, &toSSD);
    glDeleteTextures(1, &toInput);

    if (!hasSharedContext())
        glDeleteTextures(1, &toRenderingOutput);
}

/**
 * @brief Sets of input image
 * @param[in] image Input image
 */
void SSDComputingOpenGL::setInputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    QImage inputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = image.width();
    imageHeight = image.height();

    glBindTexture(GL_TEXTURE_2D, toInput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, inputImage.bits());
    glBindTexture(GL_TEXTURE_2D, 0);

    sumOfSquaredDifferences->program->bind();
    sumOfSquaredDifferences->program->setUniformValue(sumOfSquaredDifferences->uWidth, imageWidth);
    sumOfSquaredDifferences->program->release();

    inputImageLoaded = true;
}

/**
 * @brief Sets of rendering output image
 * @param[in] image Rendering output image
 */
void SSDComputingOpenGL::setRenderingOutputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    QImage renderingOutputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = image.width();
    imageHeight = image.height();

    glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, renderingOutputImage.bits());
    glBindTexture(GL_TEXTURE_2D, 0);

    sumOfSquaredDifferences->program->bind();
    sumOfSquaredDifferences->program->setUniformValue(sumOfSquaredDifferences->uWidth, imageWidth);
    sumOfSquaredDifferences->program->release();

    renderingOutputImageLoaded = true;
}

/**
 * @brief Initializes OpenGL resources, sets shared OpenGL context and initializes other stuff
 */
void SSDComputingOpenGL::initialize()
{
    // Important for resources in library
    Q_INIT_RESOURCE(shaders);

    sumOfSquaredDifferences = new SumOfSquaredDifferences();
    sumOfSquaredDifferences->program = 0;

    // Create shaders
    // Program for render histogram
    bool status;
    sumOfSquaredDifferences->program = new QOpenGLShaderProgram();
    status = sumOfSquaredDifferences->program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertexshaderSSD");
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << sumOfSquaredDifferences->program->log();
    status = sumOfSquaredDifferences->program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragmentshaderSSD");
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << sumOfSquaredDifferences->program->log();
    status = sumOfSquaredDifferences->program->link();
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << sumOfSquaredDifferences->program->log();

    // Get shaders variables locations
    sumOfSquaredDifferences->uInput = sumOfSquaredDifferences->program->uniformLocation("uInput");
    sumOfSquaredDifferences->uWidth = sumOfSquaredDifferences->program->uniformLocation("uWidth");
    sumOfSquaredDifferences->uRenderingOutput = sumOfSquaredDifferences->program->uniformLocation("uRenderingOutput");

    // Get shared resources
    if (hasSharedContext()) {
        MainRenderer* parentOpenGLWrapper = (MainRenderer *) getParentOpenGLWrapper();
        toRenderingOutput = parentOpenGLWrapper->getOutputTextureId();
        glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imageWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imageHeight);
        glBindTexture(GL_TEXTURE_2D, 0);

        sumOfSquaredDifferences->program->bind();
        sumOfSquaredDifferences->program->setUniformValue(sumOfSquaredDifferences->uWidth, imageWidth);
        sumOfSquaredDifferences->program->release();

        renderingOutputImageLoaded = true;
    } else {
        glGenTextures(1, &toRenderingOutput);
        glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glGenTextures(1, &toInput);
    glBindTexture(GL_TEXTURE_2D, toInput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create other resources
    glGenTextures(1, &toSSD);
    glBindTexture(GL_TEXTURE_1D, toSSD);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 1, 0, GL_RED, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_1D, 0);

    // Vertex Array Object
    glGenVertexArrays(1, &vao);

    // Helper framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toSSD, 0);

    // Check framebuffer
    if (GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qCritical() << "OpenGL framebuffer error" << QString::number(err);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Settings
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    //glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
}

/**
 * @brief Render function - main computation of metric
 */
void SSDComputingOpenGL::render()
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

    ssdFloat = renderGPU();

    //qDebug() << "SSD:" << ssdFloat;

    result = ssdFloat;
}

/**
 * @brief Computes SSD
 * @return SSD value
 */
float SSDComputingOpenGL::renderGPU()
{
    float ssd = 0;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toSSD, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 1, 1);

    sumOfSquaredDifferences->program->bind();
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, toInput);
    sumOfSquaredDifferences->program->setUniformValue(sumOfSquaredDifferences->uInput, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
    sumOfSquaredDifferences->program->setUniformValue(sumOfSquaredDifferences->uRenderingOutput, 1);

    glDrawArrays(GL_POINTS, 0, imageHeight);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    sumOfSquaredDifferences->program->release();

    glReadPixels(0, 0, 1, 1, GL_RED, GL_FLOAT, &ssd);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return ssd;
}
}
