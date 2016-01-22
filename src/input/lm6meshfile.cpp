/**
 * @file        lm6meshfile.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the Lm6MeshFile class.
 *
 * This class is for loading mesh files with tetrahedral data.
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

#include "input/lm6meshfile.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a Lm6MeshFile object using the filename
 * @param[in] filename MESH filename (filepath)
 */
Lm6MeshFile::Lm6MeshFile(const QString &filename) : Mesh()
{
    version = 0;
    dimension = 0;
    meshIndex = 0;

    bool writeTrianglesAdjacencyFlag = false;

    // Load MESH file
    meshIndex = GmfOpenMesh((char *) filename.toStdString().c_str(), GmfRead, &version, &dimension);
    qDebug() << "MESH file " << filename << " opened";

    //qDebug() << "\tmeshIndex =" << meshIndex << ", version =" << version << ", dimension =" << dimension;

    if (!meshIndex)
        throw std::runtime_error(std::string("Error opening MESH file ") + filename.toStdString());

    if (dimension != 3)
        throw std::runtime_error("Wrong dimension in MESH file");

    // Allocate space for vertices
    numberOfVertices = GmfStatKwd(meshIndex, GmfVertices);
    if (numberOfVertices > 0) {
        tableOfVertices = new float[numberOfVertices * 3];
    } else {
        throw std::runtime_error("Wrong MESH file (numberOfVertices)");
    }

    // Allocate space for triangles
    numberOfTriangles = GmfStatKwd(meshIndex, GmfTriangles);
    if (numberOfTriangles > 0) {
        tableOfTriangles = new unsigned int[numberOfTriangles * 3];
    } else {
        throw std::runtime_error("Wrong MESH file (numberOfTriangles)");
    }

    // Allocate space for triangles adjacency
    numberOfTrianglesAdjacency = GmfStatKwd(meshIndex, GmfTrianglesP2);
    //qDebug() << "\tnumberOfTrianglesAdjacency =" << numberOfTrianglesAdjacency;
    if (numberOfTrianglesAdjacency ==  numberOfTriangles)
        tableOfTrianglesAdjacency = new unsigned int[numberOfTrianglesAdjacency * 6];

    // Allocate space for tetrahedra
    numberOfTetrahedra = GmfStatKwd(meshIndex, GmfTetrahedra);
    if (numberOfTetrahedra > 0) {
        tableOfTetrahedra = new unsigned int[numberOfTetrahedra * 4];
    } else {
        throw std::runtime_error("Wrong MESH file (numberOfTetrahedra)");
    }

    bool first = true;

    // Load vertices
    GmfGotoKwd(meshIndex, GmfVertices);
    for (long i = 0; i < numberOfVertices; i++) {
        float xF, yF, zF;
        double xD, yD, zD;
        int r;
        if (version == 1) {
            GmfGetLin(meshIndex, GmfVertices, &xF, &yF, &zF, &r);
        } else {
            GmfGetLin(meshIndex, GmfVertices, &xD, &yD, &zD, &r);
            xF = (float) xD;
            yF = (float) yD;
            zF = (float) zD;
        }
        tableOfVertices[i * 3 + 0] = xF;
        tableOfVertices[i * 3 + 1] = yF;
        tableOfVertices[i * 3 + 2] = zF;
        if (first) {
            maxVertex[0] = xF;
            maxVertex[1] = yF;
            maxVertex[2] = zF;
            minVertex[0] = xF;
            minVertex[1] = yF;
            minVertex[2] = zF;
            first = false;
        }
        if (maxVertex[0] < xF)
            maxVertex[0] = xF;
        if (maxVertex[1] < yF)
            maxVertex[1] = yF;
        if (maxVertex[2] < zF)
            maxVertex[2] = zF;
        if (minVertex[0] > xF)
            minVertex[0] = xF;
        if (minVertex[1] > yF)
            minVertex[1] = yF;
        if (minVertex[2] > zF)
            minVertex[2] = zF;
    }
    qDebug() << "\tnumberOfVertices" << numberOfVertices;
    qDebug() << "\tVertices loaded";
    //qDebug() << "\t\tmax vertex: " << maxVertex[0] << " " << maxVertex[1] << " " << maxVertex[2];
    //qDebug() << "\t\tmin vertex: " << minVertex[0] << " " << minVertex[1] << " " << minVertex[2];

    // Load triangles
    if (numberOfTriangles > 0) {
        GmfGotoKwd(meshIndex, GmfTriangles);
        for (long i = 0; i < numberOfTriangles; i++) {
            int x, y, z, r;
            GmfGetLin(meshIndex, GmfTriangles, &x, &y, &z, &r);
            tableOfTriangles[i * 3 + 0] = x - 1;
            tableOfTriangles[i * 3 + 1] = y - 1;
            tableOfTriangles[i * 3 + 2] = z - 1;
        }
        qDebug() << "\tnumberOfTriangles" << numberOfTriangles;
        qDebug() << "\tTriangles loaded";
    }

    // Load triangles adjacency
    if (numberOfTrianglesAdjacency > 0) {
           GmfGotoKwd(meshIndex, GmfTrianglesP2);
           for (long i = 0; i < numberOfTrianglesAdjacency; i++) {
               int x, y, z, u, v, w, r;
               GmfGetLin(meshIndex, GmfTrianglesP2, &x, &y, &z, &u, &v, &w, &r);
               tableOfTrianglesAdjacency[i * 6 + 0] = x - 1;
               tableOfTrianglesAdjacency[i * 6 + 1] = y - 1;
               tableOfTrianglesAdjacency[i * 6 + 2] = z - 1;
               tableOfTrianglesAdjacency[i * 6 + 3] = u - 1;
               tableOfTrianglesAdjacency[i * 6 + 4] = v - 1;
               tableOfTrianglesAdjacency[i * 6 + 5] = w - 1;
           }
           qDebug() << "\tTriangles adjacency loaded";
    } else {
        tableOfTrianglesAdjacency = new unsigned int[numberOfTriangles * 6];
        numberOfTrianglesAdjacency = numberOfTriangles;
        generateTableOfTrianglesAdjacency();
        qDebug() << "\tTriangles Adjacency generated";
        writeTrianglesAdjacencyFlag = true;
    }

    // Load tetrahedra
    if (numberOfTetrahedra > 0) {
        GmfGotoKwd(meshIndex, GmfTetrahedra);
        for (long i = 0; i < numberOfTetrahedra; i++) {
            int x, y, z, w, r;
            GmfGetLin(meshIndex, GmfTetrahedra, &x, &y, &z, &w, &r);
            tableOfTetrahedra[i * 4 + 0] = x - 1;
            tableOfTetrahedra[i * 4 + 1] = y - 1;
            tableOfTetrahedra[i * 4 + 2] = z - 1;
            tableOfTetrahedra[i * 4 + 3] = w - 1;
        }
        qDebug() << "\tnumberOfTetrahedra" << numberOfTetrahedra;
        qDebug() << "\tTetrahedra loaded";
    }

    GmfCloseMesh(meshIndex);

    if (writeTrianglesAdjacencyFlag) {
        // Save to file
        meshIndex = GmfOpenMesh((char *) filename.toStdString().c_str(), GmfWrite, version, dimension);
        if (!meshIndex)
            throw std::runtime_error(std::string("Error opening MESH file for write"));

        GmfSetKwd(meshIndex, GmfVertices, numberOfVertices);
        for (long i = 0; i < numberOfVertices; i++) {
            if (version > 1) {
                GmfSetLin(
                        meshIndex,
                        GmfVertices,
                        (double) (tableOfVertices[i * 3 + 0]),
                        (double) (tableOfVertices[i * 3 + 1]),
                        (double) (tableOfVertices[i * 3 + 2]),
                        meshIndex - 1
                    );
            } else {
                GmfSetLin(
                        meshIndex,
                        GmfVertices,
                        (float) (tableOfVertices[i * 3 + 0]),
                        (float) (tableOfVertices[i * 3 + 1]),
                        (float) (tableOfVertices[i * 3 + 2]),
                        meshIndex - 1
                    );
            }
        }

        GmfSetKwd(meshIndex, GmfTriangles, numberOfTriangles);
        for (long i = 0; i < numberOfTriangles; i++)
            GmfSetLin(
                    meshIndex,
                    GmfTriangles,
                    tableOfTriangles[i * 3 + 0] + 1,
                    tableOfTriangles[i * 3 + 1] + 1,
                    tableOfTriangles[i * 3 + 2] + 1,
                    meshIndex - 1
                );

        GmfSetKwd(meshIndex, GmfTrianglesP2, numberOfTrianglesAdjacency);
        for (long i = 0; i < numberOfTrianglesAdjacency; i++)
            GmfSetLin(
                    meshIndex,
                    GmfTrianglesP2,
                    tableOfTrianglesAdjacency[i * 6 + 0] + 1,
                    tableOfTrianglesAdjacency[i * 6 + 1] + 1,
                    tableOfTrianglesAdjacency[i * 6 + 2] + 1,
                    tableOfTrianglesAdjacency[i * 6 + 3] + 1,
                    tableOfTrianglesAdjacency[i * 6 + 4] + 1,
                    tableOfTrianglesAdjacency[i * 6 + 5] + 1,
                    meshIndex - 1
                );

        GmfSetKwd(meshIndex, GmfTetrahedra, numberOfTetrahedra);
        for (long i = 0; i < numberOfTetrahedra; i++)
            GmfSetLin(
                    meshIndex,
                    GmfTetrahedra,
                    tableOfTetrahedra[i * 4 + 0] + 1,
                    tableOfTetrahedra[i * 4 + 1] + 1,
                    tableOfTetrahedra[i * 4 + 2] + 1,
                    tableOfTetrahedra[i * 4 + 3] + 1,
                    meshIndex - 1
                );
        GmfCloseMesh(meshIndex);
    }

    generateTableOfNormals();
    qDebug() << "\tVertices normals generated";

}

/**
 * @brief Destructor of Lm6MeshFile object
 *
 * Does nothing.
 */
Lm6MeshFile::~Lm6MeshFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] lm6MeshFile Original Lm6MeshFile object to copy
 */
Lm6MeshFile::Lm6MeshFile(const Lm6MeshFile &lm6MeshFile) : Mesh(lm6MeshFile)
{
    meshIndex = lm6MeshFile.meshIndex;
    version = lm6MeshFile.version;
    dimension = lm6MeshFile.dimension;
}

/**
 * @brief Assignment operator
 * @param[in] lm6MeshFile Reference to the existing Lm6MeshFile
 * @return Reference to Lm6MeshFile instance
 */
Lm6MeshFile &Lm6MeshFile::operator=(const Lm6MeshFile &lm6MeshFile)
{
    Mesh::operator=(lm6MeshFile);
    if (this != &lm6MeshFile) {
        meshIndex = lm6MeshFile.meshIndex;
        version = lm6MeshFile.version;
        dimension = lm6MeshFile.dimension;
    }
    return *this;
}
}
