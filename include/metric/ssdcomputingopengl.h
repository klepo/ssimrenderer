/**
 * @file        ssdcomputingopengl.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with SSDComputingOpenGL class declaration.
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

#ifndef SSIMR_SSDCOMPUTINGOPENGL_H
#define SSIMR_SSDCOMPUTINGOPENGL_H

#include "../ssimrenderer_global.h"

#include "ssdwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The SSDComputingOpenGL class represents the structure for SSD metric computing on GPU with OpenGL
 */
class SHARED_EXPORT SSDComputingOpenGL : public SSDWrapper
{
public:
    // Creates SSDComputingOpenGL object with optional parental OpenGLWrapper
    SSDComputingOpenGL(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of SSDComputingOpenGL object
    ~SSDComputingOpenGL();

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
    float renderGPU();

    // Computing SSD
    struct SumOfSquaredDifferences {
        QOpenGLShaderProgram *program;
        GLuint uInput;
        GLuint uRenderingOutput;
        GLuint uWidth;
    } *sumOfSquaredDifferences;

    // Frame Buffer Object
    GLuint fbo;

    // Vertex Array Object
    GLuint vao;

    // Texture Objects
    GLuint toSSD;
    GLuint toInput;

    Q_DISABLE_COPY(SSDComputingOpenGL)
};
}

#endif // SSIMR_SSDCOMPUTINGOPENGL_H
