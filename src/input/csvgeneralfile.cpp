/**
 * @file        csvgeneralfile.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the CSVGeneralFile class.
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

#include "input/csvgeneralfile.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a CSVGeneralFile object using the filename
 * @param[in] filename CSV filename (filepath)
 */
CSVGeneralFile::CSVGeneralFile(const QString &filename) : CSVWrapper(filename)
{

}

/**
 * @brief Destructor of CSVGeneralFile object
 *
 * Does nothing.
 */
CSVGeneralFile::~CSVGeneralFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] cSVGeneralFile Original CSVGeneralFile object to copy
 *
 * Calls base classes copy constructors.
 */
CSVGeneralFile::CSVGeneralFile(const CSVGeneralFile &cSVGeneralFile) : CSVWrapper(cSVGeneralFile)
{

}

/**
 * @brief Assignment operator
 * @param[in] cSVGeneralFile Reference to the existing CSVGeneralFile
 * @return Reference to CSVGeneralFile instance
 *
 * Calls base classes copy constructors.
 */
CSVGeneralFile &CSVGeneralFile::operator=(const CSVGeneralFile &cSVGeneralFile)
{
    CSVWrapper::operator=(cSVGeneralFile);
    return *this;
}

/**
 * @brief Returns 2D vector with data
 * @return 2D vector with data
 */
QVector<QVector<float>> CSVGeneralFile::getData2D() const
{
    return data2D;
}

/**
 * @brief Returns 1D vector with data
 * @return 1D vector with data
 */
QVector<float> CSVGeneralFile::getData1D() const
{
    return data1D;
}

/**
 * @brief Returns the pointer to data
 * @return The pointer to linearly saved float data
 *
 * Size of data is equal to size of 1D vector.
 */
float *CSVGeneralFile::getData()
{
    return &vector[0];
}
}
