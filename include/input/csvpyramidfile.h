/**
 * @file        csvpyramidfile.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The header file with CSVPyramidFile class declaration.
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

#ifndef SSIMR_CSVPYRAMIDFILE_H
#define SSIMR_CSVPYRAMIDFILE_H

#include "../ssimrenderer_global.h"

#include "csvwrapper.h"
#include "pyramid.h"

namespace SSIMRenderer
{
/**
 * @brief The CSVPyramidFile class represents the structure for %pyramid loaded from CSV file
 */
class SHARED_EXPORT CSVPyramidFile : public Pyramid, public CSVWrapper
{
public:
    // Creates a CSVPyramidFile object using the filename and special flag
    CSVPyramidFile(const QString &filename, bool specialFlag = false);

    // Destructor of CSVPyramidFile object
    virtual ~CSVPyramidFile();

    // Copy constructor
    CSVPyramidFile(const CSVPyramidFile &cSVPyramidFile);

    // Assignment operator
    CSVPyramidFile &operator=(const CSVPyramidFile &cSVPyramidFile);
};
}

#endif // SSIMR_CSVPYRAMIDFILE_H
