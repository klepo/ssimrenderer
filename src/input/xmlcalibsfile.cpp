/**
 * @file        xmlcalibsfile.cpp
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        17 November 2015
 *
 * @brief       The implementation file containing the XMLCalibsFile class.
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

#include "input/xmlcalibsfile.h"

namespace SSIMRenderer
{
/**
 * @brief Creates an XMLCalibsFile object
 * @param[in] filename XML filename (filepath)
 */
XMLCalibsFile::XMLCalibsFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error(std::string("Error opening XML file ") + filename.toStdString());

    read(&file);
    file.close();
}

/**
 * @brief Destructor of XMLCalibsFile object
 *
 * Does nothing.
 */
XMLCalibsFile::~XMLCalibsFile()
{

}

/**
 * @brief Copy constructor
 * @param[in] xMLCalibsFile Original XMLCalibsFile object to copy
 */
XMLCalibsFile::XMLCalibsFile(const XMLCalibsFile &xMLCalibsFile)
{
    myPyramids = xMLCalibsFile.myPyramids;
}

/**
 * @brief Assignment operator
 * @param[in] xMLCalibsFile
 * @return Reference to XMLCalibsFile instance
 */
XMLCalibsFile &XMLCalibsFile::operator=(const XMLCalibsFile &xMLCalibsFile)
{
    if (this != &xMLCalibsFile) {
        myPyramids = xMLCalibsFile.myPyramids;
    }
    return *this;
}

/**
 * @brief Returns list of pyramids
 * @return List of pyramids
 */
QList<Pyramid> XMLCalibsFile::getPyramids() const
{
    return myPyramids;
}

/**
 * @brief Converts QString to QVector3D
 * @param[in] string Source string
 * @return Vector
 */
QVector3D XMLCalibsFile::str2vec(const QString &string)
{
    QVector3D result;
    QStringList list = string.split(" ");
    result.setX(list.at(0).toFloat());
    result.setY(list.at(1).toFloat());
    result.setZ(list.at(2).toFloat());
    return result;
}

/**
 * @brief Reads an XML file
 * @param[in] device Device for reading
 * @return True
 *
 * @todo TODO Why this function returns true?
 */
bool XMLCalibsFile::read(QIODevice *device)
{
    QXmlStreamReader xml(device);
    QList<QVector<QVector3D>> pyramids;

    xml.readNextStartElement();
    if (xml.name() != "traumatech")
        throw std::runtime_error(std::string("Error reading XML file: the first element is not \"traumatech\"."));

    int i = 0;
    int p = -1;

    while (!xml.atEnd()) {
        if (xml.readNext() != QXmlStreamReader::StartElement)
            continue;

        if (xml.name() == "slice") {
            pyramids.append(QVector<QVector3D>(5));
            i = 0;
            p++;
        } else if (xml.name() == "camera") {
            QString text = xml.readElementText();
            pyramids[p][4] = str2vec(text);
        } else if (xml.name() == "ptdet") {
            QString text = xml.readElementText();
            pyramids[p][i] = str2vec(text);
            i++;
        } else {
            xml.skipCurrentElement();
        }
    }

    for (i = 0; i < pyramids.size(); i++) {
        myPyramids.append(Pyramid(pyramids.at(i).at(0), pyramids.at(i).at(1), pyramids.at(i).at(2), pyramids.at(i).at(3), pyramids.at(i).at(4)));
    }

    return true;
}
}
