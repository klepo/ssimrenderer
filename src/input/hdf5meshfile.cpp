/**
 * @file        hdf5meshfile.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the HDF5MeshFile class.
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

#include "input/hdf5meshfile.h"

namespace SSIMRenderer
{
/** @brief The name of VERTICES matrix in Statismo HDF5 file */
const QString HDF5MeshFile::VERTICES("representer/points");

/** @brief The name of TRIANGLES matrix in Statismo HDF5 file */
const QString HDF5MeshFile::TRIANGLES("representer/cells");

/**
 * @brief Creates a HDF5MeshFile object using the filename
 * @param[in] filename HDF5 filename (filepath)
 */
HDF5MeshFile::HDF5MeshFile(const QString &filename) : Mesh(), HDF5Wrapper(filename)
{
    // Init rows and cols
    long rows = 0, cols = 0;
    bool first = true;

    // Load vertices matrix
    float *tableOfVerticesTmp;
    load2DMatrix(VERTICES, tableOfVerticesTmp, rows, cols);
    numberOfVertices = cols;

    // Create array for vertices
    tableOfVertices = new float[rows * cols];

    // Transpose because of Statismo data layout
    for (int i = 0; i < numberOfVertices; i++) {
        tableOfVertices[3 * i + 0] = tableOfVerticesTmp[i];
        tableOfVertices[3 * i + 1] = tableOfVerticesTmp[i + numberOfVertices];
        tableOfVertices[3 * i + 2] = tableOfVerticesTmp[i + numberOfVertices * 2];

        // Get min and max vertices
        if (first) {
            maxVertex[0] = tableOfVerticesTmp[i];
            maxVertex[1] = tableOfVerticesTmp[i + numberOfVertices];
            maxVertex[2] = tableOfVerticesTmp[i + numberOfVertices * 2];
            minVertex[0] = tableOfVerticesTmp[i];
            minVertex[1] = tableOfVerticesTmp[i + numberOfVertices];
            minVertex[2] = tableOfVerticesTmp[i + numberOfVertices * 2];
            first = false;
        }
        if (maxVertex[0] < tableOfVerticesTmp[i])
            maxVertex[0] = tableOfVerticesTmp[i];
        if (maxVertex[1] < tableOfVerticesTmp[i + numberOfVertices])
            maxVertex[1] = tableOfVerticesTmp[i + numberOfVertices];
        if (maxVertex[2] < tableOfVerticesTmp[i + numberOfVertices * 2])
            maxVertex[2] = tableOfVerticesTmp[i + numberOfVertices * 2];
        if (minVertex[0] > tableOfVerticesTmp[i])
            minVertex[0] = tableOfVerticesTmp[i];
        if (minVertex[1] > tableOfVerticesTmp[i + numberOfVertices])
            minVertex[1] = tableOfVerticesTmp[i + numberOfVertices];
        if (minVertex[2] > tableOfVerticesTmp[i + numberOfVertices * 2])
            minVertex[2] = tableOfVerticesTmp[i + numberOfVertices * 2];
    }
    delete[] tableOfVerticesTmp;
    qDebug() << "\tnumberOfVertices" << numberOfVertices;
    qDebug() << "\tVertices loaded";

    // Load Triangles
    unsigned int *tableOfTrianglesTmp;
    load2DMatrix(TRIANGLES, tableOfTrianglesTmp, rows, cols);
    numberOfTriangles = cols;
    tableOfTriangles = new unsigned int[rows * cols];
    // Transpose because of Statismo data layout
    for (int i = 0; i < numberOfTriangles; i++) {
        tableOfTriangles[3 * i + 0] = tableOfTrianglesTmp[i];
        tableOfTriangles[3 * i + 1] = tableOfTrianglesTmp[i + numberOfTriangles];
        tableOfTriangles[3 * i + 2] = tableOfTrianglesTmp[i + numberOfTriangles * 2];
    }
    delete[] tableOfTrianglesTmp;
    qDebug() << "\tnumberOfTriangles" << numberOfTriangles;
    qDebug() << "\tTriangles loaded";

    // Generate triangles adjacency
    tableOfTrianglesAdjacency = new unsigned int[numberOfTriangles * 6];
    numberOfTrianglesAdjacency = numberOfTriangles;
    generateTableOfTrianglesAdjacency();
    qDebug() << "\tTriangles Adjacency generated";

    // Generate normals
    generateTableOfNormals();
    qDebug() << "\tVertices normals generated";

}

/**
 * @brief Destructor of HDF5MeshFile object
 *
 * Does nothing.
 */
HDF5MeshFile::~HDF5MeshFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] hDF5MeshFile Original HDF5MeshFile object to copy
 *
 * Calls base classes copy constructors.
 */
HDF5MeshFile::HDF5MeshFile(const HDF5MeshFile &hDF5MeshFile) : Mesh(hDF5MeshFile), HDF5Wrapper(hDF5MeshFile)
{

}

/**
 * @brief Assignment operator
 * @param[in] hDF5MeshFile Reference to the existing HDF5MeshFile
 * @return Reference to HDF5MeshFile instance
 *
 * Calls base classes copy constructors.
 */
HDF5MeshFile &HDF5MeshFile::operator=(const HDF5MeshFile &hDF5MeshFile)
{
    Mesh::operator=(hDF5MeshFile);
    HDF5Wrapper::operator=(hDF5MeshFile);
    return *this;
}
}
