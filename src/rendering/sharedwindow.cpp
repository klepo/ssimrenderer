/**
 * @file        sharedwindow.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The implementation file containing the SharedWindow class.
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

#include "rendering/sharedwindow.h"

namespace SSIMRenderer
{
/**
 * @brief Creates SharedWindow using optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
SharedWindow::SharedWindow(OpenGLWrapper *parentOpenGLWrapper)
    : Window(parentOpenGLWrapper)
{

}

/**
 * @brief Destructor of SharedWindow object
 *
 * Does nothing.
*/
SharedWindow::~SharedWindow()
{

}

/**
 * @brief Sets first pyramid do shared window
 * @param[in] pyramid Pyramid object
 */
void SharedWindow::setPyramid1(const SSIMRenderer::Pyramid &pyramid)
{
    pyramid1 = pyramid;
}

/**
 * @brief Sets second pyramid to shared window
 * @param[in] pyramid Pyramid object
 */
void SharedWindow::setPyramid2(const SSIMRenderer::Pyramid &pyramid)
{
    pyramid2 = pyramid;
}

/**
 * @brief Catches mouse move events
 * @param[in] event QMouseEvent object
 */
void SharedWindow::mouseMoveEvent(QMouseEvent *event)
{
    //mousePositionCurrent = event->pos();
    if (mouseButtonIsDown) {
        // Change current position
        mousePositionClickedCurrentMoved = event->pos();

        // Rotation of scene by left mouse click
        if (mouseButtonLeft) {
            rotateCamera((float) (mousePositionClickedLast.y() - mousePositionClickedCurrentMoved.y()) / 2.0f, (float) (mousePositionClickedLast.x() - mousePositionClickedCurrentMoved.x()) / 2.0f, clickedObjectPosition);
        }

        // Move of scene by right mouse click
        if (mouseButtonRight) {
            QPointF lastPosR = convertPointToOpenGLRelative(mousePositionClickedLast);
            QPointF currentPosR = convertPointToOpenGLRelative(mousePositionClickedCurrentMoved);
            QVector3D currentViewportCoords = computeViewportCoords(getTranslation());
            currentViewportCoords.setX(currentViewportCoords.x() - (lastPosR.x() - currentPosR.x()));
            currentViewportCoords.setY(currentViewportCoords.y() + (lastPosR.y() - currentPosR.y()));
            setTranslation(computeObjectCoords(currentViewportCoords));
            emit positionChanged();
        }

        // Render
        renderLater();
    }
}

/**
 * @brief Catches mouse press events
 * @param[in] event QMouseEvent object
 */
void SharedWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::MiddleButton) {
        setCamera(getPerspective());
        renderLater();
    } else {
        Window::mousePressEvent(event);
    }
}

/**
 * @brief Catches mouse wheel events
 * @param[in] event QMouseEvent object
 */
void SharedWindow::wheelEvent(QWheelEvent *event)
{
    QPointF currentPosR = convertPointToOpenGLRelative(event->pos());
    QVector3D currentViewportCoords;
    currentViewportCoords.setX(currentPosR.x());
    currentViewportCoords.setY(- currentPosR.y());
    currentViewportCoords.setZ(0);
    moveCamera(- event->delta(), computeObjectCoords(currentViewportCoords));
    renderLater();
}

/**
 * @brief Overrides render function with rendering of 2 pyramids
 */
void SharedWindow::render()
{
    // background color
    glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    prepareRendering();

    // Must be first!!! - depth culling
    if (isPyramidEnabled()) {
        renderPyramid(pyramid1);
        renderPyramid(pyramid2);
    }

    if (isDensityEnabled()) renderDensity();

    if (isPolygonalEnabled()) renderPolygonal();

    if (isSilhouettesEnabled()) renderSilhouettes();

    renderPostprocessing();
}
}
