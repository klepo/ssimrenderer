/**
 * @file        nmicomputingopengl.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the NMIComputingOpenGL class.
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

#include "metric/nmicomputingopengl.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a NMIComputingOpenGL object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
NMIComputingOpenGL::NMIComputingOpenGL(OpenGLWrapper *parentOpenGLWrapper)
    : NMIWrapper(parentOpenGLWrapper)
{

}

/**
 * @brief Creates a NMIComputingOpenGL object with optional parental OpenGLWrapper
 * @param[in] binsCount Bins count for histograms
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
NMIComputingOpenGL::NMIComputingOpenGL(GLuint binsCount, OpenGLWrapper *parentOpenGLWrapper)
    : NMIWrapper(binsCount, parentOpenGLWrapper)
{

}

/**
 * @brief Destructor of NMIComputingOpenGL object
 *
 * Releases some OpenGL memory objects
 */
NMIComputingOpenGL::~NMIComputingOpenGL()
{
    checkInitAndMakeCurrentContext();

    //histogram->program->release();
    delete histogram->program;
    delete histogram;

    //jointHistogram->program->release();
    delete jointHistogram->program;
    delete jointHistogram;

    //jointHistogram->program->release();
    delete entropy->program;
    delete entropy;

    glDeleteFramebuffers(1, &fbo);
    glDeleteVertexArrays(1, &vao);

    glDeleteRenderbuffers(1, &vboInput);
    glDeleteRenderbuffers(1, &vboRenderingOutput);

    glDeleteTextures(1, &toHistogramRenderingOutput);
    glDeleteTextures(1, &toHistogramInput);
    glDeleteTextures(1, &toJointHistogram);
    glDeleteTextures(1, &toEntropy);

    glDeleteTextures(1, &toInput);

    if (!hasSharedContext())
        glDeleteTextures(1, &toRenderingOutput);
}

/**
 * @brief Sets of input image
 * @param[in] image Input image
 */
void NMIComputingOpenGL::setInputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    QImage inputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = inputImage.width();
    imageHeight = inputImage.height();

    /*glBindBuffer(GL_ARRAY_BUFFER, vboInput);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(GLubyte) * imageWidth * imageHeight * 4, inputImage.bits(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    glBindTexture(GL_TEXTURE_2D, toInput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, inputImage.bits());
    glBindTexture(GL_TEXTURE_2D, 0);

    setNormalizedOutputUnit();

    inputImageLoaded = true;
}

/**
 * @brief Sets of rendering output image
 * @param[in] image Rendering output image
 */
void NMIComputingOpenGL::setRenderingOutputImage(const QImage &image)
{
    checkInitAndMakeCurrentContext();

    //@todo TODO check dimensions of both images
    QImage renderingOutputImage = image.mirrored().convertToFormat(QImage::Format_RGBA8888);
    imageWidth = renderingOutputImage.width();
    imageHeight = renderingOutputImage.height();

    /*glBindBuffer(GL_ARRAY_BUFFER, vboRenderingOutput);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(GLubyte) * imageWidth * imageHeight * 4, renderingOutputImage.bits(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, renderingOutputImage.bits());
    glBindTexture(GL_TEXTURE_2D, 0);

    setNormalizedOutputUnit();

    renderingOutputImageLoaded = true;
}

/**
 * @brief Sets bins count for histograms
 * @param[in] binsCount Bins count for histograms
 */
void NMIComputingOpenGL::setHistogramBinsCount(int binsCount)
{
    this->binsCount = binsCount;

    checkInitAndMakeCurrentContext();

    glBindTexture(GL_TEXTURE_2D, toHistogramRenderingOutput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, this->binsCount, 1, 0, GL_RED, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, toHistogramInput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, this->binsCount, 1, 0, GL_RED, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, toJointHistogram);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, this->binsCount, this->binsCount, 0, GL_RED, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    histogram->program->bind();
    histogram->program->setUniformValue(histogram->uBins, this->binsCount);
    histogram->program->release();

    entropy->program->bind();
    entropy->program->setUniformValue(entropy->uBins, this->binsCount);
    entropy->program->release();
}

/**
 * @brief Returns joint histrogram image
 * @return Joint histogram image
 */
QImage NMIComputingOpenGL::getJointHistogramImage()
{
    checkInitAndMakeCurrentContext();

    QImage image = QImage(binsCount, binsCount, QImage::Format_RGB888);
    glBindTexture(GL_TEXTURE_2D, toJointHistogram);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    float *jointHistogram = new float[binsCount * binsCount];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, jointHistogram);

    for (unsigned int x = 0; x < binsCount; x++) {
        for (unsigned int y = 0; y < binsCount; y++) {
            /*if (jointHistogram[y * binsCount + x] != 0)
                qDebug() << x << y << jointHistogram[y * binsCount + x];*/
            float value = jointHistogram[y * binsCount + x] * imageWidth * imageHeight;
            image.setPixel(x, y, qRgb(value, value, value));
        }
    }

    delete[] jointHistogram;

    glBindTexture(GL_TEXTURE_2D, 0);

    return image;
}

