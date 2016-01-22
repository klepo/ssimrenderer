/**
 * @file        hdf5wrapper.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
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

#ifndef SSIMR_HDF5WRAPPER_H
#define SSIMR_HDF5WRAPPER_H

#include "../ssimrenderer_global.h"

#include <hdf5.h>

#include <QDebug>

namespace SSIMRenderer
{
/**
 * @brief The HDF5Wrapper class represents the wrapper for Statismo HDF5 files
 *
 * This class is for loading 2D matrices from Statismo HDF5 files and for opening and closing the file.
 */
class SHARED_EXPORT HDF5Wrapper
{
public:
    // Creates a HDF5Wrapper object using the filename
    HDF5Wrapper(const QString &filename);

    // Destructor of HDF5Wrapper object
    virtual ~HDF5Wrapper();

    // Copy constructor
    HDF5Wrapper(const HDF5Wrapper &hDF5Wrapper);

    // Assignment operator
    HDF5Wrapper &operator=(const HDF5Wrapper &hDF5Wrapper);

protected:
    // Functions for loading 2D matrices from HDF5 file
    void load2DMatrix(const QString &varName, float *&data, long &rows, long &cols, bool optional = false);
    void load2DMatrix(const QString &varName, unsigned int *&data, long &rows, long &cols, bool optional = false);
    void load2DMatrix(const QString &varName, void *&data, long &rows, long &cols, int type = H5T_NATIVE_FLOAT, bool optional = false);

    /// HDF5 file id
    hid_t file;
};
}

#endif // SSIMR_HDF5WRAPPER_H
