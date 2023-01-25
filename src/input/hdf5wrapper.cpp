/**
 * @file        hdf5wrapper.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the HDF5Wrapper class.
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

#include "input/hdf5wrapper.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a HDF5Wrapper object using the filename
 * @param[in] filename HDF5 filename (filepath)
 */
HDF5Wrapper::HDF5Wrapper(const QString &filename)
{
    file = H5Fopen(filename.toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file < 0) {
        throw std::runtime_error(std::string("Error opening HDF5 file ") + filename.toStdString());
    }
    qDebug() << "HDF5 file " << filename << " opened";
}

/**
 * @brief Destructor of HDF5Wrapper object
 *
 * Closes HDF5 file.
 */
HDF5Wrapper::~HDF5Wrapper()
{
    H5Fclose(file);
}

/**
 * @brief Copy constructor
 * @param[in] hDF5Wrapper Original HDF5Wrapper object to copy
 */
HDF5Wrapper::HDF5Wrapper(const HDF5Wrapper &hDF5Wrapper)
{
    file = hDF5Wrapper.file;
}

/**
 * @brief Assignment operator
 * @param[in] hDF5Wrapper Reference to the existing HDF5Wrapper
 * @return Reference to HDF5Wrapper instance
 */
HDF5Wrapper &HDF5Wrapper::operator=(const HDF5Wrapper &hDF5Wrapper)
{
    if (this != &hDF5Wrapper) {
        file = hDF5Wrapper.file;
    }
    return *this;
}

/**
 * @brief Loads 2D float matrix (dataset) from HDF5 file
 * @param[in] name The name of dataset
 * @param[out] data Loaded data (the memory is allocated in this function)
 * @param[out] rows Number of rows
 * @param[out] cols Number of cols
 * @param[in] optional Dataset with given name does not have to be in file
 */
void HDF5Wrapper::load2DMatrix(const QString &name, float *(&data), long &rows, long &cols, const bool optional)
{
    load2DMatrix(name, (void *&) data, rows, cols, H5T_NATIVE_FLOAT, optional);
}

/**
 * @brief Loads 2D unsigned int matrix (dataset) from HDF5 file
 * @param[in] name The name of dataset
 * @param[out] data Loaded data (the memory is allocated in this function)
 * @param[out] rows Number of rows
 * @param[out] cols Number of cols
 * @param[in] optional Dataset with given name does not have to be in file
 */
void HDF5Wrapper::load2DMatrix(const QString &name, unsigned int *(&data), long &rows, long &cols, const bool optional)
{
    load2DMatrix(name, (void *&) data, rows, cols, H5T_NATIVE_UINT, optional);
}

/**
 * @brief Loads 2D matrix (dataset) with given type from HDF5 file
 * @param[in] name The name of dataset
 * @param[out] data Loaded data (the memory is allocated in this function)
 * @param[out] rows Number of rows
 * @param[out] cols Number of cols
 * @param[in] type Datatype of dataset
 * @param[in] optional Dataset with given name does not have to be in file
 */
void HDF5Wrapper::load2DMatrix(const QString &name, void *(&data), long &rows, long &cols, const int type, const bool optional)
{
    hid_t dataset = 0;
    bool exists = false;
    if (H5Lexists(file, const_cast<char *>(name.toStdString().c_str()), 0))
         exists = H5Oexists_by_name(file, const_cast<char *>(name.toStdString().c_str()), 0) != 0;
    if (exists) {
        // Open dataset
        dataset = H5Dopen(file, const_cast<char *>(name.toStdString().c_str()), 0);
        if (dataset < 0){
            throw std::runtime_error("H5Dopen error");
        }
    } else {
        if (optional) {
            rows = 0;
            cols = 0;
            data = 0;
            return;
        }
        throw std::runtime_error("H5Dopen error");
    }

    // Get space
    hid_t dataspace = H5Dget_space(dataset);
    if (dataspace < 0){
        throw std::runtime_error("H5Dget_space error");
    }

    // Get type
    hid_t datatype = H5Dget_type(dataset);
    if (datatype < 0){
        throw std::runtime_error("H5Dget_type error");
    }

    // Check type
    if (!H5Tequal(datatype, type)) {
        if (type != H5T_NATIVE_UINT || !H5Tequal(datatype, H5T_NATIVE_INT))
            throw std::runtime_error("H5Tequal error - wrong datatype of dataset");
    }

    // Get rank and dims
    hsize_t rank = H5Sget_simple_extent_ndims(dataspace);
    if (rank < 0){
        throw std::runtime_error("H5Sget_simple_extent_ndims error");
    }

    // Check rank
    if (rank != 1 && rank != 2){
        throw std::runtime_error("HDF5 error - wrong dataset rank");
    }

    // Get dims
    hsize_t dims[2];
    if (rank == 1){
        dims[1] = 1;
    }
    int dimsCount = H5Sget_simple_extent_dims(dataspace, dims, NULL);
    if (dimsCount < 0){
        throw std::runtime_error("H5Sget_simple_extent_dims error");
    }

    // Allocate memory for given type
    if (type == H5T_NATIVE_UINT)
        data = new unsigned int[dims[0] * dims[1]];
    else if (type == H5T_NATIVE_FLOAT)
        data = new float[dims[0] * dims[1]];
    else
        throw std::runtime_error("Wrong HDF5 datatype");

    // Read dataset
    herr_t status = H5Dread(dataset, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if (status < 0){
        throw std::runtime_error("H5Dread error");
    }

    rows = (long) dims[0];
    cols = (long) dims[1];

    H5Tclose(datatype);
    H5Sclose(dataspace);
    H5Dclose(dataset);
}
}
