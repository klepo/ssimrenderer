/**
 * @file        densityfsgenerator.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        4 March 2015
 *
 * @brief       The implementation file containing the DensityFSGenerator class.
 *
 * This class is for generating of fragment shader code for the integration based
 * on degree of Bernstein basis polynomials.
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

#include "rendering/densityfsgenerator/densityfsgenerator.h"

namespace SSIMRenderer
{
/**
 * @brief Default constructor
 *
 * Sets generated fragment shader source code to the simplest version.
 */
DensityFSGenerator::DensityFSGenerator()
{
    lastCoeffsCount = -1;
    lastSourceCode = generateFragmentShaderSourceCode(lastCoeffsCount);
}

/**
 * @brief Destructor of DensityFSGenerator object
 *
 * Does nothing.
 */
DensityFSGenerator::~DensityFSGenerator()
{

}

/**
 * @brief Copy constructor
 * @param[in]   densityFSGenerator Original DensityFSGenerator object to copy
 */
DensityFSGenerator::DensityFSGenerator(const DensityFSGenerator &densityFSGenerator)
{
    lastSourceCode = densityFSGenerator.lastSourceCode;
    lastCoeffsCount = densityFSGenerator.lastCoeffsCount;
}

/**
 * @brief Assignment operator
 * @param[in]   densityFSGenerator Reference to the existing DensityFSGenerator
 * @return Reference to DensityFSGenerator instance
 */
DensityFSGenerator &DensityFSGenerator::operator=(const DensityFSGenerator &densityFSGenerator)
{
    if (this != &densityFSGenerator) {
        lastSourceCode = densityFSGenerator.lastSourceCode;
        lastCoeffsCount = densityFSGenerator.lastCoeffsCount;
    }
    return *this;
}

/**
 * @brief Generates fragment shader source code for density rendering
 * @param[in]   coeffsCount Number of coefficients
 * @param[in]   debugFlag   Print generated items?
 * @return Source code of fragment shader
 */
