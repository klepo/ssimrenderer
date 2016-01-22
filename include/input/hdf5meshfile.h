/**
 * @file        hdf5meshfile.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with HDF5MeshFile class declaration.
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

#ifndef SSIMR_HDF5MESHFILE_H
#define SSIMR_HDF5MESHFILE_H

#include "../ssimrenderer_global.h"

#include "hdf5wrapper.h"
#include "mesh.h"

namespace SSIMRenderer
{
/**
 * @brief The HDF5MeshFile class represents the structure for mesh data loaded from HDF5 file
 *
 * Works with Statismo file format based on HDF5. Inherits from Mesh and HDF5Wrapper classes.
 */
class SHARED_EXPORT HDF5MeshFile : public Mesh, public HDF5Wrapper
{
public:
    // Creates a HDF5MeshFile object using the filename
    HDF5MeshFile(const QString &filename);

    // Destructor of HDF5MeshFile object
    virtual ~HDF5MeshFile();

    // Copy constructor
    HDF5MeshFile(const HDF5MeshFile &hDF5MeshFile);

    // Assignment operator
    HDF5MeshFile &operator=(const HDF5MeshFile &hDF5MeshFile);

    // Name of vertices dataset if HDF5 file
    static const QString VERTICES;

    // Name of triangles dataset if HDF5 file
    static const QString TRIANGLES;
};
}

#endif // SSIMR_HDF5MESHFILE_H
