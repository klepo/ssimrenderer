/**
 * @file        ssdcomputingcpu.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The header file with SSDComputingCPU class declaration.
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

#ifndef SSIMR_SSDCOMPUTINGCPU_H
#define SSIMR_SSDCOMPUTINGCPU_H

#include "../ssimrenderer_global.h"

#include "ssdwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The SSDComputingCPU class represents the structure for SSD metric computing on CPU
 */
class SHARED_EXPORT SSDComputingCPU : public SSDWrapper
{
public:
    // Creates SSDComputingCPU object with optional parental OpenGLWrapper
    SSDComputingCPU(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of SSDComputingCPU object
    virtual ~SSDComputingCPU();

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
    float renderCPU();

    // Other stuff
    QImage inputImage;
    QImage renderingOutputImage;

    Q_DISABLE_COPY(SSDComputingCPU)
};
}

#endif // SSIMR_SSDCOMPUTINGCPU_H
