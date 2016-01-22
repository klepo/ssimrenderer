/**
 * @file        metricwrapper.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        25 July 2015
 *
 * @brief       The implementation file containing the MetricWrapper class.
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

#include "metric/metricwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a MetricWrapper object with optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
MetricWrapper::MetricWrapper(OpenGLWrapper *parentOpenGLWrapper)
    : OpenGLWrapper(this, parentOpenGLWrapper)
{

}

/**
 * @brief Destructor of MetricWrapper object
 *
 * Does nothing.
 */
MetricWrapper::~MetricWrapper()
{

}

/**
 * @brief Computes metric, calls renderNow()
 * @return Metric result
 */
float MetricWrapper::compute()
{
    renderNow();
    return result;
}

/**
 * @brief Returns image height
 * @return Image height
 */
int MetricWrapper::getImageHeight() const
{
    return imageHeight;
}

/**
 * @brief Returns image width
 * @return Image width
 */
int MetricWrapper::getImageWidth() const
{
    return imageWidth;
}
}
