/**
 * @file        window.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The header file with Window class declaration.
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

#ifndef SSIMR_WINDOW_H
#define SSIMR_WINDOW_H

#include "../ssimrenderer_global.h"

#include "mainrenderer.h"

#include <QMouseEvent>
#include <QCoreApplication>

namespace SSIMRenderer
{
/**
 * @brief The Window class represents the structure for window rendering
 */
class SHARED_EXPORT Window : public QWindow, public MainRenderer
{
    Q_OBJECT

public:
    // Creates Window using optional parental OpenGLWrapper
    Window(OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of Window object
    virtual ~Window();

public slots:
    // Render with queued events mechanism (QEvent::UpdateRequest)
    virtual void renderLater();

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

    // Rendering settings slots
    void enableDensity(bool value);
    void enableSilhouettes(bool value);
    void enablePyramid(bool value);
    void enablePolygonal(bool value);
    void enableXMirroring(bool value);
    void enablePolygonalLighting(bool value);

    // Transformations slots
    void updateTranslation(float x, float y, float z);
    void updateTranslation(const QVector3D &position);

    void updateRotation(float xAngle, float yAngle, float zAngle);
    void updateRotation(const QVector3D &rotation);
    void updateRotationMatrix(const QMatrix4x4 &rotationMatrix);

signals:
    /// Rotation changed signal
    void rotationChanged();

    /// Position changed signal
    void positionChanged();

    /// Rendered signal
    void rendered();

protected:
    // Window events
    bool event(QEvent *event);
    virtual void exposeEvent(QExposeEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    // Helper functions for object/viewport coords
    virtual QVector3D computeObjectCoords(const QVector3D &viewportCoords) final;
    virtual QVector3D computeViewportCoords(const QVector3D &objectCoords) final;
    virtual QVector3D computeObjectCameraCoords(const QVector3D &viewportCoords) final;
    virtual QVector3D computeViewportCameraCoords(const QVector3D &objectCoords) final;

    // Mouse buttons flags
    /// Mouse button is down
    bool mouseButtonIsDown;

    /// Left mouse button is down
    bool mouseButtonLeft;

    /// Right mouse button is down
    bool mouseButtonRight;

    /// Current mouse position
    QPoint mousePositionCurrent;

    /// Last clicked mouse position
    QPoint mousePositionClickedLast;

    /// Current clicked mouse position
    QPoint mousePositionClickedCurrent;

    /// Current clicked mouse position moving
    QPoint mousePositionClickedCurrentMoved;

    /// Position of clicked object
    QVector3D clickedObjectPosition;

private:
    bool updatePending;

    virtual void setRenderWidth(GLuint renderWidth) final;
    virtual void setRenderHeight(GLuint renderHeight) final;

    Q_DISABLE_COPY(Window)
};
}

#endif // SSIMR_WINDOW_H