/**
 * @brief Returns histogram of input image
 * @return Histogram of input image
 */
QImage NMIComputingOpenGL::getInputImageHistogramImage()
{
    checkInitAndMakeCurrentContext();

    QImage image = QImage(binsCount, 1, QImage::Format_RGB888);
    glBindTexture(GL_TEXTURE_2D, toHistogramInput);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    float *histogram = new float[binsCount];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, histogram);

    for (unsigned int x = 0; x < binsCount; x++) {
        if (histogram[x] != 0)
            qDebug() << x << histogram[x];
        float value = histogram[x] * imageWidth * imageHeight;
        image.setPixel(x, 0, qRgb(value, value, value));
    }

    delete[] histogram;

    glBindTexture(GL_TEXTURE_2D, 0);

    return image;
}

/**
 * @brief Returns histogram of rendering output image
 * @return Histogram of rendering output image
 */
QImage NMIComputingOpenGL::getRenderingOutputImageHistogramImage()
{
    checkInitAndMakeCurrentContext();

    QImage image = QImage(binsCount, 1, QImage::Format_RGB888);
    glBindTexture(GL_TEXTURE_2D, toHistogramRenderingOutput);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    float *histogram = new float[binsCount];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, histogram);

    for (unsigned int x = 0; x < binsCount; x++) {
        if (histogram[x] != 0)
            qDebug() << x << histogram[x];
        float value = histogram[x] * imageWidth * imageHeight;
        image.setPixel(x, 0, qRgb(value, value, value));
    }

    delete[] histogram;

    glBindTexture(GL_TEXTURE_2D, 0);

    return image;
}

/**
 * @brief Initializes OpenGL resources, sets shared OpenGL context and initializes other stuff
 */
