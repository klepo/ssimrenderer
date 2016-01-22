/**
 * @file        ssdwrapper.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with SSDWrapper class declaration.
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

#ifndef SSIMR_SSDWRAPPER_H
#define SSIMR_SSDWRAPPER_H

#include "../ssimrenderer_global.h"

#include "metricwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The SSDWrapper class represents the wrapper for SSD metric classes
 */
class SHARED_EXPORT SSDWrapper : public MetricWrapper
{
public:
    // Creates a SSDWrapper object with optional parental OpenGLWrapper
    SSDWrapper(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of SSDWrapper object
    virtual ~SSDWrapper();

    /// Pure virtual function for setting of input image
    virtual void setInputImage(const QImage &image) = 0;

    /// Pure virtual function for setting of rendering input image
    virtual void setRenderingOutputImage(const QImage &image) = 0;

    // Returns computed SSD value
    virtual float getSSD() const final;

protected:
    // Initializes members
    virtual void init() final;

private:
    Q_DISABLE_COPY(SSDWrapper)
};
}

#endif // SSIMR_SSDWRAPPER_H
