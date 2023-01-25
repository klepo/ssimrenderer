/**
 * @file        DensityImage.cpp
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        25 January 2023
 *
 * @brief       Example demonstrating rendering density images
 *              from a Statistical Shape and Intensity Model (SSIM).
 *
 * @example     DensityImage.cpp
 *
 * This example shows how to render density images from shape and intensity model.
 * The output image is in float values oand stored in the OpenEXR file format.
 * The SSIM is loaded from two MATLAB files containing matrices of PCA-based models that
 * describe bone shape and densities and one Mesh file storing tetrahedral model of a bone.
 * In addition to the rendering it is also shown how to export surface (full or truncated)
 * of the shape model to the STL file format. For further details about initializing
 * the application and setting the project file up please have a look at the example
 * called "SimpleStatismoModel".
 */

#include <QApplication>
#include <QDebug>
#include <ssimrenderer.h>

// #undef DATA_PATH
// #define DATA_PATH "./data"

/**
 * @brief Main function
 * @param argc An integer argument count of the command line arguments
 * @param argv An argument vector of the command line arguments
 * @return An integer 0 upon exit success
 */
int main(int argc, char *argv[])
{
    // Initialization of Qt-based application, QCoreApplication is not sufficient.
    QApplication a(argc, argv);
    Q_UNUSED(a);

    // Initialization of the offscreen renderer.
    SSIMRenderer::OffscreenRenderer *renderer = new SSIMRenderer::OffscreenRenderer(1024, 1024);

    // Input file for tetrahedral mesh in Lm6 Mesh format.
    SSIMRenderer::Lm6MeshFile *meshFile = NULL;
    // Input file for PCA-based shape model corresponding to the previous tetrahedral mesh.
    // The shape model is stored in a custom format as a set of MATLAB matrices.
    SSIMRenderer::MatStatisticalDataFile *shapeFile   = NULL;
    // Input file for PCA-based model of bone densities. The bone densities are described
    // within each tetrahedron using Bernstain polynomials of certain degree (typically 0-3).
    SSIMRenderer::MatStatisticalDataFile *densityFile = NULL;

    try {
        // The Lm6 model contains tetrahedral and even polygonal meshes.
        // Thanks to that it is possible to render virtual X-Ray images,
        // surfaces and even silhouettes from the shape model.

        // Loads the shape model.
        shapeFile   = new SSIMRenderer::MatStatisticalDataFile(DATA_PATH "/shape.mat");

        // Loads the model describing bone densities.
        densityFile = new SSIMRenderer::MatStatisticalDataFile(DATA_PATH "/density.b3.mat");

        // Loads the tetrahedral model.
        meshFile    = new SSIMRenderer::Lm6MeshFile(DATA_PATH "/model.mesh");

    } catch (std::exception &e) {
        // Wrong file
        qFatal(e.what());
        exit(EXIT_FAILURE);
    }

    // Sets the tetrahedral and statistical models to the renderer.
    renderer->setMesh(meshFile);
    renderer->setVertices(shapeFile);
    renderer->setCoefficients(densityFile);

    // Setting up the renderer to render the virtual X-Ray images.
    renderer->enableSilhouettes(false);
    renderer->enableDensity(true);
    renderer->enablePostprocessing(false);
    renderer->enablePyramid(true);
    renderer->enablePolygonal(false);

    // Initialization of the perspective.
    QVector3D eye(        102.8380004f,  551.2176983f, -430.5f);
    QVector3D leftTop(   -408.6619996f, -448.7823017f, -942.f);
    QVector3D leftBottom(-408.6619996f, -448.7823017f,   81.f);
    QVector3D rightTop(   614.3380004f, -448.7823017f, -942.f);
    QVector3D rightBottom(614.3380004f, -448.7823017f,   81.f);
    SSIMRenderer::Pyramid pyramid(leftTop, leftBottom, rightTop, rightBottom, eye);

    // Sets the perspective to the renderer.
    renderer->setPerspective(pyramid);

    // Sets some particular shape parameters.
    shapeFile->updatePcsMatrix(0,  500);
    shapeFile->updatePcsMatrix(1, -500);
    shapeFile->updatePcsMatrix(2,  250);

    renderer->updateVertices(shapeFile);

    // Sets pose of the shape model.
    renderer->setRotation(1.4756f, 3.0457f, 30.784f);
    renderer->setTranslation(111.81f, 47.057f, -437.07f);

    renderer->renderNow();

    // Sets some particular density parameters.
    densityFile->updatePcsMatrix(0, -98299);
    densityFile->updatePcsMatrix(1, 73185);
    densityFile->updatePcsMatrix(2, -300138);
    densityFile->updatePcsMatrix(3, -125250);

    renderer->updateCoefficients(densityFile);

    renderer->renderNow();
    renderer->saveRedChannelToOpenExr(DATA_PATH "/drr.exr");
    qDebug() << "Digitally reconstructed radiograph saved to " DATA_PATH "/drr.exr";


    delete densityFile;
    delete shapeFile;
    delete meshFile;
    delete renderer;
    return EXIT_SUCCESS;
}
