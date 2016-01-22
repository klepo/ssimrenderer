/**
 * @file        csvwrapper.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with CSVWrapper class declaration.
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

#ifndef SSIMR_CSVWRAPPER_H
#define SSIMR_CSVWRAPPER_H

#include "../ssimrenderer_global.h"

#include <QVector>
#include <QFile>
#include <QDebug>

namespace SSIMRenderer
{
/**
 * @brief The CSVWrapper class represents the wrapper for CSV files
 */
class SHARED_EXPORT CSVWrapper
{
public:
    // Creates a CSVWrapper object using the filename
    CSVWrapper(const QString &filename);

    // Destructor of CSVWrapper object
    virtual ~CSVWrapper();

    // Copy constructor
    CSVWrapper(const CSVWrapper &cSVWrapper);

    // Assignment operator
    CSVWrapper &operator=(const CSVWrapper &cSVWrapper);

protected:
    /// 2D float array with data
    QVector<QVector<float>> data2D;

    /// 1D float array with data
    QVector<float> data1D;

    /// Vector for direct access to data
    std::vector<float> vector;

    /// Max value
    float max;

    /// Min value
    float min;
};
}

#endif // SSIMR_CSVWRAPPER_H
