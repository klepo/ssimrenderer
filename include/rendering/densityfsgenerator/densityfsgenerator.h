/**
 * @file        densityfsgenerator.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The header file with DensityFSGenerator class declaration.
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

#ifndef SSIMR_FSGENERATOR_H
#define SSIMR_FSGENERATOR_H

#include "../../ssimrenderer_global.h"

#include <QDebug>

namespace SSIMRenderer
{
/**
 * @brief The DensityFSGenerator class represents the structure for generating of
 *        fragment shader code for the integration based on degree of Bernstein basis polynomials.
 */
class SHARED_EXPORT DensityFSGenerator
{
public:
    // Default constructor
    DensityFSGenerator();

    // Destructor of DensityFSGenerator object
    virtual ~DensityFSGenerator();

    // Copy constructor
    DensityFSGenerator(const DensityFSGenerator &densityFSGenerator);

    // Assignment operator
    DensityFSGenerator &operator=(const DensityFSGenerator &densityFSGenerator);

    // Generates fragment shader source code for density rendering
    const QString generateFragmentShaderSourceCode(const int coeffsCount, const bool debugFlag = false);

    // Computes number of coefficients from degree
    int coeffsCountFromDegree(const int degree) const;

    // Tries to compute degree of Bernstein polynomial from number of coefficients
    int degreeFromCoeffsCount(const int coeffsCount) const;

private:
    int factorial(const int n) const;

    QString powString(const int x, const QString y) const;
    QString powVariables(const int degree, const QString b, const QString c) const;
    QString powStringVariable(const int x, const QString b, const QString c) const;

    void generateIjklArray(const int degree, int *outArray, bool debugFlag = false);
    void getLessOrEqualIjklArray(const int index, const int *inArray, int &count, int *outArray, const bool debugFlag = false);

    QString lastSourceCode;
    int lastCoeffsCount;
};
}

#endif // SSIMR_FSGENERATOR_H
