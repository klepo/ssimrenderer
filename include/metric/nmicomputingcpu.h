/**
 * @file        nmicomputingcpu.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with NMIComputingCPU class declaration.
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

#ifndef SSIMR_NMICOMPUTINGCPU_H
#define SSIMR_NMICOMPUTINGCPU_H

#include "../ssimrenderer_global.h"

#include "nmiwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The NMIComputingCPU class represents the structure for NMI metric computing on CPU
 */
class SHARED_EXPORT NMIComputingCPU : public NMIWrapper
{
public:
    // Creates NMIComputingCPU object with bins count and optional parental OpenGLWrapper
    NMIComputingCPU(GLuint binsCount, OpenGLWrapper *parentOpenGLWrapper = 0);

    // Creates NMIComputingCPU object with optional parental OpenGLWrapper
    NMIComputingCPU(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of NMIComputingCPU object
    virtual ~NMIComputingCPU();

    // Setting of input image
    virtual void setInputImage(const QImage &image);

    // Setting of rendering output image
    virtual void setRenderingOutputImage(const QImage &image);

    // Sets bins count for histograms
    virtual void setHistogramBinsCount(int binsCount);

    // Returns joint histogram image
    virtual QImage getJointHistogramImage();

    // Returns histogram of input image
    virtual QImage getInputImageHistogramImage();

    // Returns histogram of rendering output image
    virtual QImage getRenderingOutputImageHistogramImage();

protected:
    // Initialize function
    virtual void initialize();

    // Render function
    virtual void render();

private:
    void renderJointHistogramCPU();

    void renderHistogramCPU(bool flag, float *histogram);

    float renderEntropyCPU(float *histogram, bool jointFlag = false);

    // Other stuff
    QImage inputImage;
    QImage renderingOutputImage;

    float *jointHistogram;
    float *histogramInputImage;
    float *histogramRenderingOutputImage;

    Q_DISABLE_COPY(NMIComputingCPU)
};
}

#endif // SSIMR_NMICOMPUTINGCPU_H
