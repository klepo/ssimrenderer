/**
 * @file        offscreenrenderer.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The header file with OffscreenRenderer class declaration.
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

#ifndef SSIMR_OFFSCREENRENDERER_H
#define SSIMR_OFFSCREENRENDERER_H

#include "../ssimrenderer_global.h"

#include "mainrenderer.h"

namespace SSIMRenderer
{
/**
 * @brief The OffscreenRenderer class represents the structure for offscreen rendering
 */
class SHARED_EXPORT OffscreenRenderer : public QOffscreenSurface, public MainRenderer
{
public:
    // Creates OffscreenRenderer render with given width and height and optional parental OpenGLWrapper
    OffscreenRenderer(GLuint renderWidth, GLuint renderHeight, OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of OffscreenRenderer object
    virtual ~OffscreenRenderer();

public slots:
    // Mesh, vertices, coefficients slots
    void setMesh(Mesh *mesh, GLfloat *colors = NULL);

    void setCoefficients(StatisticalData *statisticalData);
    void setVertices(StatisticalData *statisticalData);

    void updateCoefficients(StatisticalData *statisticalData);
    void updateVertices(StatisticalData *statisticalData);

    // Rendering params slots
    void setIntensity(double value);
    void setLineWidth(double value);
    void setParam(double value);

    void setRenderWidth(GLuint renderWidth);
    void setRenderHeight(GLuint renderHeight);

    // Rendering settings slots
    void enableDensity(bool value);
    void enableSilhouettes(bool value);
    void enablePyramid(bool value);
    void enablePolygonal(bool value);
    void enableXMirroring(bool value);
    void enablePolygonalLighting(bool value);

private:
    Q_DISABLE_COPY(OffscreenRenderer)

};
}

#endif // SSIMR_OFFSCREENRENDERER_H
