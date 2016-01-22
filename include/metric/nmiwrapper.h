/**
 * @file        nmiwrapper.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with NMIWrapper class declaration.
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

#ifndef SSIMR_NMIWRAPPER_H
#define SSIMR_NMIWRAPPER_H

#include "../ssimrenderer_global.h"

#include "metricwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The NMIWrapper class represents the wrapper for NMI metric classes
 */
class SHARED_EXPORT NMIWrapper : public MetricWrapper
{
public:
    // Creates a NMIWrapper object with optional parental OpenGLWrapper
    NMIWrapper(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Creates a NMIWrapper object with bins count optional parental OpenGLWrapper
    NMIWrapper(GLuint binsCount, OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of NMIWrapper object
    virtual ~NMIWrapper();

    /// Pure virtual function for setting of input image
    virtual void setInputImage(const QImage &image) = 0;

    /// Pure virtual function for setting of rendering output image
    virtual void setRenderingOutputImage(const QImage &image) = 0;

    // Sets bins count for histograms
    virtual void setHistogramBinsCount(int binsCount);

    /// Pure virtual function for getting of joint histogram image
    virtual QImage getJointHistogramImage() = 0;

    /// Pure virtual function for getting histrogram  of input image
    virtual QImage getInputImageHistogramImage() = 0;

    /// Pure virtual function for getting histrogram of rendering output image
    virtual QImage getRenderingOutputImageHistogramImage() = 0;

    // Returns computed NMI value
    virtual float getNMI() const final;

    /// Default Number of bins for histogram generation
    static const int BINS = 256;

protected:
    // Initializes members
    virtual void init() final;

    // Sets Normalized output unit
    virtual void setNormalizedOutputUnit();

    /// Histogram Bin Size
    GLuint binsCount;

    /// Normalized output unit (image size ratio)
    float normalizedOutputUnit;

    /// Variable for joint entropy
    float jH;

    /// Entropy of input image
    float h0;

    /// Entropy of rendering output image
    float h1;

private:
    Q_DISABLE_COPY(NMIWrapper)
};
}

#endif // SSIMR_NMIWRAPPER_H
