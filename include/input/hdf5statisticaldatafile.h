/**
 * @file        hdf5statisticaldatafile.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with HDF5StatisticalDataFile class declaration.
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

#ifndef SSIMR_HDF5STATISTICALDATAFILE_H
#define SSIMR_HDF5STATISTICALDATAFILE_H

#include "../ssimrenderer_global.h"

#include "hdf5wrapper.h"
#include "statisticaldata.h"

namespace SSIMRenderer
{
/**
 * @brief The HDF5StatisticalDataFile class represents the structure for statistical data loaded from HDF5 file
 *
 * Works with Statismo file format based on HDF5. Inherits from StatisticalData and HDF5Wrapper classes.
 */
class SHARED_EXPORT HDF5StatisticalDataFile : public StatisticalData, public HDF5Wrapper
{
public:
    // Creates a HDF5StatisticalDataFile object using the filename
    HDF5StatisticalDataFile(const QString &filename);

    // Destructor of HDF5StatisticalDataFile object
    virtual ~HDF5StatisticalDataFile();

    // Copy constructor
    HDF5StatisticalDataFile(const HDF5StatisticalDataFile &hDF5StatisticalDataFile);

    // Assignment operator
    HDF5StatisticalDataFile &operator=(const HDF5StatisticalDataFile &hDF5StatisticalDataFile);

    // Matrix names (datasets) in HDF5 file
    static const QString T;
    static const QString MEAN;
    static const QString PC;
    static const QString STD;
};
}

#endif // SSIMR_HDF5STATISTICALDATAFILE_H
