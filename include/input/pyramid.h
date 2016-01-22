/**
 * @file        pyramid.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The header file with Pyramid class declaration.
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

#ifndef SSIMR_PYRAMID_H
#define SSIMR_PYRAMID_H

#include "../ssimrenderer_global.h"

#include <QVector3D>
#include <QMatrix4x4>
#include <QImage>
#include <QtMath>

namespace SSIMRenderer
{
/**
 * @brief The Pyramid class represents the structure for pyramid
 *
 * Pyramid is defined by 5 points - 4 corners (base) and eye (the top of the pyramid).
 * The corners have to be in same plane, but cannot be in same plane as eye.
 * The eye shold be centered between corners.
 * The non-neighboring corners should be orthogonal to each other.
 */
class SHARED_EXPORT Pyramid
{
public:
    // Creates a Pyramid object using the 5 given points
    Pyramid(const QVector3D &leftTop, const QVector3D &leftBottom, const QVector3D &rightTop, const QVector3D &rightBottom, const QVector3D &eye);

    // Creates a Pyramid object using an array and optional special flag
    Pyramid(const QVector<QVector<float>> &array, bool specialFlag = false);

    // Creates a Pyramid object using an array
    Pyramid(const QVector<float> &array);

    // Default constructor
    Pyramid();

    // Destructor of Pyramid object
    virtual ~Pyramid();

    // Copy constructor
    Pyramid(const Pyramid &pyramid);

    // Assignment operator
    Pyramid &operator=(const Pyramid &pyramid);

    // Returns fovy
    float getFovy() const;

    // Returns distance between eye and center
    float getDistance() const;

    // Points getters
    QVector3D getEye() const;
    QVector3D getCenter() const;
    QVector3D getLeftTop() const;
    QVector3D getLeftBottom() const;
    QVector3D getRightTop() const;
    QVector3D getRightBottom() const;
    QVector3D getUpDir() const;
    QVector3D getMiddle() const;
    QVector3D getViewDir() const;

    QMatrix4x4 getCorners() const;

    // Color getter and setter
    QVector4D getColor() const;
    void setColor(const QVector4D &vector4d);

    // Image getter and setter
    QImage getImage() const;
    void setImage(const QImage &image);

    // Returns transformed pyramid
    Pyramid transformed(double rx, double ry, double rz, double tx, double ty, double tz) const;

protected:
    // Init functions
    virtual void init(const QVector3D &leftTop, const QVector3D &leftBottom, const QVector3D &rightTop, const QVector3D &rightBottom, const QVector3D &eye) final;
    virtual void init(const QVector<QVector<float>> &array, bool specialFlag = false) final;
    virtual void init(const QVector<float> &array) final;

    // Computes other points and values
    virtual void comp() final;

private:
    void assign(const Pyramid &pyramid);

    float fovy;
    float distance;

    QVector3D eye;
    QVector3D center;

    QVector3D leftTop;
    QVector3D leftBottom;
    QVector3D rightTop;
    QVector3D rightBottom;

    QVector3D up;
    QVector3D middle;
    QVector3D view;

    QMatrix4x4 corners;

    QVector4D color;

    QImage image;
};
}

#endif // SSIMR_PYRAMID_H
