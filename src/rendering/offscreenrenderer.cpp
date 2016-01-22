/**
 * @file        offscreenrenderer.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The implementation file containing the OffscreenRenderer class.
 *
 * Rendering without window.
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

#include "rendering/offscreenrenderer.h"

namespace SSIMRenderer
{
/**
 * @brief Creates OffscreenRenderer using width, height and optional parental OpenGLWrapper
 * @param[in] renderWidth Render width
 * @param[in] renderHeight Render height
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
OffscreenRenderer::OffscreenRenderer(GLuint renderWidth, GLuint renderHeight, OpenGLWrapper *parentOpenGLWrapper)
    : MainRenderer((QOffscreenSurface *) this, renderWidth, renderHeight, parentOpenGLWrapper)
{

}

/**
 * @brief Destructor of OffscreenRenderer object
 *
 * Does nothing.
 */
OffscreenRenderer::~OffscreenRenderer()
{

}

/**
 * @brief Sets mesh and optional colors
 * @param[in] mesh Mesh
 * @param[in] colors An array with colors
 */
void OffscreenRenderer::setMesh(Mesh *mesh, GLfloat *colors)
{
    MainRenderer::setMesh(mesh, colors);
}

/**
 * @brief Sets statistical coefficients data for recomputing
 * @param[in] statisticalData Statistical coefficients data
 *
 * Schedules fast recomputing in rendering function.
 */
void OffscreenRenderer::setCoefficients(StatisticalData *statisticalData)
{
    MainRenderer::setCoefficients(statisticalData);
}

/**
 * @brief Sets statistical vertices data for recomputing
 * @param[in] statisticalData Statistical vertices data
 *
 * Schedules fast recomputing in rendering function.
 */
void OffscreenRenderer::setVertices(StatisticalData *statisticalData)
{
    MainRenderer::setVertices(statisticalData);
}

/**
 * @brief Updates statistical coefficients data for recomputing
 * @param[in] statisticalData Statistical coefficients data
 *
 * Schedules fast recomputing in rendering function.
 */
void OffscreenRenderer::updateCoefficients(StatisticalData *statisticalData)
{
    MainRenderer::updateCoefficients(statisticalData);
}

/**
 * @brief Updates statistical vertices data for recomputing
 * @param[in] statisticalData Statistical vertices data
 *
 * Schedules fast recomputing in rendering function.
 */
void OffscreenRenderer::updateVertices(StatisticalData *statisticalData)
{
    MainRenderer::updateVertices(statisticalData);
}

/**
 * @brief Sets intensity for density rendering
 * @param[in] value Intesity value from 0.0 to 1.0
 */
void OffscreenRenderer::setIntensity(double value)
{
    MainRenderer::setIntensity(value);
}

/**
 * @brief Sets line width for silhouettes rendering
 * @param[in] value Line width from 1 to 10
 */
void OffscreenRenderer::setLineWidth(double value)
{
    MainRenderer::setLineWidth(value);
}

/**
 * @brief Sets special param for density rendering - unused
 * @param[in] value Some value
 */
void OffscreenRenderer::setParam(double value)
{
    MainRenderer::setParam(value);
}

/**
 * @brief Sets render width
 * @param[in] renderWidth Render width
 */
void OffscreenRenderer::setRenderWidth(GLuint renderWidth)
{
    MainRenderer::setRenderWidth(renderWidth);
}

/**
 * @brief Sets render height
 * @param[in] renderHeight Render height
 */
void OffscreenRenderer::setRenderHeight(GLuint renderHeight)
{
    MainRenderer::setRenderHeight(renderHeight);
}

/**
 * @brief Enables or disables density rendering
 * @param[in] value Boolean flag
 */
void OffscreenRenderer::enableDensity(bool value)
{
    MainRenderer::enableDensity(value);
}

/**
 * @brief Enables or disables silhouettes rendering
 * @param[in] value Boolean flag
 */
void OffscreenRenderer::enableSilhouettes(bool value)
{
    MainRenderer::enableSilhouettes(value);
}

/**
 * @brief Enables or disables pyramid rendering
 * @param[in] value Boolean flag
 */
void OffscreenRenderer::enablePyramid(bool value)
{
    MainRenderer::enablePyramid(value);
}

/**
 * @brief Enables or disables polygonal rendering
 * @param[in] value Boolean flag
 */
void OffscreenRenderer::enablePolygonal(bool value)
{
    MainRenderer::enablePolygonal(value);
}

/**
 * @brief Enables or disables x-axis mirroring
 * @param[in] value Boolean flag
 */
void OffscreenRenderer::enableXMirroring(bool value)
{
    MainRenderer::enableXMirroring(value);
}

/**
 * @brief Enables or disables polygonal lighting
 * @param[in] value Boolean flag
 */
void OffscreenRenderer::enablePolygonalLighting(bool value)
{
    MainRenderer::enablePolygonalLighting(value);
}
}
