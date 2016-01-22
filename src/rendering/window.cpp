/**
 * @file        window.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The implementation file containing the Window class.
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

#include "rendering/window.h"

namespace SSIMRenderer
{
/**
 * @brief Creates Window using optional parental OpenGLWrapper
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
Window::Window(OpenGLWrapper *parentOpenGLWrapper)
    : MainRenderer((QWindow *) this, parentOpenGLWrapper)
    , mouseButtonIsDown(false)
    , mouseButtonLeft(false)
    , mouseButtonRight(false)
    , updatePending(false)
{

}

/**
 * @brief Destructor of Window object
 *
 * Does nothing.
 */
Window::~Window()
{

}

/**
 * @brief Render with queued events mechanism (QEvent::UpdateRequest)
 *
 * This is usefull for many rendering calls (renderNow()) in same time.
 * Some of them are filtered.
 */
void Window::renderLater()
{
    // Drop some render commands at same time...
    if (!updatePending) {
        updatePending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

/**
 * @brief Sets mesh and optional colors
 * @param[in] mesh Mesh
 * @param[in] colors An array with colors
 */
void Window::setMesh(Mesh *mesh, GLfloat *colors)
{
    MainRenderer::setMesh(mesh, colors);
    renderLater();
}

/**
 * @brief Sets statistical coefficients data for recomputing
 * @param[in] statisticalData Statistical coefficients data
 *
 * Schedules fast recomputing in rendering function.
 */
void Window::setCoefficients(StatisticalData *statisticalData)
{
    MainRenderer::setCoefficients(statisticalData);
    renderLater();
}

/**
 * @brief Sets statistical vertices data for recomputing
 * @param[in] statisticalData Statistical vertices data
 *
 * Schedules fast recomputing in rendering function.
 */
void Window::setVertices(StatisticalData *statisticalData)
{
    MainRenderer::setVertices(statisticalData);
    renderLater();
}

/**
 * @brief Updates statistical coefficients data for recomputing
 * @param[in] statisticalData Statistical coefficients data
 *
 * Schedules fast recomputing in rendering function.
 */
void Window::updateCoefficients(StatisticalData *statisticalData)
{
    MainRenderer::updateCoefficients(statisticalData);
    renderLater();
}

/**
 * @brief Updates statistical vertices data for recomputing
 * @param[in] statisticalData Statistical vertices data
 *
 * Schedules fast recomputing in rendering function.
 */
void Window::updateVertices(StatisticalData *statisticalData)
{
    MainRenderer::updateVertices(statisticalData);
    renderLater();
}

/**
 * @brief Sets intensity for density rendering
 * @param[in] value Intesity value from 0.0 to 1.0
 */
void Window::setIntensity(double value)
{
    MainRenderer::setIntensity(value);
    renderLater();
}

/**
 * @brief Sets line width for silhouettes rendering
 * @param[in] value Line width from 1 to 10
 */
void Window::setLineWidth(double value)
{
    MainRenderer::setLineWidth(value);
    renderLater();
}

/**
 * @brief Sets special param for density rendering - unused
 * @param[in] value Some value
 */
void Window::setParam(double value)
{
    MainRenderer::setParam(value);
    renderLater();
}

/**
 * @brief Enables or disables density rendering
 * @param[in] value Boolean flag
 */
void Window::enableDensity(bool value)
{
    MainRenderer::enableDensity(value);
    renderLater();
}

/**
 * @brief Enables or disables silhouettes rendering
 * @param[in] value Boolean flag
 */
void Window::enableSilhouettes(bool value)
{
    MainRenderer::enableSilhouettes(value);
    renderLater();
}

/**
 * @brief Enables or disables pyramid rendering
 * @param[in] value Boolean flag
 */
void Window::enablePyramid(bool value)
{
    MainRenderer::enablePyramid(value);
    renderLater();
}

/**
 * @brief Enables or disables polygonal rendering
 * @param[in] value Boolean flag
 */
void Window::enablePolygonal(bool value)
{
    MainRenderer::enablePolygonal(value);
    renderLater();
}

/**
 * @brief Enables or disables x-axis mirroring
 * @param[in] value Boolean flag
 */
void Window::enableXMirroring(bool value)
{
    MainRenderer::enableXMirroring(value);
    renderLater();
}

/**
 * @brief Enables or disables polygonal lighting
 * @param[in] value Boolean flag
 */
void Window::enablePolygonalLighting(bool value)
{
    MainRenderer::enablePolygonalLighting(value);
    renderLater();
}

/**
 * @brief Updates translation of model in scene
 * @param[in] x X position
 * @param[in] y Y position
 * @param[in] z Z position
 */
void Window::updateTranslation(float x, float y, float z)
{
    setTranslation(x, y, z);
    emit positionChanged();
}

/**
 * @brief Updates translation of model in scene
 * @param[in] position 3D vector with position
 */
void Window::updateTranslation(const QVector3D &position)
{
    setTranslation(position);
    emit positionChanged();
}

/**
 * @brief Updates rotation of model in scene
 * @param[in] xAngle X angle
 * @param[in] yAngle Y angle
 * @param[in] zAngle Z angle
 */
void Window::updateRotation(float xAngle, float yAngle, float zAngle)
{
    setRotation(xAngle, yAngle, zAngle);
    emit rotationChanged();
}

/**
 * @brief Updates rotation of model in scene
 * @param[in] rotation 3D vector with rotation
 */
void Window::updateRotation(const QVector3D &rotation)
{
    setRotation(rotation);
    emit rotationChanged();
}

/**
 * @brief Updates rotation matrix
 * @param[in] rotationMatrix Rotation matrix
 */
void Window::updateRotationMatrix(const QMatrix4x4 &rotationMatrix)
{
    setRotationMatrix(rotationMatrix);
    emit rotationChanged();
}

/**
 * @brief Catches mainly keyboard events
 * @param[in] event QEvent object
 * @return MainRenderer::event(event)
 */
bool Window::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Z) {
            setRotation(0, 0, 0);
            emit rotationChanged();
        }
        if (keyEvent->key() == Qt::Key_Y) {
            setRotation(90, 0, 0);
            emit rotationChanged();
        }
        if (keyEvent->key() == Qt::Key_X) {
            setRotation(0, -90, 0);
            emit rotationChanged();
        }
        if (keyEvent->key() == Qt::Key_B) {
            setRotation(0, -180, 0);
            emit rotationChanged();
        }
        if (keyEvent->key() == Qt::Key_G) {
            setRotation(90 + 180, 0, 0);
            emit rotationChanged();
        }
        if (keyEvent->key() == Qt::Key_R) {
            setRotation(0, -(90 + 180), 0);
            emit rotationChanged();
        }
        if (keyEvent->key() == Qt::Key_C) {
            setTranslation(getMiddle());
            emit positionChanged();
        }
        if (keyEvent->key() == Qt::Key_Plus) {
            setIntensity(getIntensity() + 0.01f);
        }
        if (keyEvent->key() == Qt::Key_Minus) {
            setIntensity(getIntensity() - 0.01f);
        }
        if (keyEvent->key() == Qt::Key_D) {
            enableDensity(!isDensityEnabled());
        }
        if (keyEvent->key() == Qt::Key_S) {
            enableSilhouettes(!isSilhouettesEnabled());
        }
        if (keyEvent->key() == Qt::Key_Up) {
            setLineWidth(getLineWidth() + 0.001f);
        }
        if (keyEvent->key() == Qt::Key_Down) {
            setLineWidth(getLineWidth() - 0.001f);
        }
        if (keyEvent->key() == Qt::Key_Left) {
            setParam(getParam() + 0.1f);
        }
        if (keyEvent->key() == Qt::Key_Right) {
            setParam(getParam() - 0.1f);
        }
        renderLater();
    }
    case QEvent::UpdateRequest:
        // Render event
        updatePending = false;
        renderNow();
        emit rendered();
        mousePositionClickedLast = mousePositionClickedCurrentMoved;
    }

    //qDebug() << size();
    //qDebug() << getRenderWidth() <<  getRenderHeight();

    if (getRenderHeight() != height() && height() > 0)
        setRenderHeight(height());
    if (getRenderWidth() != width() && width() > 0)
        setRenderWidth(width());

    QWindow::event(event);
    return MainRenderer::event(event);
}

