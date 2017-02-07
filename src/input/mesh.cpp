/**
 * @file        mesh.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the Mesh class.
 *
 * @todo TODO   Reimplement data allocation - setters/getters and private members
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

#include "input/mesh.h"

namespace SSIMRenderer
{
/**
 * @brief Default constructor
 *
 * Initializes min/max vertex.
 */
Mesh::Mesh()
    : tableOfVertices(0)
    , tableOfNormals(0)
    , tableOfTriangles(0)
    , tableOfTrianglesAdjacency(0)
    , tableOfTetrahedra(0)
    , numberOfVertices(0)
    , numberOfTriangles(0)
    , numberOfTrianglesAdjacency(0)
    , numberOfTetrahedra(0)
    , maxVertex(0)
    , minVertex(0)
{
    // Init min/max vertex arrays
    minVertex = new float[VERTEX_SIZE]();
    maxVertex = new float[VERTEX_SIZE]();
}

/**
 * @brief Destructor of Mesh object
 *
 * Deletes allocated memory for geometry
 */
Mesh::~Mesh()
{
    delete[] tableOfVertices;
    delete[] tableOfNormals;
    delete[] tableOfTriangles;
    delete[] tableOfTrianglesAdjacency;
    delete[] tableOfTetrahedra;
    delete[] maxVertex;
    delete[] minVertex;
}

/**
 * @brief Copy constructor
 * @param[in] mesh Original Mesh object to copy
 */
Mesh::Mesh(const Mesh &mesh)
{
    assign(mesh, false);
}

/**
 * @brief Assignment operator
 * @param[in] mesh Reference to the existing Mesh
 * @return Reference to Mesh instance
 */
Mesh &Mesh::operator=(const Mesh &mesh)
{
    if (this != &mesh) {
        assign(mesh, true);
    }
    return *this;
}

/**
 * @brief Returns minimal vertex
 * @return Minimal vertex
 */
QVector3D Mesh::getMinVertex() const
{
    return QVector3D(minVertex[0], minVertex[1], minVertex[2]);
}

/**
 * @brief Returns maximal vertex
 * @return Maximal vertex
 */
QVector3D Mesh::getMaxVertex() const
{
    return QVector3D(maxVertex[0], maxVertex[1], maxVertex[2]);
}

/**
 * @brief Returns the pointer to vertices data
 * @return The pointer to linearly saved vertices float data
 */
float *Mesh::getTableOfVertices() const
{
    return tableOfVertices;
}

/**
 * @brief Returns the pointer to normals data
 * @return The pointer to linearly saved normals float data
 */
float *Mesh::getTableOfNormals() const
{
    return tableOfNormals;
}

/**
 * @brief Returns the pointer to triangles data
 * @return The pointer to linearly saved triangles unsigned int data
 */
unsigned int *Mesh::getTableOfTriangles() const
{
    return  tableOfTriangles;
}

/**
 * @brief Returns triangles 2D vector
 * @return Triangles 2D vector
 */
QVector<QVector<unsigned int>> Mesh::getTriangles2DVector() const
{
    QVector<QVector<unsigned int>> result(numberOfTriangles);

    for(int i = 0; i < result.size(); i++) {
        result[i].resize(3);
        result[i][0] = tableOfTriangles[i * 3 + 0];
        result[i][1] = tableOfTriangles[i * 3 + 1];
        result[i][2] = tableOfTriangles[i * 3 + 2];
    }

    return result;
}

/**
 * @brief Returns the pointer to triangles adjacency data
 * @return The pointer to linearly saved triangles adjacency unsigned int data
 */
unsigned int *Mesh::getTableOfTrianglesAdjacency() const
{
    return  tableOfTrianglesAdjacency;
}

/**
 * @brief Returns the pointer to tetrahedra data
 * @return The pointer to linearly saved tetrahedra unsigned int data
 */
unsigned int *Mesh::getTableOfTetrahedra() const
{
    return tableOfTetrahedra;
}

/**
 * @brief Returns the number of vertices
 * @return Number of vertices
 */
long Mesh::getNumberOfVertices() const
{
    return numberOfVertices;
}

/**
 * @brief Returns the number of triangles
 * @return Number of triangles
 */
