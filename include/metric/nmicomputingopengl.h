/**
 * @file        nmicomputingopengl.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with NMIComputingOpenGL class declaration.
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

#ifndef SSIMR_NMICOMPUTINGOPENGL_H
#define SSIMR_NMICOMPUTINGOPENGL_H

#include "../ssimrenderer_global.h"

#include "nmiwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The NMIComputingOpenGL class represents the structure for NMI metric computing on GPU with OpenGL
 */
class SHARED_EXPORT NMIComputingOpenGL : public NMIWrapper
{
public:
    // Creates NMIComputingOpenGL object with bins count and optional parental OpenGLWrapper
    NMIComputingOpenGL(GLuint binsCount, OpenGLWrapper *parentOpenGLWrapper = 0);

    // Creates NMIComputingOpenGL object with optional parental OpenGLWrapper
    NMIComputingOpenGL(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of NMIComputingOpenGL object
    ~NMIComputingOpenGL();

    // Setting of input image
    virtual void setInputImage(const QImage &image);

    // Setting of rendering output image
    virtual void setRenderingOutputImage(const QImage &image);

    // Sets bins count for histograms
    virtual void setHistogramBinsCount(int binsCount);

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
    virtual void setNormalizedOutputUnit();

    void renderJointHistogramGPU();

    void renderHistogramGPU(bool flag, GLuint textureHistogramId);

    void renderEntropyGPU(float *h);

    // Computing histogram
    struct Histogram {
        QOpenGLShaderProgram *program;
        GLuint uFlag;
        GLuint uJointHistogram;
        GLuint uRow;
        GLuint uBins;
    } *histogram;

    // Computing joint histogram
    struct JointHistogram {
        QOpenGLShaderProgram *program;
        GLuint uNormalizedOutputUnit;
        //GLuint aPixel0;
        //GLuint aPixel1;
        GLuint uInput;
        GLuint uRenderingOutput;
        GLuint uRow;
        //GLuint uBins;
    } *jointHistogram;

    // Computing entropy
    struct Entropy {
        QOpenGLShaderProgram *program;
        GLuint uHistogram0;
        GLuint uHistogram1;
        GLuint uJointHistogram;
        //GLuint uRow;
        GLuint uBins;
    } *entropy;

    // Frame Buffer Object
    GLuint fbo;

    // Vertex Array Object
    GLuint vao;

    // Vertex Buffer Objects
    GLuint vboInput;
    GLuint vboRenderingOutput;

    // Texture Objects
    GLuint toHistogramRenderingOutput;
    GLuint toHistogramInput;
    GLuint toJointHistogram;

    GLuint toEntropy;

    GLuint toInput;

    // Other stuff
    //QImage inputImage;
    //QImage renderingOutputImage;

    Q_DISABLE_COPY(NMIComputingOpenGL)
};
}

#endif // SSIMR_NMICOMPUTINGOPENGL_H