/**
 * @brief Catches expose events
 * @param[in] event QExposeEvent object
 */
void Window::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);
    if (isExposed())
        renderLater();
}

/**
 * @brief Catches resize events
 * @param[in] event QResizeEvent object
 */
void Window::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (isExposed())
        renderLater();
}

/**
 * @brief Catches mouse press events
 * @param[in] event QMouseEvent object
 */
void Window::mousePressEvent(QMouseEvent *event)
{
    // Mouse button type
    if (event->buttons() == Qt::RightButton)
        mouseButtonRight = true;
    if (event->buttons() == Qt::LeftButton)
        mouseButtonLeft = true;
    if (event->buttons() == Qt::MiddleButton) {
        setTranslation(getMiddle());
        emit positionChanged();
    }

    mouseButtonIsDown = true;
    // Save mouse position
    mousePositionClickedLast = event->pos();
    mousePositionClickedCurrent = event->pos();
    QPointF clickedPosR = convertPointToOpenGLRelative(mousePositionClickedCurrent);
    QVector3D currentViewportCoords = computeViewportCoords(getTranslation());
    currentViewportCoords.setX(clickedPosR.x());
    currentViewportCoords.setY(- clickedPosR.y());
    clickedObjectPosition = computeObjectCoords(currentViewportCoords);
    mousePositionClickedCurrentMoved = event->pos();

    renderLater();
}

