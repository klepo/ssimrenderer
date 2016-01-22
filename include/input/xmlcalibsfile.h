/**
 * @file        xmlcalibsfile.h
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        17 November 2015
 *
 * @brief       The header file with XMLCalibsFile class declaration.
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

#ifndef SSIMR_XMLCALIBSFILE_H
#define SSIMR_XMLCALIBSFILE_H

#include "../ssimrenderer_global.h"

#include "pyramid.h"

#include <QXmlStreamReader>
#include <QFile>

namespace SSIMRenderer
{
/**
 * @brief The XMLCalibsFile class represents the structure for calibration data loaded from XML file
 *
 * @todo TODO Ondra - more comments
 */
class SHARED_EXPORT XMLCalibsFile
{
public:
    // Creates an XMLCalibsFile object
    XMLCalibsFile(const QString &filename);

    // Destructor of XMLCalibsFile object
    virtual ~XMLCalibsFile();

    // Copy constructor
    XMLCalibsFile(const XMLCalibsFile &xMLCalibsFile);

    // Assignment operator
    XMLCalibsFile &operator=(const XMLCalibsFile &xMLCalibsFile);

    // Returns list of pyramids
    QList<SSIMRenderer::Pyramid> getPyramids() const;

private:
    bool read(QIODevice *device);
    QVector3D str2vec(const QString &string);
    QList<SSIMRenderer::Pyramid> myPyramids;
};
}

#endif // SSIMR_XMLCALIBSFILE_H
