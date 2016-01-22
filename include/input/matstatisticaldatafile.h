/**
 * @file        matstatisticaldatafile.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with MatStatisticalDataFile class declaration.
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

#ifndef SSIMR_MATSTATISTICALDATAFILE_H
#define SSIMR_MATSTATISTICALDATAFILE_H

#include "../ssimrenderer_global.h"

#include "statisticaldata.h"

#include <matio.h>

namespace SSIMRenderer
{
/**
 * @brief The MatStatisticalDataFile class represents the structure for statistical data loaded from MAT file
 */
class SHARED_EXPORT MatStatisticalDataFile : public StatisticalData
{
public:
    // Creates a MatStatisticalDataFile object using the filename
    MatStatisticalDataFile(const QString &filename);

    // Destructor of MatStatisticalDataFile object
    virtual ~MatStatisticalDataFile();

    // Copy constructor
    MatStatisticalDataFile(const MatStatisticalDataFile &matStatisticalDataFile);

    // Assignment operator
    MatStatisticalDataFile &operator=(const MatStatisticalDataFile &matStatisticalDataFile);

private:
    void loadMatrix(mat_t *matFile, const QString &name, float *(&data), long &rows, long &cols, bool optional = false);
};
}

#endif // SSIMR_MATSTATISTICALDATAFILE_H
