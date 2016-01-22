/**
 * @file        pyramid.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The implementation file containing the Pyramid class.
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

#include "input/pyramid.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a Pyramid object using the 5 given points
 * @param[in] leftTop Left top corner of pyramid base
 * @param[in] leftBottom Left bottom corner of pyramid base
 * @param[in] rightTop Right top corner of pyramid base
 * @param[in] rightBottom Right bottom corner of pyramid base
 * @param[in] eye Position of eye - the top of the pyramid
 */
Pyramid::Pyramid(const QVector3D &leftTop, const QVector3D &leftBottom, const QVector3D &rightTop, const QVector3D &rightBottom, const QVector3D &eye)
{
    init(leftTop, leftBottom, rightTop, rightBottom, eye);
}

/**
 * @brief Creates a Pyramid object using an 2D float array and optional special flag
 * @param[in] array 2D array of sizes 3x5 or 3x6 (specialFlag) or bigger
 * @param[in] specialFlag Flag for special initialization of pyramid from 2D data (some coords are swapped)
 */
Pyramid::Pyramid(const QVector<QVector<float>> &array, bool specialFlag)
{
    init(array, specialFlag);
}

/**
 * @brief Creates a Pyramid object using an 1D float array
 * @param[in] array 1D array of size 15 (3x5, Row-major order)
 */
Pyramid::Pyramid(const QVector<float> &array)
{
    init(array);
}

/**
 * @brief Default constructor
 *
 * Init pyramid with some default values.
 */
Pyramid::Pyramid()
{
    init(
            QVector3D(-100,  100,  0),
            QVector3D(-100, -100,  0),
            QVector3D( 100,  100,  0),
            QVector3D( 100, -100,  0),
            QVector3D( 0,  0,  500)
        );
    color = QVector4D(0.2f, 0.2f, 0.8f, 1.0f);
}

/**
 * @brief Destructor of Pyramid object
 *
 * Does nothing.
 */
Pyramid::~Pyramid()
{

}

/**
 * @brief Copy constructor
 * @param[in] pyramid Original Pyramid object to copy
 */
Pyramid::Pyramid(const Pyramid &pyramid)
{
    init(pyramid.leftTop, pyramid.leftBottom, pyramid.rightTop, pyramid.rightBottom, pyramid.eye);
    assign(pyramid);
}

/**
 * @brief Assignment operator
 * @param[in] pyramid Reference to the existing Pyramid
 * @return Reference to Pyramid instance
 */
Pyramid &Pyramid::operator=(const Pyramid &pyramid)
{
    if (this != &pyramid) {
        init(pyramid.leftTop, pyramid.leftBottom, pyramid.rightTop, pyramid.rightBottom, pyramid.eye);
        assign(pyramid);
    }
    return *this;
}

/**
 * @brief Returns the fovy
 * @return The field of view in degrees in the y direction (from the eye to the center)
 */
float Pyramid::getFovy() const
{
    return fovy;
}

/**
 * @brief Returns distance from eye to the center
 * @return Distance from eye to the center
 */
float Pyramid::getDistance() const
{
    return distance;
}

/**
 * @brief Returns the eye position
 * @return The eye position
 */
QVector3D Pyramid::getEye() const
{
    return eye;
}

/**
 * @brief Returns the center position
 * @return The center position
 */
QVector3D Pyramid::getCenter() const
{
    return center;
}

/**
 * @brief Returns left top corner position
 * @return Left top corner position
 */
QVector3D Pyramid::getLeftTop() const
{
    return leftTop;
}

/**
 * @brief Returns left bottom corner position
 * @return Left bottom corner position
 */
QVector3D Pyramid::getLeftBottom() const
{
    return leftBottom;
}

/**
 * @brief Returns right top corner position
 * @return Right top corner position
 */
QVector3D Pyramid::getRightTop() const
{
    return rightTop;
}

/**
 * @brief Returns right bottom corner position
 * @return Right bottom corner position
 */
QVector3D Pyramid::getRightBottom() const
{
    return rightBottom;
}

/**
 * @brief Returns the up vector
 * @return The up vector
 */
QVector3D Pyramid::getUpDir() const
{
    return up;
}

/**
 * @brief Returns the middle point position
 * @return The middle point position
 */
QVector3D Pyramid::getMiddle() const
{
    return middle;
}

/**
 * @brief Returns the view vector
 * @return The view vector
 */
QVector3D Pyramid::getViewDir() const
{
    return view;
}

/**
 * @brief Returns the corners in matrix
 * @return The corners in matrix
 */
QMatrix4x4 Pyramid::getCorners() const
{
    return corners;
}

/**
 * @brief Returns pyramid color
 * @return Pyramid color
 */
QVector4D Pyramid::getColor() const
{
    return color;
}

/**
 * @brief Sets pyramid color
 * @param[in] vector4d Color to set
 */
void Pyramid::setColor(const QVector4D &vector4d)
{
    color = vector4d;
}

/**
 * @brief Returns the image for "base"
 * @return The image for "base"
 */
QImage Pyramid::getImage() const
{
    return image;
}

/**
 * @brief Sets the image for "base"
 * @param[in] image The image for "base"
 */
void Pyramid::setImage(const QImage &image)
{
    this->image = image.convertToFormat(QImage::Format_RGB888);
}

/**
 * @brief Returns transformed pyramid
 * @param[in] rx X rotation
 * @param[in] ry Y rotation
 * @param[in] rz Z rotation
 * @param[in] tx X translation
 * @param[in] ty Y translation
 * @param[in] tz Z translation
 * @return Transformed pyramid
 */