void NMIComputingOpenGL::initialize()
{
    // Important for resources in library
    Q_INIT_RESOURCE(shaders);

    histogram = new Histogram();
    histogram->program = 0;

    jointHistogram = new JointHistogram();
    jointHistogram->program = 0;

    entropy = new Entropy();
    entropy->program = 0;

    // Create shaders
    // Program for render histogram
    bool status;
    histogram->program = new QOpenGLShaderProgram();
    status = histogram->program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertexshaderHistogram");
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << histogram->program->log();
    status = histogram->program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragmentshaderHistogram");
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << histogram->program->log();
    status = histogram->program->link();
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << histogram->program->log();

    // Program for render joint histogram
    jointHistogram->program = new QOpenGLShaderProgram();
    status = jointHistogram->program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertexshaderJointHistogram");
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << jointHistogram->program->log();
    status = jointHistogram->program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragmentshaderJointHistogram");
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << jointHistogram->program->log();
    status = jointHistogram->program->link();
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << jointHistogram->program->log();

    // Program for computing entropy
    entropy->program = new QOpenGLShaderProgram();
    status = entropy->program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertexshaderEntropy");
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << entropy->program->log();
    status = entropy->program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragmentshaderEntropy");
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << entropy->program->log();
    status = entropy->program->link();
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader error" << entropy->program->log();

    // Get shaders variables locations
    histogram->uFlag = histogram->program->uniformLocation("uFlag");
    histogram->uJointHistogram = histogram->program->uniformLocation("uJointHistogram");
    histogram->uRow = histogram->program->uniformLocation("uRow");
    histogram->uBins = histogram->program->uniformLocation("uBins");

    jointHistogram->uNormalizedOutputUnit = jointHistogram->program->uniformLocation("uNormalizedOutputUnit");
    //jointHistogram->uBins = jointHistogram->program->uniformLocation("uBins");
    //jointHistogram->aPixel0 = jointHistogram->program->attributeLocation("aPixel0");
    //jointHistogram->aPixel1 = jointHistogram->program->attributeLocation("aPixel1");
    jointHistogram->uRow = jointHistogram->program->uniformLocation("uRow");
    jointHistogram->uInput = jointHistogram->program->uniformLocation("uInput");
    jointHistogram->uRenderingOutput = jointHistogram->program->uniformLocation("uRenderingOutput");

    entropy->uHistogram0 = entropy->program->uniformLocation("uHistogram0");
    entropy->uHistogram1 = entropy->program->uniformLocation("uHistogram1");
    entropy->uJointHistogram = entropy->program->uniformLocation("uJointHistogram");
    //entropy->uRow = entropy->program->uniformLocation("uRow");
    entropy->uBins = entropy->program->uniformLocation("uBins");

    // Vertex buffers
    //glGenBuffers(1, &vboInput);
    //glGenBuffers(1, &vboRenderingOutput);

    // Get shared resources
    /*if (hasSharedContext()) {
        MainRenderer* parentOpenGLWrapper = (MainRenderer *) getParentOpenGLWrapper();
        toRenderingOutput = parentOpenGLWrapper->getOutputTextureId();
        glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imageWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imageHeight);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, vboRenderingOutput);
        glBufferData(GL_PIXEL_PACK_BUFFER,  sizeof(GLubyte) * imageWidth * imageHeight * 4, 0, GL_STATIC_DRAW);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        setNormalizedOutputUnit();

        renderingOutputImageLoaded = true;
    }*/

    // Get shared resources
    if (hasSharedContext()) {
        MainRenderer* parentOpenGLWrapper = (MainRenderer *) getParentOpenGLWrapper();
        toRenderingOutput = parentOpenGLWrapper->getOutputTextureId();
        glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imageWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imageHeight);
        glBindTexture(GL_TEXTURE_2D, 0);

        setNormalizedOutputUnit();

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
    glGenTextures(1, &toHistogramRenderingOutput);
    glBindTexture(GL_TEXTURE_2D, toHistogramRenderingOutput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, binsCount, 1, 0, GL_RED, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &toHistogramInput);
    glBindTexture(GL_TEXTURE_2D, toHistogramInput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, binsCount, 1, 0, GL_RED, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &toJointHistogram);
    glBindTexture(GL_TEXTURE_2D, toJointHistogram);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, binsCount, binsCount, 0, GL_RED, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &toEntropy);
    glBindTexture(GL_TEXTURE_1D, toEntropy);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 1, 0, GL_RGBA, GL_FLOAT, 0);
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
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toJointHistogram, 0);

    // Check framebuffer
    if (GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qCritical() << "OpenGL framebuffer error" << QString::number(err);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    setHistogramBinsCount(binsCount);

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
void NMIComputingOpenGL::render()
{
    if (!inputImageLoaded) {
        qCritical() << "First image is not loaded!";
        return;
    }

    if (!renderingOutputImageLoaded) {
        qCritical() << "Second image is not loaded!";
        return;
    }

    /*if (hasSharedContext()) {
        glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imageWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imageHeight);
        setNormalizedOutputUnit();
        glBindBuffer(GL_PIXEL_PACK_BUFFER, vboRenderingOutput);
        glBufferData(GL_PIXEL_PACK_BUFFER,  sizeof(GLubyte) * imageWidth * imageHeight * 4, 0, GL_STATIC_DRAW);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }*/

    renderJointHistogramGPU();
    renderHistogramGPU(0, toHistogramInput);
    renderHistogramGPU(1, toHistogramRenderingOutput);

    float h[4] = {0};

    renderEntropyGPU(h);

    h0 = -h[0];
    h1 = -h[1];
    jH = -h[2];

    //qDebug() << "joint entropy:" << jH;
    //qDebug() << "entropy 0:" << h0;
    //qDebug() << "entropy 1:" << h1;

    result = (h0 + h1) / jH;
    //qDebug() << "NMI:" << nmi;
}

/**
 * @brief NMIComputingOpenGL::setNormalizedOutputUnit
 */
void NMIComputingOpenGL::setNormalizedOutputUnit()
{
    normalizedOutputUnit = float (1.0f / (imageWidth * imageHeight));

    jointHistogram->program->bind();
    jointHistogram->program->setUniformValue(jointHistogram->uNormalizedOutputUnit, normalizedOutputUnit);
    jointHistogram->program->release();
}

/**
 * @brief Makes joint histogram
 */
void NMIComputingOpenGL::renderJointHistogramGPU()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toJointHistogram, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, binsCount, binsCount);

    jointHistogram->program->bind();
    glBindVertexArray(vao);

    /*glBindBuffer(GL_ARRAY_BUFFER, vboInput);
    glEnableVertexAttribArray(jointHistogram->aPixel0);
    glVertexAttribIPointer(jointHistogram->aPixel0, 4, GL_UNSIGNED_BYTE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboRenderingOutput);
    glEnableVertexAttribArray(jointHistogram->aPixel1);
    glVertexAttribIPointer(jointHistogram->aPixel1, 4, GL_UNSIGNED_BYTE, 0, 0);*/

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, toInput);
    jointHistogram->program->setUniformValue(jointHistogram->uInput, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, toRenderingOutput);
    jointHistogram->program->setUniformValue(jointHistogram->uRenderingOutput, 1);

    for (int i = 0; i < imageHeight; i++) {
        jointHistogram->program->setUniformValue(jointHistogram->uRow, i);
        glDrawArrays(GL_POINTS, 0, imageWidth);
    }

    //glDrawArrays(GL_POINTS, 0, imageHeight * imageWidth);

    //glDisableVertexAttribArray(jointHistogram->aPixel0);
    //glDisableVertexAttribArray(jointHistogram->aPixel1);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    jointHistogram->program->release();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