/**
 * @brief Catches mouse move events
 * @param[in] event QMouseEvent object
 */
void Window::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "---> Event" << "mouseMoveEvent" << event->pos();
    //mousePositionCurrent = event->pos();
    if (mouseButtonIsDown) {
        // Change current position
        mousePositionClickedCurrentMoved = event->pos();

        // Rotation of scene by left mouse click
        if (mouseButtonLeft) {
            rotate((float) (mousePositionClickedLast.y() - mousePositionClickedCurrentMoved.y()) / 2.0f, (float) (mousePositionClickedLast.x() - mousePositionClickedCurrentMoved.x()) / 2.0f, 0);
            emit rotationChanged();
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
 * @brief Catches mouse release events
 * @param[in] event QMouseEvent object
 */
void Window::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mouseButtonRight = false;
    mouseButtonLeft = false;
    mouseButtonIsDown = false;
    renderLater();
}

/**
 * @brief Catches mouse wheel events
 * @param[in] event QMouseEvent object
 */
void Window::wheelEvent(QWheelEvent *event)
{
    QVector3D currentViewportCoords = computeViewportCameraCoords(getTranslation());
    currentViewportCoords.setZ(currentViewportCoords.z() + (float) event->delta() / 10.0f);
    // Move by z viewport coord
    setTranslation(computeObjectCameraCoords(currentViewportCoords));
    emit positionChanged();
    renderLater();
}

/**
 * @brief Computes object coords from viewport coords
 * @param[in] viewportCoords Viewport coords
 * @return Object coords
 */
QVector3D Window::computeObjectCoords(const QVector3D &viewportCoords)
{
    return ((getPerspetiveMatrix() * getCameraMatrix()).inverted() * QVector4D(viewportCoords, 1.0f)).toVector3DAffine();
}

/**
 * @brief Computes viewport coords from object coords
 * @param[in] objectCoords Object coords
 * @return Viewport coords
 */
QVector3D Window::computeViewportCoords(const QVector3D &objectCoords)
{
    return (getPerspetiveMatrix() * getCameraMatrix() * QVector4D(objectCoords, 1.0f)).toVector3DAffine();
}

/**
 * @brief Computes object camera coords from viewport coords (without perspective matrix)
 * @param[in] viewportCoords Viewport coords
 * @return Object camera coords
 */
QVector3D Window::computeObjectCameraCoords(const QVector3D &viewportCoords)
{
    return (getCameraMatrix().inverted() * QVector4D(viewportCoords, 1.0f)).toVector3DAffine();
}

/**
 * @brief Computes object camera coords from viewport coords (without perspective matrix)
 * @param[in] objectCoords Object coords
 * @return Viewport camera coords
 */
QVector3D Window::computeViewportCameraCoords(const QVector3D &objectCoords)
{
    return (getCameraMatrix() * QVector4D(objectCoords, 1.0f)).toVector3DAffine();
}

/**
 * @brief Sets render width
 * @param[in] renderWidth Render width
 */
void Window::setRenderWidth(GLuint renderWidth)
{
    MainRenderer::setRenderWidth(renderWidth);
    renderLater();
}

/**
 * @brief Sets render height
 * @param[in] renderHeight Render height
 */
void Window::setRenderHeight(GLuint renderHeight)
{
    MainRenderer::setRenderHeight(renderHeight);
    renderLater();
}
}