Pyramid Pyramid::transformed(double rx, double ry, double rz, double tx, double ty, double tz) const
{
    QMatrix4x4 r, t;
    r.setToIdentity();
    r.rotate(rz, 0, 0, 1);
    r.rotate(ry, 0, 1, 0);
    r.rotate(rx, 1, 0, 0);
    t.setToIdentity();
    t.translate(tx, ty, tz);

    Pyramid result(t * r * leftTop, t * r * leftBottom, t * r * rightTop, t * r * rightBottom, t * r * eye);
    result.setColor(color);
    result.setImage(image);

    return result;
}

/**
 * @brief Initialies Pyramid object using corners, eye and compute other values
 * @param[in] leftTop Left top corner of pyramid base
 * @param[in] leftBottom Left bottom corner of pyramid base
 * @param[in] rightTop Right top corner of pyramid base
 * @param[in] rightBottom Right bottom corner of pyramid base
 * @param[in] eye Position of eye - the top of the pyramid
 */
void Pyramid::init(const QVector3D &leftTop, const QVector3D &leftBottom, const QVector3D &rightTop, const QVector3D &rightBottom, const QVector3D &eye)
{
    this->leftTop = leftTop;
    this->leftBottom = leftBottom;
    this->rightTop = rightTop;
    this->rightBottom = rightBottom;
    this->eye = eye;

    // Compute others
    comp();
}

/**
 * @brief Initialies Pyramid object using an 2D float array and optional special flag
 * @param[in] array 2D array of sizes 3x5 or 3x6 (specialFlag) or bigger
 * @param[in] specialFlag Flag for special initialization of pyramid from 2D data (some coords are swapped)
 */
void Pyramid::init(const QVector<QVector<float>> &array, bool specialFlag)
{
    // Special situation
    // @todo TODO
    if (specialFlag) {
        Q_ASSERT(array.size() >= 6);
        Q_ASSERT(array.at(0).size() == 3);
        Q_ASSERT(array.at(1).size() == 3);
        Q_ASSERT(array.at(2).size() == 3);
        Q_ASSERT(array.at(3).size() == 3);
        Q_ASSERT(array.at(5).size() == 3);

        init(
                QVector3D(array[0][0], array[1][1], array[0][2]),
                QVector3D(array[1][0], array[0][1], array[1][2]),
                QVector3D(array[2][0], array[3][1], array[2][2]),
                QVector3D(array[3][0], array[2][1], array[3][2]),
                QVector3D(array[5][0], array[5][1], array[5][2])
            );
    } else {
        Q_ASSERT(array.size() >= 5);
        Q_ASSERT(array.at(0).size() == 3);
        Q_ASSERT(array.at(1).size() == 3);
        Q_ASSERT(array.at(2).size() == 3);
        Q_ASSERT(array.at(3).size() == 3);
        Q_ASSERT(array.at(4).size() == 3);

        init(
                QVector3D(array[0][0], array[0][1], array[0][2]),
                QVector3D(array[1][0], array[1][1], array[1][2]),
                QVector3D(array[2][0], array[2][1], array[2][2]),
                QVector3D(array[3][0], array[3][1], array[3][2]),
                QVector3D(array[4][0], array[4][1], array[4][2])
            );
    }
}

/**
 * @brief Initialies Pyramid object using an 1D float array
 * @param[in] array 1D array of size 15 (3x5, Row-major order)
 */
void Pyramid::init(const QVector<float> &array)
{
    Q_ASSERT(array.size() == 15);
    init(
            QVector3D(array[0], array[1], array[2]),
            QVector3D(array[3], array[4], array[5]),
            QVector3D(array[6], array[7], array[8]),
            QVector3D(array[9], array[10], array[11]),
            QVector3D(array[12], array[13], array[14])
        );
}

/**
 * @brief Helper function for copy and assign constructors
 * @param[in] pyramid Original Pyramid object to copy
 */
void Pyramid::assign(const Pyramid &pyramid)
{
   this->center = pyramid.center;
   this->up = pyramid.up;
   this->view = pyramid.view;
   this->middle = pyramid.middle;
   this->fovy = pyramid.fovy;
   this->distance = pyramid.distance;
   this->corners = pyramid.corners;
   this->color = pyramid.color;
   this->image = pyramid.image;
}

/**
 * @brief Computes other points and values
 */
void Pyramid::comp()
{
    QVector3D m0 = (leftTop + (rightTop - leftTop) / 2);
    QVector3D m1 = (leftBottom + (rightBottom - leftBottom) / 2);
    QVector3D v0 = m0 - eye;
    QVector3D v1 = m1 - eye;

    center = m0 + (m1 - m0) / 2;
    distance = eye.distanceToPoint(center);
    fovy = qRadiansToDegrees(qAcos(QVector3D::dotProduct(v0.normalized(), v1.normalized())));
    corners = QMatrix4x4(
            leftTop.x(), leftBottom.x(), rightTop.x(), rightBottom.x(),
            leftTop.y(), leftBottom.y(), rightTop.y(), rightBottom.y(),
            leftTop.z(), leftBottom.z(), rightTop.z(), rightBottom.z(),
            1.0f, 1.0f, 1.0f, 1.0f
        );

    middle = center + (eye - center) / 2;
    up = (leftTop - leftBottom).normalized();
    view = (eye - center).normalized();

    // Check some wrong values
    Q_ASSERT(!view.isNull());
    Q_ASSERT(!up.isNull());
    Q_ASSERT(eye != center);
    Q_ASSERT(!qFuzzyCompare(eye, center));
}
}