const QString DensityFSGenerator::generateFragmentShaderSourceCode(int coeffsCount, bool debugFlag)
{
    if (lastCoeffsCount == coeffsCount)
        return lastSourceCode;
    else
        lastCoeffsCount = coeffsCount;

    int *array = 0, degree = 0;
    // 1 / (degree + 1)
    float d1Inv = 1.0f;

    lastSourceCode.clear();

    // Header
    lastSourceCode.append(QString(
                              "#version 330\n"
                              "\n"
                              //// Texture with Bernstein coefficients
                              "uniform samplerBuffer uBernCoeffs;\n"
                              "uniform sampler2D uBernCoeffsDiff;\n"
                              "\n"
                              "uniform int uPositionDiffLengthMinus1;\n"
                              "uniform int uPositionDiffLengthLog2;\n"
                              "uniform float uParam;\n"
                              "\n"
                              //// Output color
                              "out vec4 outColor;\n"
                              "\n"
                              //// Barycentric coords for fragment
                              "in vec4 b;\n"
                              //// Barycentric eye dir
                              "in vec4 bEyedir;\n"
                              //// Eye dir in normalized eye coordinates
                              "in vec4 eEyedir;\n"
                              "\n"
                            ));

    lastSourceCode.append(QString(
                              "void main()\n"
                              "{\n"
                              "    outColor.a = gl_FragCoord.z;\n"
                              "\n"
                              //// Inverted factor s
                              "    vec4 sIV;\n"
                              //// Final factor s and traversal length
                              "    float sIF, wLength;\n"
                              //// Barycentric exit coords from tetrahedron
                              "    vec4 bOut;\n"
                              //// Barycentric input coords to tetrahedron
                              "    vec4 bIn = b;\n"
                              //// Clamp wrong values (artefacts)
                              "    if (bIn[0] < 0 || bIn[1] < 0 || bIn[2] < 0 || bIn[3] < 0) discard;\n"
                              "    if (bIn[0] > 1 || bIn[1] > 1 || bIn[2] > 1 || bIn[3] > 1) discard;\n"
                              "\n"
                              //// Compute factor s
                              "    sIV = -bEyedir / bIn;\n"
                              "\n"
                              //// Select max s
                              "    sIF = max(max(sIV[0], sIV[1]), max(sIV[2], sIV[3]));\n"
                              //// Compute length of traversal
                              "    wLength = length(eEyedir) / sIF;\n"
                              //// Compute bOut and clamp
                              "    bOut = bIn + bEyedir / sIF;\n"
                              "    //if (bOut[0] < -1 || bOut[1] < -1 || bOut[2] < -1 || bOut[3] < -1) discard;\n"
                              "    //if (bOut[0] > 1 || bOut[1] > 1 || bOut[2] > 1 || bOut[3] > 1) discard;\n"
                              "\n"
                              "    outColor.g = wLength;\n"
                              "\n"
                            ));

    // For coefficients generate integration code
    if (coeffsCount > 0) {
        array = new int[coeffsCount * 4]();
        degree = degreeFromCoeffsCount(coeffsCount);
        generateIjklArray(degree, array/*, true*/);
        d1Inv = 1.0f / (degree + 1.0f);

        // Generate some pow numbers
        lastSourceCode.append("\n");
        lastSourceCode.append(powVariables(degree, "bIn", "x"));
        lastSourceCode.append(powVariables(degree, "bIn", "y"));
        lastSourceCode.append(powVariables(degree, "bIn", "z"));
        lastSourceCode.append(powVariables(degree, "bIn", "w"));
        lastSourceCode.append(powVariables(degree, "bOut", "x"));
        lastSourceCode.append(powVariables(degree, "bOut", "y"));
        lastSourceCode.append(powVariables(degree, "bOut", "z"));
        lastSourceCode.append(powVariables(degree, "bOut", "w"));
        lastSourceCode.append("\n");

        lastSourceCode.append(QString(
                                  "    float sum = 0.0f, sum2 = 0.0f;\n"
                                  "\n"
                                  "    float cijkl, bq, bkq;\n"
                                  "    int index;\n"
                                  "    int indexPart = gl_PrimitiveID * " +  QString::number(coeffsCount) + ";\n"
                                  "\n"
                                ));

        for (int c = 0; c < coeffsCount; c++) {
            // Get coefficient value from texture
            lastSourceCode.append(QString(
                                      "    index = indexPart + " +  QString::number(c) + ";\n"
                                      "    cijkl = texelFetch(uBernCoeffs, index).r;\n"
                                      "    cijkl += texelFetch(uBernCoeffsDiff, ivec2(index & uPositionDiffLengthMinus1, index >> uPositionDiffLengthLog2), 0).r;\n"
                                      "\n"
                                    ));

            lastSourceCode.append(QString(
                                      "    sum2 = 0.0f;\n"
                                    ));

            int *outArray = new int[coeffsCount * 4]();
            int k = 0;
            getLessOrEqualIjklArray(c, array, k, outArray);
            k--;

            // For every ijkl item q "less or equal than" k generate inner sum
            for (int q = 0; q <= k; q++) {
                // q
                int qArray[4] = {outArray[q * 4], outArray[q * 4 + 1], outArray[q * 4 + 2], outArray[q * 4 + 3]};
                // k-q
                int kqArray[4] = {outArray[k * 4] - outArray[q * 4], outArray[k * 4 + 1] - outArray[q * 4 + 1], outArray[k * 4 + 2] - outArray[q * 4 + 2], outArray[k * 4 + 3] - outArray[q * 4 + 3]};
                // Degrees
                int dq = qArray[0] + qArray[1] + qArray[2] + qArray[3];
                int dkq = kqArray[0] + kqArray[1] + kqArray[2] + kqArray[3];

                // Parts of Bernstein basis
                float bPartq = ((float) factorial(dq)) / (factorial(qArray[0]) * factorial(qArray[1]) * factorial(qArray[2]) * factorial(qArray[3]));
                float bPartkq = ((float) factorial(dkq)) / (factorial(kqArray[0]) * factorial(kqArray[1]) * factorial(kqArray[2]) * factorial(kqArray[3]));

                QString bPartqString = (bPartq != 1) ? QString::number(bPartq, 'g').prepend(" * ") : QString("");
                QString bPartkqString = (bPartkq != 1) ? QString::number(bPartkq, 'g').prepend(" * ") : QString("");

                // Pows strings
                QString powBxq = powStringVariable(qArray[0], "bIn", "x");
                QString powByq = powStringVariable(qArray[1], "bIn", "y").prepend(" * ");
                QString powBzq = powStringVariable(qArray[2], "bIn", "z").prepend(" * ");
                QString powBwq = powStringVariable(qArray[3], "bIn", "w").prepend(" * ");
                QString powBxkq = powStringVariable(kqArray[0], "bOut", "x");
                QString powBykq = powStringVariable(kqArray[1], "bOut", "y").prepend(" * ");
                QString powBzkq = powStringVariable(kqArray[2], "bOut", "z").prepend(" * ");
                QString powBwkq = powStringVariable(kqArray[3], "bOut", "w").prepend(" * ");

                QString bqbkq = QString(QString(powBxq + powByq + powBzq + powBwq + bPartqString).remove("1 * ").remove(" * 1") + " * " + QString(powBxkq + powBykq + powBzkq + powBwkq + bPartkqString)).remove("1 * ").remove(" * 1");

                /*lastSourceCode.append(QString(
                                          "    bq = " + QString(powBxq + powByq + powBzq + powBwq + bPartqString).remove(" * 1").remove("1 * ") + ";\n"
                                          "    bkq = " + QString(powBxkq + powBykq + powBzkq + powBwkq + bPartkqString).remove(" * 1").remove("1 * ") + ";\n"
                                        ));*/

                lastSourceCode.append(QString(
                                          "    sum2 += " + bqbkq + ";\n"
                                        ));
            }

            delete[] outArray;

            lastSourceCode.append(QString(
                                      "    sum += cijkl * sum2;\n"
                                      "\n"
                                    ));
        }

        // Output color value
        lastSourceCode.append(QString(
                                  "    outColor.r = sum * wLength * " + QString::number(d1Inv, 'g') + ";\n"
                                  "\n"
                                ));



        delete[] array;
    }

    lastSourceCode.append(QString(
                              "}\n"
                            ));

    if (debugFlag)
        qDebug() << lastSourceCode;

    return lastSourceCode;
}

