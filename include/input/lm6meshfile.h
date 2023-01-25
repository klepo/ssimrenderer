/**
 * @file        lm6meshfile.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with Lm6MeshFile class declaration.
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

#ifndef SSIMR_LM6MESHFILE_H
#define SSIMR_LM6MESHFILE_H

#include "../ssimrenderer_global.h"

#include "mesh.h"

extern "C"
{
    #include <libmeshb7.h>
}

namespace SSIMRenderer
{
/**
 * @brief The Lm6MeshFile class represents the structure for mesh data loaded from The Gamma %Mesh Format file
 *
 * This class is for loading mesh files with tetrahedral data.
 */
class SHARED_EXPORT Lm6MeshFile : public Mesh
{
public:
    // Creates a Lm6MeshFile object using the filename
    Lm6MeshFile(const QString &filename);

    // Destructor of Lm6MeshFile object
    virtual ~Lm6MeshFile();

    // Copy constructor
    Lm6MeshFile(const Lm6MeshFile &lm6MeshFile);

    // Assignment operator
    Lm6MeshFile &operator=(const Lm6MeshFile &lm6MeshFile);

private:
    int meshIndex;
    int version;
    int dimension;
};
}

#endif // SSIMR_LM6MESHFILE_H
