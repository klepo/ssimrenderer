/**
 * @file        matstatisticaldatafile.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the MatStatisticalDataFile class.
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

#include "input/matstatisticaldatafile.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a MatStatisticalDataFile object using the filename
 * @param[in] filename MAT filename (filepath)
 */
MatStatisticalDataFile::MatStatisticalDataFile(const QString &filename)
{
    mat_t *matFile;
    // Try open the file
    matFile = Mat_Open(filename.toStdString().c_str(), MAT_ACC_RDONLY);
    if (NULL == matFile)
        throw std::runtime_error(std::string("Error opening MAT file ") + filename.toStdString());
    qDebug() << "MAT file " << filename << " opened";

    long rows = 0, cols = 0;

    loadMatrix(matFile, T, tMatrix, rows, cols);
    numberOfRows = rows;
    numberOfParameters = cols;
    qDebug() << "\tMatrix \"T\" loaded";

    loadMatrix(matFile, MEAN, meanMatrix, rows, cols);
    if (rows != numberOfRows || cols != 1)
        throw std::runtime_error("Wrong MAT variable (rows or cols count)");
    qDebug() << "\tMatrix \"mean\" loaded";

    loadMatrix(matFile, PC, pcMatrix, rows, cols, true);
    numberOfSettings = cols;
    if (rows != numberOfParameters && numberOfSettings != 0)
        throw std::runtime_error("Wrong MAT variable (rows or cols count)");
    if (numberOfSettings > 0)
        qDebug() << "\tMatrix \"pc\" loaded";

    loadMatrix(matFile, STD, stdMatrix, rows, cols);
    if (cols != numberOfParameters || rows != 1)
        throw std::runtime_error("Wrong MAT variable (rows or cols count)");
    qDebug() << "\tMatrix \"std\" loaded";

    pcsMatrix = new float[numberOfParameters * 1]();
}

/**
 * @brief Destructor of MatStatisticalDataFile object
 *
 * Does nothing.
 */
MatStatisticalDataFile::~MatStatisticalDataFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] matStatisticalDataFile Original MatStatisticalDataFile object to copy
 *
 * Calls base classes copy constructors.
 */
MatStatisticalDataFile::MatStatisticalDataFile(const MatStatisticalDataFile &matStatisticalDataFile) : StatisticalData(matStatisticalDataFile)
{

}

/**
 * @brief Assignment operator
 * @param[in] matStatisticalDataFile Reference to the existing MatStatisticalDataFile
 * @return Reference to MatStatisticalDataFile instance
 *
 * Calls base classes copy constructors.
 */
MatStatisticalDataFile &MatStatisticalDataFile::operator=(const MatStatisticalDataFile &matStatisticalDataFile)
{
    StatisticalData::operator=(matStatisticalDataFile);
    return *this;
}

/**
 * @brief Loads 2D unsigned float matrix from MAT file
 * @param[in] matFile Matfile to load
 * @param[in] name The name of variable
 * @param[out] data Loaded data (the memory is allocated in this function)
 * @param[out] rows Number of rows
 * @param[out] cols Number of cols
 * @param[in] optional Variable with given name does not have to be in file
 */
void MatStatisticalDataFile::loadMatrix(mat_t *matFile, const QString &name, float *(&data), long &rows, long &cols, const bool optional)
{
    matvar_t *matrix;

    matrix = Mat_VarRead(matFile, (char *) name.toStdString().c_str());

    if (NULL == matrix && optional) {
        data = NULL;
        rows = 0;
        cols = 0;
        return;
    }

    if (NULL == matrix)
        throw std::runtime_error(std::string("Error reading MAT variable ") + (char *) name.toStdString().c_str());

    if (matrix->rank != 2)
        throw std::runtime_error(std::string("Wrong MAT variable (rank) ") + (char *) name.toStdString().c_str());

    rows = (long) matrix->dims[0];
    cols = (long) matrix->dims[1];
    data = new float[rows * cols];

    for (long i = 0; i < rows * cols; i++)
        data[i] = (float) ((double *) matrix->data)[i];
}
}
