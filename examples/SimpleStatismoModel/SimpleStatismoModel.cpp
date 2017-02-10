/**
 * @file        SimpleStatismoModel.cpp
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        16 December 2015
 *
 * @brief       Example demonstrating offscreen rendering of a simple polygonal shape model.
 *
 * @example     SimpleStatismoModel.cpp
 *
 * This example demonstrates how to use the SSIMRenderer library for rendering images from
 * simple polygonal shape model created by Statismo framework and stored in HDF5 file format.
 * The example covers appropriate initialization of the Qt-based application, creating shared
 * renderers for the same shape model, setting shared pose a shape parameters, setting individual
 * perspectives and rendering settings and storing rendered images in the png file format.
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
    // At the beginning an object of QApplication class has to be created.
    // QCoreApplication class is not sufficient!

    // "QT += widgets" has to be included in the .pro file, even if the
    // target application has only a command-line user interface and it is
    // performing only offscreen rendering.

    QApplication a(argc, argv);
    Q_UNUSED(a);

    // The following line creates an object for an offscreen rendering with a size
    // of the target image set to 1024x1024 pixels.
    SSIMRenderer::OffscreenRenderer *renderer = new SSIMRenderer::OffscreenRenderer(1024, 1024);

    // Two or more renderers can share the same shape model, its pose and shape model parameters.
    // Each renderer still has its own perspective, size and rendering settings (silhouettes, polyg. model etc.).
    SSIMRenderer::OffscreenRenderer *sharingRenderer = new SSIMRenderer::OffscreenRenderer(512, 512, renderer);

    // Files containing a polygonal mesh and corresponding PCA shape model.
    // Coordinates of the vertices should be stored in milimeter units.
    SSIMRenderer::HDF5MeshFile *meshFile = 0;
    SSIMRenderer::HDF5StatisticalDataFile *shapeFile = 0;

    // Tries to load the files.
    try {
        meshFile = new SSIMRenderer::HDF5MeshFile(DATA_PATH "/shape_model.h5");

        // The pose of a shape model should be standardized to a centroid of the shape model,
        // but it is not neccesary. The following example shape model of a femur bone is
        // not translated to its centroid.
        shapeFile = new SSIMRenderer::HDF5StatisticalDataFile(DATA_PATH "/shape_model.h5");

    } catch (std::exception &e) {
        // Wrong file
        qFatal(e.what());
        exit(EXIT_FAILURE);
    }

    // Sets the polygonal mesh to the rendering object
    renderer->setMesh(meshFile);
    // Sets the shape model to the rendering object
    renderer->setVertices(shapeFile);

    // Both polygonal mesh and PCA model must be set to all renderers, even
    // if they are shared.
    sharingRenderer->setMesh(meshFile);
    sharingRenderer->setVertices(shapeFile);

    // Sets the main renderer to render silhouettes of the shape model.
    renderer->enableSilhouettes(true);
    // Sets a thickness of the silhouettes lines.
    renderer->setLineWidth(1);
    // Rendering of virtual X-Ray images from the shape model has to be disabled
    // when the shape model is based on polygonal mesh and the bone densities are
    // not available
    renderer->enableDensity(false);
    renderer->enablePyramid(true);
    renderer->enablePolygonal(false);

    // Disables OpenGL log messages
    renderer->enableLogMessages(false);

    // Sets the second renderer to render a polygonal model.
    sharingRenderer->enableSilhouettes(false);
    sharingRenderer->enablePolygonal(true);
    sharingRenderer->enablePolygonalLighting(true);

    // Creates the perspective pyramid - calibration of the X-Ray machine.
    // The "eye" point defines pose of the X-Ray source, rest four points
    // define corners of an X-Ray detector.
    // The following perspective describes a machine with a detector
    // of a 1x1m size and 1m SID.
    // Coordinates of the points must be in the same units as the coordinates
    // of the shape vertices, i.e. in milimeters.
    QVector3D eye(0, -500, -1450);
    QVector3D leftTop(-500, 500, -950);
    QVector3D leftBottom(-500, 500, -1950);
    QVector3D rightTop(500, 500, -950);
    QVector3D rightBottom(500, 500, -1950);
    SSIMRenderer::Pyramid pyramid(leftTop, leftBottom, rightTop, rightBottom, eye);

    // Sets the perspective to the renderer.
    renderer->setPerspective(pyramid);

    // Creates perspective for another view, orthogonal to the first one.
    eye         = QVector3D(-500, 0,  -1450);
    leftTop     = QVector3D(500, -500, -950);
    leftBottom  = QVector3D(500, -500, -1950);
    rightTop    = QVector3D(500, 500, -950);
    rightBottom = QVector3D(500, 500, -1950);

    pyramid = SSIMRenderer::Pyramid(leftTop, leftBottom, rightTop, rightBottom, eye);
    sharingRenderer->setPerspective(pyramid);

    // Sets parameters of the PCA based shape model to certain values.
    // The parameters are shared between both renderers.
    shapeFile->updatePcsMatrix(0, 1000);
    shapeFile->updatePcsMatrix(1, -500);
    shapeFile->updatePcsMatrix(2,  325);

    // Updates the shape model accordingly to the new parameters.
    renderer->updateVertices(shapeFile);

    // Sets the pose of the shape model. Units are the same as in the case
    // of the shape model vertices (should be [mm]).
    // The pose is shared between both renderers.
    renderer->setTranslation(0, 0, -1485);
    // The rotation values are in degrees.
    renderer->setRotation(10, -5, 60);

    // Renders the bone.
    renderer->renderNow();
    sharingRenderer->renderNow();

    // Save the rendered images.
    renderer->saveRenderedImage(DATA_PATH "/silhouettes.png");
    qDebug() << "Silhouettes image saved to " DATA_PATH "/silhouettes.png";

    sharingRenderer->saveRenderedImage(DATA_PATH "/surface.png");
    qDebug() << "Surface image saved to " DATA_PATH "/surface.png";

    delete shapeFile;
    delete meshFile;
    delete sharingRenderer;
    delete renderer;
    return EXIT_SUCCESS;
}
