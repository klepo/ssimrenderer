/**
 * @file        mesh.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with Mesh class declaration.
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

#ifndef SSIMR_MESH_H
#define SSIMR_MESH_H

#include "../ssimrenderer_global.h"

#include <QVector3D>
#include <QHash>
#include <QDebug>

namespace SSIMRenderer
{
/**
 * @brief The Mesh class represents the structure for %mesh data
 */
class SHARED_EXPORT Mesh
{
public:
    // Default constructor
    Mesh();

    // Destructor of Mesh object
    virtual ~Mesh();

    // Copy constructor
    Mesh(const Mesh &mesh);

    // Assignment operator
    Mesh &operator=(const Mesh &mesh);

    // Returns minimal vertex
    QVector3D getMinVertex() const;

    // Returns maximal vertex
    QVector3D getMaxVertex() const;

    // Returns vertices data
    float *getTableOfVertices() const;

    // Returns normals data
    float *getTableOfNormals() const;

    // Returns triangles data (inidices)
    unsigned int *getTableOfTriangles() const;
    QVector<QVector<unsigned int>> getTriangles2DVector() const;

    // Returns triangles ajdacency data
    unsigned int *getTableOfTrianglesAdjacency() const;

    // Returns tetrahedra data
    unsigned int *getTableOfTetrahedra() const;

    // Returns number of vertices
    long getNumberOfVertices() const;

    // Returns number of triangles
    long getNumberOfTriangles() const;

    // Returns number of triangles ajdacency
    long getNumberOfTrianglesAdjacency() const;

    // Returns number of tetrahedra
    long getNumberOfTetrahedra() const;

protected:
    // Generates triangles adjacency data
    void generateTableOfTrianglesAdjacency();

    // Generate normals data
    void generateTableOfNormals();

    /// Maximal vertex
    float *maxVertex;

    /// Minimal vertex
    float *minVertex;

    /// Number of vertices
    long numberOfVertices;

    /// Vertices data
    float *tableOfVertices;

    /// Normals data
    float *tableOfNormals;

    /// Number of triangles
    long numberOfTriangles;

    /// Triangles data
    unsigned int *tableOfTriangles;

    /// Number of triangles adjacency
    long numberOfTrianglesAdjacency;

    /// Triangles adjacency data
    unsigned int *tableOfTrianglesAdjacency;

    /// Number of Tetrahedra
    long numberOfTetrahedra;

    /// Tetrahedra data
    unsigned int *tableOfTetrahedra;

    /// Size of one vertex
    const unsigned int VERTEX_SIZE = 3;

private:
    QVector3D getVectorOfTrianglePoint(long triangleIndex, int index) const;
    void assign(const Mesh &mesh, bool deleteFlag);
};
}

#endif // SSIMR_MESH_H
