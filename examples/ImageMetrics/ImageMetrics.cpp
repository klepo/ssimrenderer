/**
 * @file        ImageMetrics.cpp
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        16 December 2015
 *
 * @brief       Example demonstrating computation of image similarity metrics between
 *              two virtual X-Ray images.
 * @example     ImageMetrics.cpp
 *
 * This example renders two virtual X-Ray images, these images are then used to evaluate
 * normalized mutual information and sum of squared differences image similarity metrics.
 * Both metrics are implemented in CPU and OpenGL versions. For further details about
 * setting the project file up, initialization of the application and rendering virtual X-Ray
 * images, please have a look at the "SimpleStatismoModel" and "IntensityShapeModel" examples.
 */

#include <QApplication>
#include <QDebug>
#include <ssimrenderer.h>

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

    // Initialization of CPU and OpenGL versions of the NMI metric.
    // The image from the renderer will be used as one of the inputs for the metric evaluation.
    SSIMRenderer::NMIComputingCPU    * nmiCPU    = new SSIMRenderer::NMIComputingCPU(renderer);
    SSIMRenderer::NMIComputingOpenGL * nmiOpenGL = new SSIMRenderer::NMIComputingOpenGL(renderer);

    // Initialization of CPU and OpenGL versions of the SSD metric.
    SSIMRenderer::SSDComputingCPU    * ssdCPU    = new SSIMRenderer::SSDComputingCPU(renderer);
    SSIMRenderer::SSDComputingOpenGL * ssdOpenGL = new SSIMRenderer::SSDComputingOpenGL(renderer);

    // Input files for the shape model.
    SSIMRenderer::Lm6MeshFile *meshFile = NULL;
    SSIMRenderer::MatStatisticalDataFile *shapeFile   = NULL;
    SSIMRenderer::MatStatisticalDataFile *densityFile = NULL;

    try {
        meshFile    = new SSIMRenderer::Lm6MeshFile(DATA_PATH "/model.mesh");
        shapeFile   = new SSIMRenderer::MatStatisticalDataFile(DATA_PATH "/shape.mat");
        densityFile = new SSIMRenderer::MatStatisticalDataFile(DATA_PATH "/density.b3.mat");

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

    // Download the image from the renderer and use it also as the second
    // input to the metrics, so the metrics will be evaluated using two
    // exactly the same images.

    QImage image = renderer->getRenderedImage();
    nmiCPU->setInputImage(image);
    nmiOpenGL->setInputImage(image);
    ssdCPU->setInputImage(image);
    ssdOpenGL->setInputImage(image);

    qDebug() << "";
    qDebug() << "Image similarity metrics evaluated on the same images ";
    qDebug() << "------------------------------------------------------";
    qDebug() << "Normalized mutual information | CPU    |" << nmiCPU->compute();
    qDebug() << "Normalized mutual information | OpenGL |" << nmiOpenGL->compute();
    qDebug() << "Sum of squared differences    | CPU    |" << ssdCPU->compute();
    qDebug() << "Sum of squared differences    | OpenGL |" << ssdOpenGL->compute();
    qDebug() << "";

    // Slightly change the pose of the shape model and render new virtual X-Ray images,
    // so the first input to the metrics will be updated. Consequently, the metrics will
    // be evaluated on two different images this time.

    renderer->setRotation(1.2756f, 3.4457f, 33.784f);
    renderer->setTranslation(110.81f, 48.057f, -436.07f);
    renderer->renderNow();

    qDebug() << "";
    qDebug() << "Image similarity metrics evaluated on different images ";
    qDebug() << "------------------------------------------------------";
    qDebug() << "Normalized mutual information | CPU    |" << nmiCPU->compute();
    qDebug() << "Normalized mutual information | OpenGL |" << nmiOpenGL->compute();
    qDebug() << "Sum of squared differences    | CPU    |" << ssdCPU->compute();
    qDebug() << "Sum of squared differences    | OpenGL |" << ssdOpenGL->compute();
    qDebug() << "";

    delete densityFile;
    delete shapeFile;
    delete meshFile;
    delete renderer;
    return EXIT_SUCCESS;
}