long Mesh::getNumberOfTriangles() const
{
    return numberOfTriangles;
}

/**
 * @brief Returns the number of triangles adjacency
 * @return Number of triangles adjacency
 */
long Mesh::getNumberOfTrianglesAdjacency() const
{
    return numberOfTrianglesAdjacency;
}

/**
 * @brief Returns the number of tetrahedra
 * @return Number of tetrahedra
 */
long Mesh::getNumberOfTetrahedra() const
{
    return numberOfTetrahedra;
}

/**
 * @brief Generates triangles adjacency data
 */
void Mesh::generateTableOfTrianglesAdjacency()
{
    QHash<QString, int> hashTable;
    for (long i = 0; i < numberOfTriangles; i++) {
        hashTable[QString::number(tableOfTriangles[i * 3 + 0]) + QString::number(tableOfTriangles[i * 3 + 1])] = tableOfTriangles[i * 3 + 2];
        hashTable[QString::number(tableOfTriangles[i * 3 + 1]) + QString::number(tableOfTriangles[i * 3 + 2])] = tableOfTriangles[i * 3 + 0];
        hashTable[QString::number(tableOfTriangles[i * 3 + 2]) + QString::number(tableOfTriangles[i * 3 + 0])] = tableOfTriangles[i * 3 + 1];
    }

    for (long i = 0; i < numberOfTriangles; i++) {
        tableOfTrianglesAdjacency[i * 6 + 0] = tableOfTriangles[i * 3 + 0];
        tableOfTrianglesAdjacency[i * 6 + 1] = tableOfTriangles[i * 3 + 1];
        tableOfTrianglesAdjacency[i * 6 + 2] = tableOfTriangles[i * 3 + 2];

        QHash<QString, int>::iterator it;
        it = hashTable.find(QString::number(tableOfTriangles[i * 3 + 1]) + QString::number(tableOfTriangles[i * 3 + 0]));
        if (it != hashTable.end())
            tableOfTrianglesAdjacency[i * 6 + 3] = it.value();
        else
            tableOfTrianglesAdjacency[i * 6 + 3] = tableOfTriangles[i * 3 + 2];
            //throw std::runtime_error("Wrong mesh file (holes in the mesh?)");
        it = hashTable.find(QString::number(tableOfTriangles[i * 3 + 2]) + QString::number(tableOfTriangles[i * 3 + 1]));
        if (it != hashTable.end())
            tableOfTrianglesAdjacency[i * 6 + 4] = it.value();
        else
            tableOfTrianglesAdjacency[i * 6 + 4] = tableOfTriangles[i * 3 + 0];
            //throw std::runtime_error("Wrong mesh file (holes in the mesh?)");
        it = hashTable.find(QString::number(tableOfTriangles[i * 3 + 0]) + QString::number(tableOfTriangles[i * 3 + 2]));
        if (it != hashTable.end())
            tableOfTrianglesAdjacency[i * 6 + 5] = it.value();
        else
            tableOfTrianglesAdjacency[i * 6 + 5] = tableOfTriangles[i * 3 + 1];
            //throw std::runtime_error("Wrong mesh file (holes in the mesh?)");
    }
}

/**
 * @brief Generates normals data
 */