/**
 * @brief Computes number of coefficients from degree
 * @param[in]   degree      Degree of Berstein polynomial
 * @return Number of coefficients
 */
int DensityFSGenerator::coeffsCountFromDegree(int degree) const
{
    return factorial(degree + 3) / (6 * factorial(degree));
}

/**
 * @brief Tries to compute degree of Bernstein polynomial from number of coefficients
 * @param[in]   coeffsCount Number of coefficients
 * @return Degree of Bernstein polynomial (returns 0 for illegal number of coefficients)
 */
int DensityFSGenerator::degreeFromCoeffsCount(int coeffsCount) const
{
    if (coeffsCount == 0) {
        qWarning() << "Wrong coefficients count";
        return -1;
    }

    int degree = 0;
    while (coeffsCount != coeffsCountFromDegree(degree)) {
        degree++;
        if (coeffsCount < coeffsCountFromDegree(degree)) {
            qWarning() << "Wrong coefficients count";
            return -1;
        }
    }

    return degree;
}

/**
 * @brief Simple factorial function
 * @param[in]   number      Input number
 * @return Factorial of number
 */
int DensityFSGenerator::factorial(const int number) const
{
    int fact = 1;
    for (int c = 1; c <= number; c++)
        fact *= c;
    return fact;
}

/**
 * @brief Generates array of ijkl combinations from degree
 * @param[in]   degree      Degree of Berstein polynomial
 * @param[out]  outArray       Array of size 4 x number of coefficients
 * @param[in]   debugFlag Print generated items?
 */
void DensityFSGenerator::generateIjklArray(const int degree, int *outArray, const bool debugFlag)
{
    int i = 0, j = 0, k = 0, l = 0;
    int c = 0;

    // Generate all possible combinations
    while (1) {
        // Select only right combinations: i + j + k + l == degree
        if (i + j + k + l == degree) {
            outArray[c * 4] = i;
            outArray[c * 4 + 1] = j;
            outArray[c * 4 + 2] = k;
            outArray[c * 4 + 3] = l;
            c++;
            if (debugFlag)
                qDebug() << c << ":\t" << i << j << k << l;
        }

        if (i == degree && j == degree && k == degree && l == degree) break;

        if (j == degree && k == degree && l == degree)
            i++;

        if (k == degree && l == degree)
            j++;
        j = j % (degree + 1);

        if (l == degree)
            k++;
        k = k % (degree + 1);

        l++;
        l = l % (degree + 1);
    }
}

