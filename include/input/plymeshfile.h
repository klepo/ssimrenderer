/**
 * @file        plymeshfile.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        17 November 2015
 *
 * @brief       The header file with PLYMeshFile class declaration.
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

#ifndef SSIMR_PLYMESHFILE_H
#define SSIMR_PLYMESHFILE_H

#include "../ssimrenderer_global.h"

#include "mesh.h"

#include <ply.h>

namespace SSIMRenderer
{
/**
 * @brief The PLYMeshFile class represents the structure for mesh data loaded from PLY file
 */
class SHARED_EXPORT PLYMeshFile : public Mesh
{
public:
    // Creates a PLYMeshFile object using the filename
    PLYMeshFile(const QString &filename);

    // Destructor of PLYMeshFile object
    virtual ~PLYMeshFile();

    // Copy constructor
    PLYMeshFile(const PLYMeshFile &pLYMeshFile);

    // Assignment operator
    PLYMeshFile &operator=(const PLYMeshFile &pLYMeshFile);

private:
    void readFile(FILE *file);
    static int vertex_cb(p_ply_argument argument);
    static int face_cb(p_ply_argument argument);
    struct Data {
        unsigned int *tableOfTriangles;
        float *tableOfVertices;
    } *data;
};
}

#endif // SSIMR_PLYMESHFILE_H
