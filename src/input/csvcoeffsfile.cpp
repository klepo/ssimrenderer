/**
 * @file        csvcoeffsfile.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the CSVCoeffsFile class.
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

#include "input/csvcoeffsfile.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a CSVCoeffsFile object using the filename
 * @param[in] filename CSV filename (filepath)
 */
CSVCoeffsFile::CSVCoeffsFile(const QString &filename) : CSVWrapper(filename)
    , numberOfItems(0)
    , numberOfCoeffs(0)
{
    if (data1D.size() > 0) {
        numberOfCoeffs = data2D.at(0).size();
        numberOfItems = data2D.size();
    }

    qDebug() << "Coefficients loaded";
    qDebug() << "\tnumber of coefficients: " << numberOfCoeffs;
    qDebug() << "\tnumber of items: " << numberOfItems;
    qDebug() << "\tmax: " << max;
    qDebug() << "\tmin: " << min;
}

/**
 * @brief Destructor of CSVCoeffsFile object
 *
 * Does nothing.
 */
CSVCoeffsFile::~CSVCoeffsFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] cSVCoeffsFile Original CSVCoeffsFile object to copy
 *
 * Calls base classes copy constructors.
 */
CSVCoeffsFile::CSVCoeffsFile(const CSVCoeffsFile &cSVCoeffsFile) : CSVWrapper(cSVCoeffsFile)
{
    numberOfCoeffs = cSVCoeffsFile.numberOfCoeffs;
    numberOfItems = cSVCoeffsFile.numberOfItems;
}

/**
 * @brief Assignment operator
 * @param[in] cSVCoeffsFile Reference to the existing CSVCoeffsFile
 * @return Reference to CSVCoeffsFile instance
 *
 * Calls base classes copy constructors.
 */
CSVCoeffsFile &CSVCoeffsFile::operator=(const CSVCoeffsFile &cSVCoeffsFile)
{
    CSVWrapper::operator=(cSVCoeffsFile);
    if (this != &cSVCoeffsFile) {
        numberOfCoeffs = cSVCoeffsFile.numberOfCoeffs;
        numberOfItems = cSVCoeffsFile.numberOfItems;
    }
    return *this;
}

/**
 * @brief Returns the pointer to coefficients data
 * @return The pointer to linearly saved coefficients float data
 *
 * Size of the table of coefficients is number of coefficients times number of items.
 */
float *CSVCoeffsFile::getTableOfCoeffs()
{
    return &vector[0];
}

/**
 * @brief Returns the number of coefficients
 * @return Number of coefficients for one tetrahedron
 */
int CSVCoeffsFile::getNumberOfCoeffs() const
{
    return numberOfCoeffs;
}

/**
 * @brief Returns the number of items
 * @return Number of items (Number of tetrahedrons)
 */
long CSVCoeffsFile::getNumberOfItems() const
{
    return numberOfItems;
}

/**
 * @brief Returns minimal coefficient
 * @return Minimal coefficient
 */
float CSVCoeffsFile::getMin() const
{
    return min;
}

/**
 * @brief Returns maximal coefficient
 * @return Maximal coefficient
 */
float CSVCoeffsFile::getMax() const
{
    return max;
}
}
