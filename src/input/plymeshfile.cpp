/**
 * @file        plymeshfile.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        17 November 2015
 *
 * @brief       The implementation file containing the PLYMeshFile class.
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

#include "input/plymeshfile.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a PLYMeshFile object using the filename
 * @param[in] filename PLY filename (filepath)
 */
PLYMeshFile::PLYMeshFile(const QString &filename) : Mesh()
{
    bool first = true;

    p_ply ply = ply_open(filename.toStdString().c_str(), NULL, 0, NULL);
    if (!ply)
        throw std::runtime_error("Cannot open PLY file");
    if (!ply_read_header(ply))
        throw std::runtime_error("Cannot read PLY file header");

    qDebug() << "PLY file " << filename << " opened";

    data = new Data();
    data->tableOfVertices = 0;
    data->tableOfTriangles = 0;

    numberOfVertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, data, 0);
    ply_set_read_cb(ply, "vertex", "y", vertex_cb, data, 1);
    ply_set_read_cb(ply, "vertex", "z", vertex_cb, data, 2);
    numberOfTriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, data, 0);

    if (numberOfVertices > 0) {
        tableOfVertices = new float[numberOfVertices * 3]();
        data->tableOfVertices = tableOfVertices;
    } else {
        throw std::runtime_error("Wrong PLY file (numberOfVertices)");
    }

    if (numberOfTriangles > 0) {
        tableOfTriangles = new unsigned int[numberOfTriangles * 3]();
        data->tableOfTriangles = tableOfTriangles;
    } else {
        throw std::runtime_error("Wrong PLY file (numberOfVertices)");
    }

    // Load Triangles adjacency
    tableOfTrianglesAdjacency = new unsigned int[numberOfTriangles * 6]();
    numberOfTrianglesAdjacency = numberOfTriangles;
    //qDebug() << "\tnumberOfTrianglesAdjacency =" << numberOfTrianglesAdjacency;

    if (!ply_read(ply))
        throw std::runtime_error("Cannot read PLY file header");

    qDebug() << "\tnumberOfVertices" << numberOfVertices;
    qDebug() << "\tVertices loaded";
    qDebug() << "\tnumberOfTriangles" << numberOfTriangles;
    qDebug() << "\tTriangles loaded";

    ply_close(ply);

    // Get min and max vertices
    for (int i = 0; i < numberOfVertices; i++) {
        if (first) {
            maxVertex[0] = tableOfVertices[i * 3 + 0];
            maxVertex[1] = tableOfVertices[i * 3 + 1];
            maxVertex[2] = tableOfVertices[i * 3 + 2];
            minVertex[0] = tableOfVertices[i * 3 + 0];
            minVertex[1] = tableOfVertices[i * 3 + 1];
            minVertex[2] = tableOfVertices[i * 3 + 2];
            first = false;
        }
        if (maxVertex[0] < tableOfVertices[i * 3 + 0])
            maxVertex[0] = tableOfVertices[i * 3 + 0];
        if (maxVertex[1] < tableOfVertices[i * 3 + 1])
            maxVertex[1] = tableOfVertices[i * 3 + 1];
        if (maxVertex[2] < tableOfVertices[i * 3 + 2])
            maxVertex[2] = tableOfVertices[i * 3 + 2];
        if (minVertex[0] > tableOfVertices[i * 3 + 0])
            minVertex[0] = tableOfVertices[i * 3 + 0];
        if (minVertex[1] > tableOfVertices[i * 3 + 1])
            minVertex[1] = tableOfVertices[i * 3 + 1];
        if (minVertex[2] > tableOfVertices[i * 3 + 2])
            minVertex[2] = tableOfVertices[i * 3 + 2];
    }

    generateTableOfTrianglesAdjacency();
    qDebug() << "\tTriangles Adjacency generated";

    generateTableOfNormals();
}

/**
 * @brief Destructor of PLYMeshFile object
 *
 * Does nothing.
 */
PLYMeshFile::~PLYMeshFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] pLYMeshFile Original PLYMeshFile object to copy
 */
PLYMeshFile::PLYMeshFile(const PLYMeshFile &pLYMeshFile) : Mesh(pLYMeshFile)
{
    data = new Data();
    data->tableOfVertices = pLYMeshFile.data->tableOfVertices;
    data->tableOfTriangles = pLYMeshFile.data->tableOfTriangles;
}

/**
 * @brief Assignment operator
 * @param[in] pLYMeshFile Reference to the existing PLYMeshFile
 * @return Reference to PLYMeshFile instance
 */
PLYMeshFile &PLYMeshFile::operator=(const PLYMeshFile &pLYMeshFile)
{
    Mesh::operator=(pLYMeshFile);
    if (this != &pLYMeshFile) {
        delete data;
        data = new Data();
        data->tableOfVertices = pLYMeshFile.data->tableOfVertices;
        data->tableOfTriangles = pLYMeshFile.data->tableOfTriangles;
    }
    return *this;
}

/**
 * @brief Callback function for reading vertex
 * @param[in] argument Argument for vertex
 * @return 1
 */
int PLYMeshFile::vertex_cb(p_ply_argument argument) {
    Data *data = new Data();
    long idata;
    long index;
    ply_get_argument_user_data(argument, (void **) &data, &idata);
    ply_get_argument_element(argument, NULL, &index);
    float value = (float) ply_get_argument_value(argument);
    data->tableOfVertices[index * 3 + idata] = value;
    return 1;
}

/**
 * @brief Callback function for reading triangle
 * @param[in] argument Argument for triangle
 * @return 1
 */
int PLYMeshFile::face_cb(p_ply_argument argument) {
    long length, value_index;
    Data *data = new Data();
    long idata;
    long index;
    ply_get_argument_user_data(argument, (void **) &data, &idata);
    ply_get_argument_property(argument, NULL, &length, &value_index);
    ply_get_argument_element(argument, NULL, &index);
    if (value_index >= 0) {
        unsigned int value = (unsigned int) ply_get_argument_value(argument);
        data->tableOfTriangles[index * length + value_index] = value;
    }
    return 1;
}
}
