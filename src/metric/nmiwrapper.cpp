/**
 * @file        nmiwrapper.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the NMIWrapper class.
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

#include "metric/nmiwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a NMIWrapper object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
NMIWrapper::NMIWrapper(OpenGLWrapper *parentOpenGLWrapper)
    : MetricWrapper(parentOpenGLWrapper)
{
    init();
}

/**
 * @brief Creates a NMIWrapper object with optional parental OpenGLWrapper
 * @param[in] binsCount Bins count for histograms
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
NMIWrapper::NMIWrapper(GLuint binsCount, OpenGLWrapper *parentOpenGLWrapper)
    : MetricWrapper(parentOpenGLWrapper)
{
    init();
    setHistogramBinsCount(binsCount);
}

/**
 * @brief Destructor of NMIWrapper object
 *
 * Does nothing.
 */
NMIWrapper::~NMIWrapper()
{

}

/**
 * @brief Sets bins count for histograms
 * @param[in] binsCount Bins count for histograms
 */
void NMIWrapper::setHistogramBinsCount(int binsCount)
{
    this->binsCount = binsCount;
}

/**
 * @brief Returns computed NMI value
 * @return Computed NMI value
 */
float NMIWrapper::getNMI() const
{
    return result;
}

/**
 * @brief Initializes members
 */
void NMIWrapper::init()
{
    imageWidth = 1;
    imageHeight = 1;
    result = 0;
    binsCount = BINS;
    normalizedOutputUnit = 1;
    inputImageLoaded = false;
    renderingOutputImageLoaded = false;
    jH = 1;
    h0 = 1;
    h1 = 1;
}

/**
 * @brief Sets Normalized output unit from image width and height
 */
void NMIWrapper::setNormalizedOutputUnit()
{
    normalizedOutputUnit = float (1.0f / (imageWidth * imageHeight));
}
}
