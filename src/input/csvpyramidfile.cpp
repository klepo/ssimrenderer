/**
 * @file        csvpyramidfile.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The implementation file containing the CSVPyramidFile class.
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

#include "input/csvpyramidfile.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a CSVPyramidFile object using the filename
 * @param[in] filename CSV filename (filepath)
 * @param[in] specialFlag Flag for special initialization of pyramid from 2D data
 */
CSVPyramidFile::CSVPyramidFile(const QString &filename, bool specialFlag) : Pyramid(), CSVWrapper(filename)
{
    init(this->data2D, specialFlag);
}

/**
 * @brief Destructor of CSVPyramidFile object
 *
 * Does nothing.
 */
CSVPyramidFile::~CSVPyramidFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] cSVPyramidFile Original CSVPyramidFile object to copy
 *
 * Calls base classes copy constructors.
 */
CSVPyramidFile::CSVPyramidFile(const CSVPyramidFile &cSVPyramidFile) : Pyramid(cSVPyramidFile), CSVWrapper(cSVPyramidFile)
{

}

/**
 * @brief Assignment operator
 * @param[in] cSVPyramidFile Reference to the existing CSVPyramidFile
 * @return Reference to CSVPyramidFile instance
 *
 * Calls base classes copy constructors.
 */
CSVPyramidFile &CSVPyramidFile::operator=(const CSVPyramidFile &cSVPyramidFile)
{
    Pyramid::operator=(cSVPyramidFile);
    CSVWrapper::operator=(cSVPyramidFile);
    return *this;
}
}







