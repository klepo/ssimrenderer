/**
 * @file        sharedwindow.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The header file with SharedWindow class declaration.
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

#ifndef SSIMR_SHAREDWINDOW_H
#define SSIMR_SHAREDWINDOW_H

#include "../ssimrenderer_global.h"

#include "window.h"

namespace SSIMRenderer
{
/**
 * @brief The SharedWindow class represents the structure for shared window (shared context) rendering
 */
class SHARED_EXPORT SharedWindow : public Window
{
public:
    // Creates SharedWindow using optional parental OpenGLWrapper
    SharedWindow(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of SharedWindow object
    ~SharedWindow();

    // Set Pyramid (two perspectives)
    void setPyramid1(const SSIMRenderer::Pyramid &pyramid);
    void setPyramid2(const SSIMRenderer::Pyramid &pyramid);

protected:
    // Modified render function - two perspectives
    virtual void render();

    // Modified Window events
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    SSIMRenderer::Pyramid pyramid1;
    SSIMRenderer::Pyramid pyramid2;
    QVector3D cameraPosition;

    Q_DISABLE_COPY(SharedWindow)
};
}

#endif // SSIMR_SHAREDWINDOW_H
