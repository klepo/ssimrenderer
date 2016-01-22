/**
 * @file        statisticaldata.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with StatisticalData class declaration.
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

#ifndef SSIMR_STATISTICALDATA_H
#define SSIMR_STATISTICALDATA_H

#include "../ssimrenderer_global.h"

#include <QObject>
#include <QDebug>

namespace SSIMRenderer
{
/**
 * @brief The StatisticalData class represents the structure for statistical data
 */
class SHARED_EXPORT StatisticalData : public QObject
{
    Q_OBJECT

public:
    // Default constructor
    StatisticalData();

    // Destructor of StatisticalData object
    virtual ~StatisticalData();

    // Copy constructor
    StatisticalData(const StatisticalData &statisticalData);

    // Assignment operator
    StatisticalData &operator=(const StatisticalData &statisticalData);

    // Returns matrices
    float *getTMatrix() const;
    float *getMeanMatrix() const;
    float *getPcMatrix() const;
    float *getPcsMatrix() const;
    float *getStdMatrix() const;

    // Returns sizes
    int getNumberOfSettings() const;
    int getNumberOfParameters() const;
    long getNumberOfRows() const;

    // Matrix names
    static const QString T;
    static const QString MEAN;
    static const QString PC;
    static const QString STD;

public slots:
    // Updates PCS matrix
    void updatePcsMatrix(int index, float value);

protected:
    /// T Matrix
    float *tMatrix;

    /// MEAN Matrix
    float *meanMatrix;

    /// PC Matrix
    float *pcMatrix;

    /// PCS Matrix
    float *pcsMatrix;

    /// STD Matrix
    float *stdMatrix;

    /// Number of settings
    int numberOfSettings;

    /// Number of parameters
    int numberOfParameters;

    /// Number of rows
    long numberOfRows;

private:
    void assign(const StatisticalData &statisticalData, bool deleteFlag);
};
}

#endif // SSIMR_STATISTICALDATA_H
