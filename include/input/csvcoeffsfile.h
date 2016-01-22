/**
 * @file        csvcoeffsfile.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with CSVCoeffsFile class declaration.
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

#ifndef SSIMR_CSVCOEFFSFILE_H
#define SSIMR_CSVCOEFFSFILE_H

#include "../ssimrenderer_global.h"

#include "csvwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The CSVCoeffsFile class represents the structure for Bernstein coeficients loaded from CSV file
 */
class SHARED_EXPORT CSVCoeffsFile : public CSVWrapper
{
public:
    // Creates a CSVCoeffsFile object using the filename
    CSVCoeffsFile(const QString &filename);

    // Destructor of CSVCoeffsFile object
    virtual ~CSVCoeffsFile();

    // Copy constructor
    CSVCoeffsFile(const CSVCoeffsFile &cSVCoeffsFile);

    // Assignment operator
    CSVCoeffsFile &operator=(const CSVCoeffsFile &cSVCoeffsFile);

    // Returns the pointer to coefficients data
    float *getTableOfCoeffs();

    // Returns the number of coefficients
    int getNumberOfCoeffs() const;

    // Returns the number of items
    long getNumberOfItems() const;

    // Returns minimal coefficient
    float getMin() const;

    // Returns maximal coefficient
    float getMax() const;

private:
    int numberOfCoeffs;
    long numberOfItems;
};
}

#endif // SSIMR_CSVCOEFFSFILE_H
