/**
 * @file        statisticaldata.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the StatisticalData class.
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

#include "input/statisticaldata.h"

namespace SSIMRenderer
{
/** @brief The name of T matrix in statistical file */
const QString StatisticalData::T("T");

/** @brief The name of MEAN matrix in statistical file */
const QString StatisticalData::MEAN("mean");

/** @brief The name of PC matrix in statistical file */
const QString StatisticalData::PC("pc");

/** @brief The name of STD matrix in statistical file */
const QString StatisticalData::STD("std");

/**
 * @brief Creates a StatisticalData object
 */
StatisticalData::StatisticalData()
    : tMatrix(0)
    , meanMatrix(0)
    , pcMatrix(0)
    , pcsMatrix(0)
    , stdMatrix(0)
{

}

/**
 * @brief Destructor of StatisticalData object
 *
 * Deletes matrices
 */
StatisticalData::~StatisticalData()
{
    delete[] tMatrix;
    delete[] meanMatrix;
    delete[] pcMatrix;
    delete[] pcsMatrix;
    delete[] stdMatrix;
}

/**
 * @brief Copy constructor
 * @param[in] statisticalData Original StatisticalData object to copy
 */
StatisticalData::StatisticalData(const StatisticalData &statisticalData)
{
    assign(statisticalData, false);
}

/**
 * @brief Assignment operator
 * @param[in] statisticalData Reference to the existing StatisticalData
 * @return Reference to StatisticalData instance
 */
StatisticalData &StatisticalData::operator=(const StatisticalData &statisticalData)
{
    if (this != &statisticalData) {
        assign(statisticalData, true);
    }
    return *this;
}

/**
 * @brief Returns the pointer to T matrix data
 * @return The pointer to linearly saved T matrix float data
 */
float *StatisticalData::getTMatrix() const
{
    return tMatrix;
}

/**
 * @brief Returns the pointer to MEAN matrix data
 * @return The pointer to linearly saved MEAN matrix float data
 */
float *StatisticalData::getMeanMatrix() const
{
    return meanMatrix;
}

/**
 * @brief Returns the pointer to PC matrix data
 * @return The pointer to linearly saved PC matrix float data
 */
float *StatisticalData::getPcMatrix() const
{
    return pcMatrix;
}

/**
 * @brief Returns the pointer to PCS matrix data
 * @return The pointer to linearly saved PCS matrix float data
 */
float *StatisticalData::getPcsMatrix() const
{
    return pcsMatrix;
}

/**
 * @brief Returns the pointer to STD matrix data
 * @return The pointer to linearly saved STD matrix float data
 */
float *StatisticalData::getStdMatrix() const
{
    return stdMatrix;
}

/**
 * @brief Returns the number of settings
 * @return Number of settings
 */
int StatisticalData::getNumberOfSettings() const
{
    return numberOfSettings;
}

/**
 * @brief Returns the number of parameters
 * @return Number of parameters
 */
int StatisticalData::getNumberOfParameters() const
{
    return numberOfParameters;
}

/**
 * @brief Returns the number of rows
 * @return Number of rows
 */
long StatisticalData::getNumberOfRows() const
{
    return numberOfRows;
}

/**
 * @brief Updates PCS matrix with given value on given index
 * @param[in] index Index to PCS matrix
 * @param[in] value Float value to update
 */
void StatisticalData::updatePcsMatrix(const int index, const float value)
{
    if (index >= numberOfParameters)
        throw std::runtime_error("Wrong index to Pcs matrix");
    pcsMatrix[index] = value;
}

/**
 * @brief Helper function for copy and assign constructors
 * @param[in] statisticalData Original StatisticalData object to copy
 * @param[in] deleteFlag Flag for releasing memory
 */
void StatisticalData::assign(const StatisticalData &statisticalData, const bool deleteFlag)
{
    if (deleteFlag) {
        delete[] tMatrix;
        delete[] meanMatrix;
        delete[] pcMatrix;
        delete[] pcsMatrix;
        delete[] stdMatrix;
    }

    tMatrix = new float[statisticalData.numberOfRows * statisticalData.numberOfParameters]();
    std::memcpy(tMatrix, statisticalData.tMatrix, statisticalData.numberOfRows * statisticalData.numberOfParameters * sizeof(float));

    meanMatrix = new float[statisticalData.numberOfRows * 1]();
    std::memcpy(meanMatrix, statisticalData.meanMatrix, statisticalData.numberOfRows * 1 * sizeof(float));

    pcMatrix = new float[statisticalData.numberOfParameters * statisticalData.numberOfSettings]();
    std::memcpy(pcMatrix, statisticalData.pcMatrix, statisticalData.numberOfParameters * statisticalData.numberOfSettings * sizeof(float));

    pcsMatrix = new float[statisticalData.numberOfParameters * 1]();
    std::memcpy(pcsMatrix, statisticalData.pcsMatrix, statisticalData.numberOfParameters * 1 * sizeof(float));

    stdMatrix = new float[1 * statisticalData.numberOfParameters]();
    std::memcpy(stdMatrix, statisticalData.stdMatrix, 1 * statisticalData.numberOfParameters * sizeof(float));

    numberOfSettings = statisticalData.numberOfSettings;
    numberOfParameters = statisticalData.numberOfParameters;
    numberOfRows = statisticalData.numberOfRows;
}
}
