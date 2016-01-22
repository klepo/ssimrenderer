/**
 * @file        csvgeneralfile.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with CSVGeneralFile class declaration.
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

#ifndef SSIMR_CSVGENERALFILE_H
#define SSIMR_CSVGENERALFILE_H

#include "../ssimrenderer_global.h"

#include "csvwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief The CSVGeneralFile class represents the structure for general float 2D data loaded from CSV file
 */
class SHARED_EXPORT CSVGeneralFile : public CSVWrapper
{
public:
    // Creates a CSVGeneralFile object using the filename
    CSVGeneralFile(const QString &filename);

    // Destructor of CSVGeneralFile object
    virtual ~CSVGeneralFile();

    // Copy constructor
    CSVGeneralFile(const CSVGeneralFile &cSVGeneralFile);

    // Assignment operator
    CSVGeneralFile &operator=(const CSVGeneralFile &cSVGeneralFile);

    // Returns 2D vector with data
    QVector<QVector<float>> getData2D() const;

    // Returns 1D vector with data
    QVector<float> getData1D() const;

    // Returns the pointer to coefficients data
    float *getData();
};
}

#endif // SSIMR_CSVGENERALFILE_H