/**
 * @brief Makes histogram from joint histogram
 * @param[in] flag Compute first or second histogram
 * @param[in] textureHistogramId Id of output histogram texture
 */
void NMIComputingOpenGL::renderHistogramGPU(bool flag, GLuint textureHistogramId)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureHistogramId, 0);

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, binsCount, 1);

    histogram->program->bind();

    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, toJointHistogram);
    histogram->program->setUniformValue(histogram->uJointHistogram, 0);
    histogram->program->setUniformValue(histogram->uFlag, flag);

    for (unsigned int i = 0; i < binsCount; i++) {
        histogram->program->setUniformValue(histogram->uRow, i);
        glDrawArrays(GL_POINTS, 0, binsCount);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);

    histogram->program->release();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

/**
 * @brief Computes entropy
 * @param[out] h Output array of length 4 with entropies for all histograms
 * @return Computed entropies
 *
 * h[0] - entropy of histogram of input image
 * h[1] - entropy of histogram of rendering output image
 * h[2] - entropy of joint histogram
 */
void NMIComputingOpenGL::renderEntropyGPU(float *h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toEntropy, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 1, 1);

    entropy->program->bind();
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, toHistogramInput);
    entropy->program->setUniformValue(entropy->uHistogram0, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, toHistogramRenderingOutput);
    entropy->program->setUniformValue(entropy->uHistogram1, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, toJointHistogram);
    entropy->program->setUniformValue(entropy->uJointHistogram, 2);

    /*for (unsigned int i = 0; i < binsCount; i++) {
        entropy->program->setUniformValue(entropy->uRow, i);
        glDrawArrays(GL_POINTS, 0, this->binsCount);
    }*/

    glDrawArrays(GL_POINTS, 0, binsCount);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    entropy->program->release();

    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, h);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
}
