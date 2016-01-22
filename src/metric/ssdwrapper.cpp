/**
 * @file        ssdwrapper.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the SSDWrapper class.
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

#include "metric/ssdwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a SSDWrapper object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
SSDWrapper::SSDWrapper(OpenGLWrapper *parentOpenGLWrapper)
    : MetricWrapper(parentOpenGLWrapper)
{
    init();
}

/**
 * @brief Destructor of SSDWrapper object
 *
 * Does nothing
 */
SSDWrapper::~SSDWrapper()
{

}

/**
 * @brief Returns computed SSD value
 * @return Computed SSD value
 */
float SSDWrapper::getSSD() const
{
    return result;
}

/**
 * @brief Initializes members
 */
void SSDWrapper::init()
{
    imageWidth = 1;
    imageHeight = 1;
    result = 0;
    inputImageLoaded = false;
    renderingOutputImageLoaded = false;
}
}
