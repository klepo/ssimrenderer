/**
 * @file        csvwrapper.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the CSVWrapper class.
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

#include "input/csvwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a CSVWrapper object using the filename
 * @param[in] filename HDF5 filename (filepath)
 */
CSVWrapper::CSVWrapper(const QString &filename)
{
    // Try open the file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error(std::string("Error opening CSV file ") + filename.toStdString());

    qDebug() << "CSV file " << filename << " opened";

    QList<QByteArray> list;
    int rows = 0;
    int cols = 0;
    bool firstLine = true;
    // Min/max values
    min = std::numeric_limits<float>::max();
    max = std::numeric_limits<float>::min();

    while (!file.atEnd()) {
        // For every line in file
        QByteArray line = file.readLine();
        // Data (cols) are splitted by ';'
        list = line.split(';');
        // Check number of cols
        if (firstLine) {
            cols = list.count();
            firstLine = false;
        }
        if (cols != list.count())
            throw std::runtime_error(std::string("Error reading CSV file: wrong number of cols"));
        QVector<float> nestedArray;
        // Append values into array
        foreach (QByteArray item, list) {
            // Trim white chars and convert to float
            bool ok;
            float value = item.trimmed().toFloat(&ok);
            nestedArray.append(value);
            data1D.append(value);
            if (!ok)
                throw std::runtime_error(std::string("Error reading CSV file: line ") + std::to_string(rows));
            if (value < min)
                min = value;
            if (value > max)
                max = value;
        }
        data2D.append(nestedArray);
        rows++;
    }

    if (data1D.size() <= 0) {
        min = 0;
        max = 0;
    } else {
        vector = data1D.toStdVector();
    }

    file.close();
    qDebug() << "\tCSV data loaded";

}

/**
 * @brief Destructor of CSVWrapper object
 *
 * Does nothing.
 */
CSVWrapper::~CSVWrapper()
{

}

/**
 * @brief Copy constructor
 * @param[in] cSVWrapper Original CSVWrapper object to copy
 */
CSVWrapper::CSVWrapper(const CSVWrapper &cSVWrapper)
{
    vector = cSVWrapper.vector;
    data1D = cSVWrapper.data1D;
    data2D = cSVWrapper.data2D;
    min = cSVWrapper.min;
    max = cSVWrapper.max;
}

/**
 * @brief Assignment operator
 * @param[in] cSVWrapper Reference to the existing CSVWrapper
 * @return Reference to CSVWrapper instance
 */
CSVWrapper &CSVWrapper::operator=(const CSVWrapper &cSVWrapper)
{
    if (this != &cSVWrapper) {
        vector = cSVWrapper.vector;
        data1D = cSVWrapper.data1D;
        data2D = cSVWrapper.data2D;
        min = cSVWrapper.min;
        max = cSVWrapper.max;
    }
    return *this;
}
}