void Mesh::generateTableOfNormals()
{
    //return;

    delete[] tableOfNormals;
    tableOfNormals = new float[numberOfVertices * 3]();

    /*for (long i = 0; i < numberOfVertices; i++) {
        QVector3D normal(0, 0, 0);
        tableOfNormals[i * 3 + 0] = normal.x();
        tableOfNormals[i * 3 + 1] = normal.y();
        tableOfNormals[i * 3 + 2] = normal.z();
    }*/

    for (long i = 0; i < numberOfTriangles; i++) {
        QVector3D a = getVectorOfTrianglePoint(i, 0);
        QVector3D b = getVectorOfTrianglePoint(i, 1);
        QVector3D c = getVectorOfTrianglePoint(i, 2);
        QVector3D normal = QVector3D::crossProduct(c - a, b - a).normalized();

        long i0 = tableOfTriangles[i * 3 + 0];
        long i1 = tableOfTriangles[i * 3 + 1];
        long i2 = tableOfTriangles[i * 3 + 2];

        tableOfNormals[i0 * 3 + 0] += normal.x();
        tableOfNormals[i0 * 3 + 1] += normal.y();
        tableOfNormals[i0 * 3 + 2] += normal.z();

        tableOfNormals[i1 * 3 + 0] += normal.x();
        tableOfNormals[i1 * 3 + 1] += normal.y();
        tableOfNormals[i1 * 3 + 2] += normal.z();

        tableOfNormals[i2 * 3 + 0] += normal.x();
        tableOfNormals[i2 * 3 + 1] += normal.y();
        tableOfNormals[i2 * 3 + 2] += normal.z();
    }

    for (long i = 0; i < numberOfVertices; i++) {
        QVector3D normal(tableOfNormals[i * 3 + 0], tableOfNormals[i * 3 + 1], tableOfNormals[i * 3 + 2]);
        normal.normalize();
        tableOfNormals[i * 3 + 0] = normal.x();
        tableOfNormals[i * 3 + 1] = normal.y();
        tableOfNormals[i * 3 + 2] = normal.z();
    }

}

/**
 * @brief Finds the vertex of triangle by triangle index and coordinate index
 * @param[in] triangleIndex Index of triangle
 * @param[in] index X, Y or Z index (coordinate)
 * @return Selected vertex of selected triangle
 */
QVector3D Mesh::getVectorOfTrianglePoint(const long triangleIndex, const int index) const
{
    long i = tableOfTriangles[triangleIndex * 3 + index];
    return QVector3D(tableOfVertices[i * 3 + 0], tableOfVertices[i * 3 + 1], tableOfVertices[i * 3 + 2]);
}

/**
 * @brief Helper function for copy and assign constructors
 * @param[in] statisticalData Original StatisticalData object to copy
 * @param[in] deleteFlag Flag for releasing memory
 */
void Mesh::assign(const Mesh &mesh, const bool deleteFlag)
{
    if (deleteFlag) {
        delete[] tableOfVertices;
        delete[] tableOfNormals;
        delete[] tableOfTriangles;
        delete[] tableOfTrianglesAdjacency;
        delete[] tableOfTetrahedra;
        delete[] maxVertex;
        delete[] minVertex;
    }

    tableOfVertices = new float[mesh.numberOfVertices * 3]();
    std::memcpy(tableOfVertices, mesh.tableOfVertices, mesh.numberOfVertices * 3 * sizeof(float));

    tableOfNormals = new float[mesh.numberOfVertices * 3]();
    std::memcpy(tableOfNormals, mesh.tableOfNormals, mesh.numberOfVertices * 3 * sizeof(float));

    tableOfTriangles = new unsigned int[mesh.numberOfTriangles * 3]();
    std::memcpy(tableOfTriangles, mesh.tableOfTriangles, mesh.numberOfTriangles * 3 * sizeof(unsigned int));

    tableOfTrianglesAdjacency = new unsigned int[mesh.numberOfTrianglesAdjacency * 6];
    std::memcpy(tableOfTrianglesAdjacency, mesh.tableOfTrianglesAdjacency, mesh.numberOfTrianglesAdjacency * 6 * sizeof(unsigned int));

    tableOfTetrahedra = new unsigned int[mesh.numberOfTetrahedra * 4]();
    std::memcpy(tableOfTetrahedra, mesh.tableOfTetrahedra, mesh.numberOfTetrahedra * 4 * sizeof(unsigned int));

    numberOfVertices = mesh.numberOfVertices;
    numberOfTriangles = mesh.numberOfTriangles;
    numberOfTrianglesAdjacency = mesh.numberOfTrianglesAdjacency;
    numberOfTetrahedra = mesh.numberOfTetrahedra;

    if (maxVertex) {
        maxVertex = new float[VERTEX_SIZE]();
        std::memcpy(maxVertex, mesh.maxVertex, VERTEX_SIZE * sizeof(float));
    }

    if (minVertex) {
        minVertex = new float[VERTEX_SIZE]();
        std::memcpy(minVertex, mesh.minVertex, VERTEX_SIZE * sizeof(float));
    }
}
}