/**
 * @brief Generates array with ijkl items less or equal than item selected by index from inArray
 * @param[in]   index       Index of selected item in inArray
 * @param[in]   inArray     Input array
 * @param[out]  count       Number of items in outArray
 * @param[out]  outArray    Array of max size 4 x number of coefficients
 * @param[in]   debugFlag   Print generated items?
 */
void DensityFSGenerator::getLessOrEqualIjklArray(const int index, const int *inArray, int &count, int *outArray, const bool debugFlag)
{
    int newIndex = 0;
    // Get degree of selected item
    int degree = inArray[index * 4] + inArray[index * 4 + 1] + inArray[index * 4 + 2] + inArray[index * 4 + 3];

    for (int d = 0; d < degree; d++) {
        // Temp ijkl array for current degree d
        int *array = new int[coeffsCountFromDegree(d) * 4]();
        generateIjklArray(d, array);
        int i = 0;

        while (i != coeffsCountFromDegree(d)) {
            // Check less or equal values
            if (array[i * 4] <= inArray[index * 4] && array[i * 4 + 1] <= inArray[index * 4 + 1] && array[i * 4 + 2] <= inArray[index * 4 + 2] && array[i * 4 + 3] <= inArray[index * 4 + 3]) {
                outArray[newIndex * 4] = array[i * 4];
                outArray[newIndex * 4 + 1] = array[i * 4 + 1];
                outArray[newIndex * 4 + 2] = array[i * 4 + 2];
                outArray[newIndex * 4 + 3] = array[i * 4 + 3];
                if (debugFlag)
                    qDebug() << outArray[newIndex * 4] << outArray[newIndex * 4 + 1] << outArray[newIndex * 4 + 2] << outArray[newIndex * 4 + 3];
                newIndex++;
            }
            i++;
        }

        delete[] array;
    }
    // Add selected item itself
    outArray[newIndex * 4] = inArray[index * 4];
    outArray[newIndex * 4 + 1] = inArray[index * 4 + 1];
    outArray[newIndex * 4 + 2] = inArray[index * 4 + 2];
    outArray[newIndex * 4 + 3] = inArray[index * 4 + 3];
    count = newIndex + 1;
    if (debugFlag)
        qDebug() << outArray[newIndex * 4] << outArray[newIndex * 4 + 1] << outArray[newIndex * 4 + 2] << outArray[newIndex * 4 + 3];
}

/**
 * @brief Generates QString for pow operation (x^y)
 * @param[in] x Base
 * @param[in] y Exponent
 * @return Generated QString for shader
 */
QString DensityFSGenerator::powString(const int x, const QString y) const
{
    QString ret = y;
    // If x = 0, power is 1
    if (x == 0) {
        ret = "1";
    } else {
        // for example: bIn.x * bIn.x * bIn.x * ...
        for (int i = 1; i < x; i++)
            ret.append(" * " + y);
    }
    return ret;
}

/**
 * @brief Generates strings for array with indexing
 * @param[in] x Value
 * @param[in] b Name of b variable
 * @param[in] c Name of coord
 * @return Generated QString for shader
 */
QString DensityFSGenerator::powStringVariable(const int x, const QString b, const QString c) const
{
    QString ret;
    QString array = b + c.toUpper();
    // If x = 0, power is 1
    if (x == 0) {
        ret = "1";
    } else {
        ret = array + "[" + QString::number(x - 1) + "]";
    }
    return ret;
}

/**
 * @brief Generates strings for array declarations by degree
 * @param degree Degree
 * @param[in] b Name of b variable
 * @param[in] c Name of coord
 * @return Generated QString for shader
 */
QString DensityFSGenerator::powVariables(const int degree, const QString b, const QString c) const
    {
        if (degree < 1)
            return QString("");
        QString ret;
        QString array = b + c.toUpper();
        QString var = b + "." + c;
        ret.append(QString("    float " + array + "[" + QString::number(degree) + "];\n"));
        for (int i = 0; i < degree; i++)
            ret.append(QString("    " + array + "[" + QString::number(i) + "] = " + powString(i + 1, var) + ";\n"));
        return ret;
    }
}
