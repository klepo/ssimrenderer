/**
 * @file        hdf5statisticaldatafile.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the HDF5StatisticalDataFile class.
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

#include "input/hdf5statisticaldatafile.h"

namespace SSIMRenderer
{
/** @brief The name of T matrix in Statismo HDF5 file */
const QString HDF5StatisticalDataFile::T("model/pcaBasis");

/** @brief The name of MEAN matrix in Statismo HDF5 file */
const QString HDF5StatisticalDataFile::MEAN("model/mean");

/** @brief The name of PC matrix in Statismo HDF5 file */
const QString HDF5StatisticalDataFile::PC("modelinfo/scores");

/** @brief The name of STD matrix in Statismo HDF5 file */
const QString HDF5StatisticalDataFile::STD("model/pcaVariance");

/**
 * @brief Creates a HDF5StatisticalDataFile object using the filename
 * @param[in] filename HDF5 filename (filepath)
 */
HDF5StatisticalDataFile::HDF5StatisticalDataFile(const QString &filename) : HDF5Wrapper(filename)
{
    long rows = 0, cols = 0;

    load2DMatrix(T, tMatrix, rows, cols);
    numberOfRows = rows;
    numberOfParameters = cols;
    qDebug() << "\tMatrix \"T\" loaded";

    load2DMatrix(PC, pcMatrix, rows, cols, true);
    numberOfSettings = cols;
    if (rows != numberOfParameters && numberOfSettings != 0)
        throw std::runtime_error("Wrong MAT variable (rows or cols count)");
    if (numberOfSettings > 0)
        qDebug() << "\tMatrix \"pc\" loaded";

    load2DMatrix(MEAN, meanMatrix, rows, cols);
    if (rows != numberOfRows || cols != 1)
        throw std::runtime_error("Wrong MAT variable (rows or cols count)");
    qDebug() << "\tMatrix \"mean\" loaded";

    // Different order of roows and cols - not same as in Matlab file!!!
    load2DMatrix(STD, stdMatrix, rows, cols);
    if (rows != numberOfParameters || cols != 1)
        throw std::runtime_error("Wrong MAT variable (rows or cols count)");

    // Recompute std

    for (int i = 0; i < rows; i++)
        stdMatrix[i] = sqrt(stdMatrix[i]);

    qDebug() << "\tMatrix \"std\" loaded";

    // Recompute pc
    for (int setting = 0; setting < numberOfSettings; setting++) {
        for (int parameter = 0; parameter < numberOfParameters; parameter++) {
            pcMatrix[parameter * numberOfSettings + setting] *= stdMatrix[parameter];
        }
    }

    pcsMatrix = new float[numberOfParameters * 1]();
}

/**
 * @brief Destructor of HDF5StatisticalDataFile object
 *
 * Does nothing.
 */
HDF5StatisticalDataFile::~HDF5StatisticalDataFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] hDF5StatisticalDataFile Original HDF5StatisticalDataFile object to copy
 *
 * Calls base classes copy constructors.
 */
HDF5StatisticalDataFile::HDF5StatisticalDataFile(const HDF5StatisticalDataFile &hDF5StatisticalDataFile) : StatisticalData(hDF5StatisticalDataFile), HDF5Wrapper(hDF5StatisticalDataFile)
{

}

/**
 * @brief Assignment operator
 * @param[in] hDF5StatisticalDataFile Reference to the existing HDF5StatisticalDataFile
 * @return Reference to HDF5StatisticalDataFile instance
 *
 * Calls base classes copy constructors.
 */
HDF5StatisticalDataFile &HDF5StatisticalDataFile::operator=(const HDF5StatisticalDataFile &hDF5StatisticalDataFile)
{
    StatisticalData::operator=(hDF5StatisticalDataFile);
    HDF5Wrapper::operator=(hDF5StatisticalDataFile);
    return *this;
}
}
