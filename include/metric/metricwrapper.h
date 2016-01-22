/**
 * @file        metricwrapper.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with MetricWrapper class declaration.
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

#ifndef SSIMR_METRICWRAPPER_H
#define SSIMR_METRICWRAPPER_H

#include "../ssimrenderer_global.h"

#include "../opengl/openglwrapper.h"
#include "../rendering/mainrenderer.h"

namespace SSIMRenderer
{
/**
 * @brief The MetricWrapper class represents the wrapper for metric classes
 *
 * This is pure virtual class. Derived classes have to implement some methods.
 */
class SHARED_EXPORT MetricWrapper : public QOffscreenSurface, public OpenGLWrapper
{
public:
    // Creates a MetricWrapper object with optional parental OpenGLWrapper
    MetricWrapper(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of MetricWrapper object
    virtual ~MetricWrapper();

    /// Pure virtual function for setting of input image
    virtual void setInputImage(const QImage &image) = 0;

    /// Pure virtual function for setting of rendering input image
    virtual void setRenderingOutputImage(const QImage &image) = 0;

    // Compute metric function
    virtual float compute() final;

    // Returns image width
    virtual int getImageWidth() const final;

    // Returns image height
    virtual int getImageHeight() const final;

protected:
    /// Pure virtual function for initialization
    virtual void init() = 0;

    /// Result of metric
    float result;

    /// Shared texture ID
    GLuint toRenderingOutput;

    /// Input image loaded flag
    bool inputImageLoaded;

    /// Rendering output image loaded flag
    bool renderingOutputImageLoaded;

    /// Image width
    int imageWidth;

    /// Image height
    int imageHeight;

private:
    Q_DISABLE_COPY(MetricWrapper)
};
}

#endif // SSIMR_METRICWRAPPER_H
