/**
 * @file        mainrenderer.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The implementation file containing the MainRenderer class.
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

#include "rendering/mainrenderer.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a MainRenderer with surface and optional parental OpenGLWrapper
 * @param[in] surface Surface usually of type QOffscreenSurface or QWindow
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
MainRenderer::MainRenderer(QSurface *surface, OpenGLWrapper *parentOpenGLWrapper)
    : OpenGLWrapper(surface, parentOpenGLWrapper)
{
    init();
    checkInitAndMakeCurrentContext();
}

/**
 * @brief Creates a MainRenderer with surface and optional parental OpenGLWrapper
 * @param[in] surface Surface usually of type QOffscreenSurface or QW
 * @param[in] renderWidth Render width
 * @param[in] renderHeight Render height
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
MainRenderer::MainRenderer(QSurface *surface, GLuint renderWidth, GLuint renderHeight, OpenGLWrapper *parentOpenGLWrapper)
    : OpenGLWrapper(surface, parentOpenGLWrapper)
{
    init();
    checkInitAndMakeCurrentContext();
    setRenderWidth(renderWidth);
    setRenderHeight(renderHeight);
}

/**
 * @brief Destructor of OffscreenRenderer object
 *
 * Deletes some OpenGL memory objects.
 */
MainRenderer::~MainRenderer()
{
    checkInitAndMakeCurrentContext();

    // Delete programs and others
    if (!hasSharedContext() && density->program) {
        //density->program->release();
        delete density->program;
        delete density->fragmentShader;
        delete density;
    }

    if (!hasSharedContext() && silhouettes->program) {
        //silhouettes->program->release();
        delete silhouettes->program;
        delete silhouettes;
    }

    if (!hasSharedContext() && computing->program) {
        //computing->program->release();
        delete computing->program;
        delete computing;
    }

    if (!hasSharedContext() && pyramid->program) {
        //pyramid->program->release();
        delete pyramid->program;
        delete pyramid;
    }

    if (!hasSharedContext() && polygonal->program) {
        //polygonal->program->release();
        delete polygonal->program;
        delete polygonal;
    }

    if (!hasSharedContext() && postprocessing->program && postprocessing->programSimple) {
        //postprocessing->program->release();
        delete postprocessing->program;
        delete postprocessing->programSimple;
        delete postprocessing;
    }

    if (!hasSharedContext()) {
        iboElementsTetrahedra.destroy();
        iboElementsTriangles.destroy();
        iboElementsTrianglesAdjacency.destroy();

        vboVertices.destroy();
        vboVerticesColors.destroy();
        //vboComputeIndicesX.destroy();
        //vboComputeIndicesY.destroy();
        vboNormals.destroy();

        glDeleteBuffers(1, &tboBerncoeffs);
        glDeleteBuffers(1, &tboT);
        glDeleteBuffers(1, &tboPcs);

        glDeleteTextures(1, &toCompCoeffs);
        glDeleteTextures(1, &toCompVertices);
        glDeleteTextures(1, &toBerncoeffs);
        glDeleteTextures(1, &toT);
        glDeleteTextures(1, &toPcs);
    }

    glDeleteTextures(1, &toDensity);
    glDeleteTextures(1, &toSilhouettes);
    glDeleteTextures(1, &toPyramid);
    glDeleteTextures(1, &toPolygonal);
    glDeleteTextures(1, &toOutput);
    glDeleteTextures(1, &toPyramidImage);

    glDeleteRenderbuffers(1, &rbo);

    glDeleteVertexArrays(1, &vao);

    glDeleteFramebuffers(1, &fbo);
    glDeleteFramebuffers(1, &fboOutput);
    glDeleteFramebuffers(1, &fboComputing);

}

/**
 * @brief Sets mesh and optional colors and normals
 * @param[in] mesh Mesh
 * @param[in] colors An array with colors
 * @param[in] normals An array with normals
 */
void MainRenderer::setMesh(Mesh *mesh, GLfloat *colors, GLfloat *normals)
{
    if (!mesh) {
        qCritical() << "MainRenderer::setMesh error: null Mesh";
        return;
    }

    checkInitAndMakeCurrentContext();

    this->mesh = mesh;

    // Load vertices and indices data to GPU by main context
    if (!hasSharedContext()) {

        QList<OpenGLWrapper *> childOpenGLWrappers = getChildOpenGLWrappers();
        for (int i = 0; i < childOpenGLWrappers.size(); i++) {
            OpenGLWrapper *renderSurface = childOpenGLWrappers.at(i);
            if (dynamic_cast<MainRenderer *>(renderSurface)) {
                ((MainRenderer *) renderSurface)->mesh = mesh;
            }
        }

        if (this->mesh->getNumberOfTetrahedra() == 0)
            qWarning() << "Tetrahedral mesh is not available (mesh->getNumberOfTetrahedra() == 0)";

        iboElementsTetrahedra.bind();
        iboElementsTetrahedra.allocate(this->mesh->getTableOfTetrahedra(), sizeof(GLuint) * this->mesh->getNumberOfTetrahedra() * 4);
        iboElementsTetrahedra.release();

        iboElementsTriangles.bind();
        iboElementsTriangles.allocate(this->mesh->getTableOfTriangles(), sizeof(GLuint) * this->mesh->getNumberOfTriangles() * 3);
        iboElementsTriangles.release();

        if (colors) {
            setVerticesColors(colors);
        } else {
            float *colors = new float[3 * mesh->getNumberOfVertices()]();
            std::fill(colors, colors + 3 * mesh->getNumberOfVertices(), 1.0f);
            setVerticesColors(colors);
            delete[] colors;
        }

        if (normals)
            setNormals(normals);
        else
            setNormals(this->mesh->getTableOfNormals());

        if (this->mesh->getNumberOfTrianglesAdjacency() == 0)
            qWarning() << "Triangles adjacency are not available (mesh->getNumberOfTrianglesAdjacency() == 0)";

        iboElementsTrianglesAdjacency.bind();
        iboElementsTrianglesAdjacency.allocate(this->mesh->getTableOfTrianglesAdjacency(), sizeof(GLuint) * this->mesh->getNumberOfTrianglesAdjacency() * 6);
        iboElementsTrianglesAdjacency.release();

        vboVertices.bind();
        vboVertices.allocate(this->mesh->getTableOfVertices(), sizeof(GLfloat) * this->mesh->getNumberOfVertices() * 3);
        vboVertices.release();
    }
}

/**
 * @brief Sets vertices colors
 * @param[in] colors An array with colors
 */
void MainRenderer::setVerticesColors(GLfloat *colors)
{
    if (!mesh) {
        qCritical() << "MainRenderer::setVerticesColors error: null Mesh";
        return;
    }

    checkInitAndMakeCurrentContext();

    if (!hasSharedContext()) {
        vboVerticesColors.bind();
        if (colors)
            vboVerticesColors.allocate(colors, sizeof(GLfloat) * mesh->getNumberOfVertices() * 3); // @todo TODO
        else
            vboVerticesColors.allocate(sizeof(GLfloat) * mesh->getNumberOfVertices() * 3); // @todo TODO
        vboVerticesColors.release();
    }
}

/**
 * @brief Sets vetrices normals
 * @param[in] normals An array with normals
 */
void MainRenderer::setNormals(GLfloat *normals)
{
    if (!mesh) {
        qCritical() << "MainRenderer::setNormals error: null Mesh";
        return;
    }

    checkInitAndMakeCurrentContext();

    if (!hasSharedContext()) {
        vboNormals.bind();
        if (normals)
            vboNormals.allocate(normals, sizeof(GLfloat) * mesh->getNumberOfVertices() * 3); // @todo TODO
        else
            vboNormals.allocate(sizeof(GLfloat) * mesh->getNumberOfVertices() * 3); // @todo TODO
        vboNormals.release();
    }
}

/**
 * @brief Sets statistical coefficients data for recomputing
 * @param[in] statisticalData Statistical coefficients data
 *
 * Schedules fast recomputing in rendering function.
 */
void MainRenderer::setCoefficients(StatisticalData *statisticalData)
{
    if (!statisticalData) {
        qCritical() << "MainRenderer::setCoefficients error: null StatisticalData";
        return;
    }

    if (!mesh) {
        qCritical() << "MainRenderer::setCoefficients error: null Mesh";
        return;
    }

    checkInitAndMakeCurrentContext();

    if (!hasSharedContext()) {
        if (mesh->getNumberOfTetrahedra() == 0) {
            qWarning() << "Tetrahedral mesh is not available (mesh->getNumberOfTetrahedra() == 0)";
            return;
        }
        int bernCoeffsCount = statisticalData->getNumberOfRows() / mesh->getNumberOfTetrahedra();  // @todo TODO check size

        glBindBuffer(GL_TEXTURE_BUFFER, tboBerncoeffs);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(GLfloat) * statisticalData->getNumberOfRows(), statisticalData->getMeanMatrix(), GL_STATIC_DRAW);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        glBindTexture(GL_TEXTURE_BUFFER, toBerncoeffs);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tboBerncoeffs);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        if (lastBernCoeffsCount != bernCoeffsCount) {
            density->program->removeShader(density->fragmentShader);
            addShaderFromSource(density->program, density->fragmentShader, fsGenerator.generateFragmentShaderSourceCode(bernCoeffsCount/*, true*/));
            linkProgram(density->program);

            getVariablesLocations();
            initUniformVariables();

            density->program->bind();
            density->program->setUniformValue(density->uPositionDiffLengthLog2, positionDiffLengthLog2);
            density->program->setUniformValue(density->uPositionDiffLengthMinus1, positionDiffLengthMinus1);
            density->program->release();
        }

        lastBernCoeffsCount = bernCoeffsCount;
    }

    if (this->statisticalData != statisticalData) {
        setStatisticalData(statisticalData);
        updateCoefficients(statisticalData);
    }
}

/**
 * @brief Sets statistical vertices data for recomputing
 * @param[in] statisticalData Statistical vertices data
 *
 * Schedules fast recomputing in rendering function.
 */
void MainRenderer::setVertices(StatisticalData *statisticalData)
{
    if (!statisticalData) {
        qCritical() << "MainRenderer::setVertices error: null StatisticalData";
        return;
    }

    checkInitAndMakeCurrentContext();

    if (!hasSharedContext()) {
        // @todo TODO check size

        vboVertices.bind();
        vboVertices.allocate(statisticalData->getMeanMatrix(), sizeof(GLfloat) * statisticalData->getNumberOfRows());
        vboVertices.release();

        if (this->statisticalData != statisticalData) {
            setStatisticalData(statisticalData);
            updateVertices(statisticalData);
        }
    }
}

/**
 * @brief Updates statistical coefficients data for recomputing
 * @param[in] statisticalData Statistical coefficients data
 *
 * Schedules fast recomputing in rendering function.
 */
void MainRenderer::updateCoefficients(StatisticalData *statisticalData)
{
    if (!statisticalData) {
        qCritical() << "MainRenderer::updateCoefficients error: null StatisticalData";
        return;
    }

    checkInitAndMakeCurrentContext();

    if (this->statisticalData != statisticalData) {
        setStatisticalData(statisticalData);
        setCoefficients(statisticalData);
    }

    if (!hasSharedContext())
        recomputeCoefficientsDiffFlag = true;
}

/**
 * @brief Updates statistical vertices data for recomputing
 * @param[in] statisticalData Statistical vertices data
 *
 * Schedules fast recomputing in rendering function.
 */
void MainRenderer::updateVertices(StatisticalData *statisticalData)
{
    if (!statisticalData) {
        qCritical() << "MainRenderer::updateVertices error: null StatisticalData";
        return;
    }

    checkInitAndMakeCurrentContext();

    if (this->statisticalData != statisticalData) {
        setStatisticalData(statisticalData);
        setVertices(statisticalData);
    }

    if (!hasSharedContext())
        recomputeVerticesDiffFlag = true;
}

/**
 * @brief Returns recomputed vertices
 * @param[out] vertices Output array with vertices
 *
 * Vertices output array is allocated in this function.
 */
long MainRenderer::getRecomputedVertices(float *&vertices, bool transformed)
{
    if (!mesh) {
        qCritical() << "MainRenderer::getRecomputedVertices error: null Mesh";
        return 0;
    }

    checkInitAndMakeCurrentContext();

    float *dataDiff = new float[cWidth * cHeight]();

    glBindTexture(GL_TEXTURE_2D, toCompVertices);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, dataDiff);
    glBindTexture(GL_TEXTURE_2D, 0);

    float *dataMean;

    vboVertices.bind();
    dataMean = (float *) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

    long size = mesh->getNumberOfVertices();
    vertices = new float[size * 3]();

    // @todo TODO - this is slowly solution

    for (int i = 0; i < size; i++) {
        // Invert x coordinate
        if (xMirroringEnabled)
            vertices[i * 3] = -(dataMean[i * 3] + dataDiff[i * 3]);
        else
            vertices[i * 3] = dataMean[i * 3] + dataDiff[i * 3];

        vertices[i * 3 + 1] = dataMean[i * 3 + 1] + dataDiff[i * 3 + 1];
        vertices[i * 3 + 2] = dataMean[i * 3 + 2] + dataDiff[i * 3 + 2];

        if (transformed) {
            QVector3D transformedVertex = (translationMatrix * rotationMatrix * QVector4D(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2], 1)).toVector3DAffine();
            vertices[i * 3 + 0] = transformedVertex.x();
            vertices[i * 3 + 1] = transformedVertex.y();
            vertices[i * 3 + 2] = transformedVertex.z();
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    vboVertices.release();

    delete[] dataDiff;

    return size;
}

/**
 * @brief Returns recomputed vertices
 * @param[in] transformed Flag for getting transformed vertices
 * @return Recomputed vertices
 */
QVector<QVector3D> MainRenderer::getRecomputedVertices(bool transformed)
{
    float *buffer;

    if (!mesh) {
        qCritical() << "MainRenderer::getRecomputedVertices error: null Mesh";
        return QVector<QVector3D>();
    }

    getRecomputedVertices(buffer, transformed);

    QVector<QVector3D> result(mesh->getNumberOfVertices());

    for(int i = 0; i < result.size(); i++) {
        result[i].setX(buffer[i * 3 + 0]);
        result[i].setY(buffer[i * 3 + 1]);
        result[i].setZ(buffer[i * 3 + 2]);
    }

    delete[] buffer;
    return result;
}

/**
 * @brief Checks whether every vertex is in rectangle.
 * @param[in] vertices Array with vertices (optional)
 * @param[in] numberOfVertices Number of vertices (optional)
 * @param[in] crop Cropping rectangle (optional).
 * @return Vector with boolenan values for every vertex
 *
 * If cropping rectangle is not defined, perspective corners are used.
 */
QVector<bool> MainRenderer::getVerticesMask(float *vertices, int numberOfVertices, const QRectF &cropRectangle)
{
    if (vertices == 0) {
        if (!mesh) {
            qCritical() << "MainRenderer::getMask error: null Mesh";
            return QVector<bool>();
        }
        numberOfVertices = mesh->getNumberOfVertices();
        getRecomputedVertices(vertices);
    }

    // Create output
    QVector<bool> result(numberOfVertices, false);

    // Transformations
    QMatrix4x4 p = perspectiveMatrix * cameraMatrix;
    QMatrix4x4 t = p * translationMatrix * rotationMatrix;
    QMatrix4x4 corners = getPerspective().getCorners();

    QVector4D upperLeft = (p * corners).column(0);
    upperLeft = upperLeft / upperLeft[3];

    QVector4D bottomRight = (p * corners).column(3);
    bottomRight = bottomRight / bottomRight[3];

    // Clipping rect
    QRectF rect(upperLeft.toPointF(), bottomRight.toPointF());
    if (cropRectangle != QRectF())
        rect = cropRectangle;

    // For every vertex
    for (int i = 0; i < numberOfVertices; i++) {
        QVector4D vertex = t * QVector4D(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2], 1);
        vertex = vertex / vertex[3];
        if (rect.contains(vertex.toPointF()))
            result[i] = true;
    }

    return result;
}

/**
 * @brief Exports recomputed STL polygonal model (can be transformed)
 * @param[in] filename STL filename to write
 * @param[in] transformed Transformed flag (optional)
 * @param[in] mask Mask for selected vertices (optional)
 */
void MainRenderer::exportSTL(const QString &filename, bool transformed, QVector<bool> mask)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);

        out << "solid renderer\n";

        QVector<QVector3D> vertices = getRecomputedVertices(transformed);
        QVector<QVector<unsigned int>> triangles = mesh->getTriangles2DVector();
        //QVector<bool> msk = getMask();

//        if(!xMirroringEnabled)
//        {

            for(int i = 0; i < triangles.size(); i++)
            {
                unsigned int tmp = triangles[i][0];
                triangles[i][0]  = triangles[i][1];
                triangles[i][1]  = tmp;
            }
//        }

        if (mask.size() == 0)
            mask = QVector<bool>(vertices.size(), true);

        for (int row = 0; row < triangles.size(); row++) {
            if (!mask.at(triangles.at(row).at(0)) || !mask.at(triangles.at(row).at(1)) || !mask.at(triangles.at(row).at(2)))
                continue;

            // Calculate face normal
            QVector3D a = vertices.at(triangles.at(row).at(0));
            QVector3D b = vertices.at(triangles.at(row).at(1));
            QVector3D c = vertices.at(triangles.at(row).at(2));

            QVector3D normal = QVector3D::crossProduct(c - a, b - a).normalized();

            out << "  facet normal ";

            out << QString::number(normal.x(), 'e', 6) << " ";
            out << QString::number(normal.y(), 'e', 6) << " ";
            out << QString::number(normal.z(), 'e', 6) << "\n";

            out << "    outer loop\n";

            out << "      vertex  " << QString::number(a.x(), 'e', 6) << " " << QString::number(a.y(), 'e', 6) << " " << QString::number(a.z(), 'e', 6) << "\n";
            out << "      vertex  " << QString::number(b.x(), 'e', 6) << " " << QString::number(b.y(), 'e', 6) << " " << QString::number(b.z(), 'e', 6) << "\n";
            out << "      vertex  " << QString::number(c.x(), 'e', 6) << " " << QString::number(c.y(), 'e', 6) << " " << QString::number(c.z(), 'e', 6) << "\n";

            out << "\n";
            out << "    endloop\n";
            out << "  endfacet\n";
        }

        out << "endsolid renderer\n";

        file.close();
        qDebug() << "STL model was saved to file";
    } else {
        // @todo TODO throw something?
        qCritical() << "Error opening STL for writting:" << filename;
    }
}

/**
 * @brief Saves rendered image into png file
 * @param[in] filePath Filename of output png image (filepath)
 */
void MainRenderer::saveRenderedImage(const QString &filePath)
{
    QPixmap::fromImage(getRenderedImage()).save(filePath, "png", 100);
}

/**
 * @brief Returns rendered image
 * @return Rendered image
 */
QImage MainRenderer::getRenderedImage()
{
    checkInitAndMakeCurrentContext();

    //debugTexture(toOutput);

    QImage image(getCropWidth(), getCropHeight(), QImage::Format_RGB888);

    glBindTexture(GL_TEXTURE_2D, toOutput);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
    glBindTexture(GL_TEXTURE_2D, 0);

    return image.mirrored();
}

/**
 * @brief Returns rendered red channel
 * @param[out] data Float 1D array
 *
 * Output array is allocated in this function.
 */
void MainRenderer::getRenderedRedChannel(float *&data)
{
    checkInitAndMakeCurrentContext();

    data = new float [getCropWidth() * getCropHeight()]();
    glBindTexture(GL_TEXTURE_2D, toOutput);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief Returns current silhouettes image
 * @param[out] image Output array with silhouettes image
 *
 * Output array is allocated in this function
 */
void MainRenderer::getCurrentSilhouettesImage(float *&image)
{
    checkInitAndMakeCurrentContext();

    image = new float [getRenderWidth() * getRenderHeight() * 4]();
    glBindTexture(GL_TEXTURE_2D, toSilhouettes);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief Returns output texture id
 * @return Output texture id
 *
 * For work with shared contexts
 */
GLuint MainRenderer::getOutputTextureId() const
{
    return toOutput;
}

/**
 * @brief Sets intensity for density rendering
 * @param[in] value Intesity value from 0.0 to 1.0
 */
void MainRenderer::setIntensity(double value)
{
    checkInitAndMakeCurrentContext();

    intensity = value;
    if (intensity < 0.0f) intensity = 0.0f;
    if (intensity > 1.0f) intensity = 1.0f;
    postprocessing->program->bind();
    postprocessing->program->setUniformValue(postprocessing->uIntensity, (float) (value / 5000.0f));
    postprocessing->program->release();
}

/**
 * @brief Sets line width for silhouettes rendering
 * @param[in] value Line width from 1 to 10
 */
void MainRenderer::setLineWidth(double value)
{
    checkInitAndMakeCurrentContext();

    lineWidth = value;
    if (lineWidth < 1) lineWidth = 1;
    postprocessing->program->bind();
    postprocessing->program->setUniformValue(postprocessing->uLineWidth, (int) lineWidth);
    postprocessing->program->release();
}

/**
 * @brief Sets special param for density rendering - unused
 * @param[in] value Some value
 */
void MainRenderer::setParam(double value)
{
    checkInitAndMakeCurrentContext();

    // @todo TODO - delete in future
    param = value;
    density->program->bind();
    density->program->setUniformValue(density->uParam, (float) param);
    density->program->release();
}

/**
 * @brief Returns intenstity for density rendering
 * @return Intenstity value
 */
double MainRenderer::getIntensity() const
{
    return intensity;
}

/**
 * @brief Returns line width for silhouettes rendering
 * @return Line width
 */
double MainRenderer::getLineWidth() const
{
    return lineWidth;
}

/**
 * @brief Return special param value
 * @return Value of special param for density rendering
 */
double MainRenderer::getParam() const
{
    return param;
}

/**
 * @brief Sets render width
 * @param[in] renderWidth Render width
 *
 * Sets cropping to same size.
 */
void MainRenderer::setRenderWidth(GLuint renderWidth)
{
    checkInitAndMakeCurrentContext();

    if (renderWidth < 1 || renderWidth > maxTextureOrRenderSize[0]) {
        qCritical("Wrong render width");
        Q_ASSERT_X(false, "OpenGL error", "Wrong render width");
        exit(1);
    }
    this->renderWidth = renderWidth;
    this->cropX = 0;
    this->cropWidth = renderWidth;
    ratio = (float) getRenderWidth() / (float) getRenderHeight();
    step.setX(1.0f / (float) getRenderWidth());
    step.setY(1.0f / (float) getRenderHeight());
    sizeChanged = true;
}

/**
 * @brief Sets render height
 * @param[in] renderHeight Render height
 *
 * Sets cropping to same size.
 */
void MainRenderer::setRenderHeight(GLuint renderHeight)
{
    checkInitAndMakeCurrentContext();

    if (renderHeight < 1 || renderHeight > maxTextureOrRenderSize[1]) {
        qCritical("Wrong render height");
        Q_ASSERT_X(false, "OpenGL error", "Wrong render height");
        exit(1);
    }
    this->renderHeight = renderHeight;
    this->cropY = 0;
    this->cropHeight = renderHeight;
    ratio = (float) getRenderWidth() / (float) getRenderHeight();
    step.setX(1.0f / (float) getRenderWidth());
    step.setY(1.0f / (float) getRenderHeight());
    sizeChanged = true;
}

/**
 * @brief Sets render size
 * @param[in] renderWidth Render width
 * @param[in] renderHeight Render height
 *
 * Sets cropping to same size.
 */
void MainRenderer::setRenderSize(GLuint renderWidth, GLuint renderHeight)
{
    setRenderWidth(renderWidth);
    setRenderHeight(renderHeight);
}

/**
 * @brief Returns render width
 * @return Render width
 */
GLuint MainRenderer::getRenderWidth() const
{
    return renderWidth;
}

/**
 * @brief Returns render height
 * @return Render height
 */
GLuint MainRenderer::getRenderHeight() const
{
    return renderHeight;
}

/**
 * @brief Sets cropping window
 * @param[in] cropX X position - top left
 * @param[in] cropY Y position - top left
 * @param[in] cropWidth Cropping width
 * @param[in] cropHeight Cropping height
 */
void MainRenderer::setCropWindow(GLuint cropX, GLuint cropY, GLuint cropWidth, GLuint cropHeight)
{
    checkInitAndMakeCurrentContext();

    GLuint newCropY = renderHeight - (cropHeight + cropY);

    if (cropX < 0 || cropX >= renderWidth - 1) {
        qCritical("Wrong crop x position");
        return;
    }

    if (cropWidth < 1 || cropWidth > maxTextureOrRenderSize[0] || cropX + cropWidth > renderWidth) {
        qCritical("Wrong crop width");
        return;
    }

    if (newCropY < 0 || newCropY >= renderHeight - 1) {
        qCritical("Wrong crop y position");
        return;
    }

    if (cropHeight < 1 || cropHeight > maxTextureOrRenderSize[1] || newCropY + cropHeight > renderHeight) {
        qCritical("Wrong crop height");
        return;
    }

    this->cropX = cropX;
    this->cropWidth = cropWidth;
    this->cropY = newCropY;
    this->cropHeight = cropHeight;
    sizeChanged = true;
}

/**
 * @brief Sets cropping window
 * @param[in] rectangle Cropping rectangle
 */
void MainRenderer::setCropWindow(const QRect &rectangle)
{
    setCropWindow(rectangle.x(), rectangle.y(), rectangle.width(), rectangle.height());
}

/**
 * @brief Returns cropping x position
 * @return Cropping x position
 */
GLuint MainRenderer::getCropX() const
{
    return cropX;
}

/**
 * @brief Returns cropping y position
 * @return Cropping y position
 */
GLuint MainRenderer::getCropY() const
{
    return cropY;
}

/**
 * @brief Returns cropping width
 * @return Cropping width
 */
GLuint MainRenderer::getCropWidth() const
{
    return cropWidth;
}

/**
 * @brief Returns cropping height
 * @return Cropping height
 */
GLuint MainRenderer::getCropHeight() const
{
    return cropHeight;
}

/**
 * @brief Returns cropping Rectangle
 * @return Cropping Rectangle
 */
QRect MainRenderer::getCropRectangle() const
{
    return QRect(cropX, cropY, cropWidth, cropHeight);
}

/**
 * @brief Enables or disables density rendering
 * @param[in] value Boolean flag
 */
void MainRenderer::enableDensity(bool value)
{
    checkInitAndMakeCurrentContext();

    densityEnabled = value;

    postprocessing->program->bind();
    postprocessing->program->setUniformValue(postprocessing->uDensityEnabled, densityEnabled);
    postprocessing->program->release();
}

/**
 * @brief Enables or disables density rendering
 * @param[in] value Boolean flag
 */
void MainRenderer::enableSilhouettes(bool value)
{
    checkInitAndMakeCurrentContext();

    silhouettesEnabled = value;

    postprocessing->program->bind();
    postprocessing->program->setUniformValue(postprocessing->uSilhouettesEnabled, silhouettesEnabled);
    postprocessing->program->release();
}

/**
 * @brief Enables or disables pyramid rendering
 * @param[in] value Boolean flag
 */
void MainRenderer::enablePyramid(bool value)
{
    checkInitAndMakeCurrentContext();

    pyramidEnabled = value;

    postprocessing->program->bind();
    postprocessing->program->setUniformValue(postprocessing->uPyramidEnabled, pyramidEnabled);
    postprocessing->program->release();
}

/**
 * @brief Enables or disables polygonal rendering
 * @param[in] value Boolean flag
 */
void MainRenderer::enablePolygonal(bool value)
{
    checkInitAndMakeCurrentContext();

    polygonalEnabled = value;

    postprocessing->program->bind();
    postprocessing->program->setUniformValue(postprocessing->uPolygonalEnabled, polygonalEnabled);
    postprocessing->program->release();
}

/**
 * @brief Enables or disables x-axis mirroring
 * @param[in] value Boolean flag
 */
void MainRenderer::enableXMirroring(bool value)
{
    checkInitAndMakeCurrentContext();

    xMirroringEnabled = value;

    density->program->bind();
    density->program->setUniformValue(density->uXMirror, xMirroringEnabled);
    density->program->release();

    silhouettes->program->bind();
    silhouettes->program->setUniformValue(silhouettes->uXMirror, xMirroringEnabled);
    silhouettes->program->release();

    polygonal->program->bind();
    polygonal->program->setUniformValue(polygonal->uXMirror, xMirroringEnabled);
    polygonal->program->release();
}

/**
 * @brief Enables or disables polygonal lighting
 * @param[in] value Boolean flag
 */
void MainRenderer::enablePolygonalLighting(bool value)
{
    checkInitAndMakeCurrentContext();

    polygonalLightingEnabled = value;

    polygonal->program->bind();
    polygonal->program->setUniformValue(polygonal->uLightingEnabled, polygonalLightingEnabled);
    polygonal->program->release();
}

/**
 * @brief Is density rendering enabled?
 * @return True if density rendering is enabled
 */
bool MainRenderer::isDensityEnabled() const
{
    return densityEnabled;
}

/**
 * @brief Is silhouettes rendering enabled?
 * @return True if silhouettes rendering is enabled
 */
bool MainRenderer::isSilhouettesEnabled() const
{
    return silhouettesEnabled;
}

/**
 * @brief Is pyramid rendering enabled?
 * @return True if density pyramid is enabled
 */
bool MainRenderer::isPyramidEnabled() const
{
    return pyramidEnabled;
}

/**
 * @brief Is polygonal rendering enabled?
 * @return True if density polygonal is enabled
 */
bool MainRenderer::isPolygonalEnabled() const
{
    return polygonalEnabled;
}

/**
 * @brief Is x-axis mirroring enabled?
 * @return True if x-axis mirroring is enabled
 */
bool MainRenderer::isXMirroringEnabled() const
{
    return xMirroringEnabled;
}

/**
 * @brief Is polygonal lighting enabled?
 * @return True if polygonal lighting is enabled
 */
bool MainRenderer::isLightingEnabled() const
{
    return polygonalLightingEnabled;
}

/**
 * @brief Sets flag for sharing transformations between shared contexts
 * @param[in] value Boolean flag
 */
void MainRenderer::setFlagShareTransformations(bool value)
{
    flagShareTransformations = value;
}

/**
 * @brief Sets translation of model in scene
 * @param[in] x X position
 * @param[in] y Y position
 * @param[in] z Z position
 */
void MainRenderer::setTranslation(float x, float y, float z)
{
    setTranslation(QVector3D(x, y, z));
}

/**
 * @brief Sets translation of model in scene
 * @param[in] position 3D vector with position
 */
void MainRenderer::setTranslation(QVector3D position)
{
    translationMatrix.setToIdentity();
    translationMatrix.translate(position.x(), position.y(), position.z());

    if (flagShareTransformations) {
        if (!hasSharedContext()) {
            QList<OpenGLWrapper *> childOpenGLWrappers = getChildOpenGLWrappers();
            for (int i = 0; i < childOpenGLWrappers.size(); i++) {
                OpenGLWrapper *renderSurface = childOpenGLWrappers.at(i);
                if (dynamic_cast<MainRenderer *>(renderSurface)) {
                    ((MainRenderer *) renderSurface)->translationMatrix = translationMatrix;
                }
            }
        } else {
            ((MainRenderer *) getParentOpenGLWrapper())->setTranslation(position);
        }
    }
}

/**
 * @brief Returns translation
 * @param[out] x X position
 * @param[out] y Y position
 * @param[out] z Z position
 */
void MainRenderer::getTranslation(float &x, float &y, float &z)
{
    QVector3D translation = getTranslation();
    x = translation.x();
    y = translation.y();
    z = translation.z();
}

/**
 * @brief Returns translation
 * @return 3D vector with position
 */
QVector3D MainRenderer::getTranslation()
{
    return QVector3D(translationMatrix.column(3).x(), translationMatrix.column(3).y(), translationMatrix.column(3).z());
}

/**
 * @brief Translates model in scene
 * @param[in] x X position
 * @param[in] y Y position
 * @param[in] z Z position
 */
void MainRenderer::translate(float x, float y, float z)
{
    translate(QVector3D(x, y, z));
}

/**
 * @brief Translates model in scene
 * @param[in] position 3D vector with position
 */
void MainRenderer::translate(QVector3D position)
{
    translationMatrix.translate(position.x(), position.y(), position.z());

    if (flagShareTransformations) {
        if (!hasSharedContext()) {
            QList<OpenGLWrapper *> childOpenGLWrappers = getChildOpenGLWrappers();
            for (int i = 0; i < childOpenGLWrappers.size(); i++) {
                OpenGLWrapper *renderSurface = childOpenGLWrappers.at(i);
                if (dynamic_cast<MainRenderer *>(renderSurface)) {
                    ((MainRenderer *) renderSurface)->translationMatrix = translationMatrix;
                }
            }
        } else {
            ((MainRenderer *) getParentOpenGLWrapper())->translate(position);
        }
    }
}

/**
 * @brief Sets rotation of model in scene
 * @param[in] xAngle X angle
 * @param[in] yAngle Y angle
 * @param[in] zAngle Z angle
 */
void MainRenderer::setRotation(float xAngle, float yAngle, float zAngle)
{
    setRotation(QVector3D(xAngle, yAngle, zAngle));
}

/**
 * @brief Sets rotation of model in scene
 * @param[in] rotation 3D vector with rotation
 */
void MainRenderer::setRotation(QVector3D rotation)
{
    rotationMatrix.setToIdentity();
    angles = rotation;
    if (angles.x() < 0)
        angles.setX(360 + angles.x());
    if (angles.y() < 0)
        angles.setY(360 + angles.y());
    if (angles.z() < 0)
        angles.setZ(360 + angles.z());
    angles.setX(fmod(angles.x(), 360));
    angles.setY(fmod(angles.y(), 360));
    angles.setZ(fmod(angles.z(), 360));

    /*float sX = qSin(qDegreesToRadians(angles.x()));
    float sY = qSin(qDegreesToRadians(angles.y()));
    float sZ = qSin(qDegreesToRadians(angles.z()));
    float cX = qCos(qDegreesToRadians(angles.x()));
    float cY = qCos(qDegreesToRadians(angles.y()));
    float cZ = qCos(qDegreesToRadians(angles.z()));

    rotationMatrix(0, 0) = cY * cZ;
    rotationMatrix(0, 1) = -cY * sZ;
    rotationMatrix(0, 2) = sY;

    rotationMatrix(1, 0) = cZ * sX * sY + cX * sZ;
    rotationMatrix(1, 1) = cX * cZ-sX * sY * sZ;
    rotationMatrix(1, 2) = -cY * sX;

    rotationMatrix(2, 0) = -cX * cZ * sY + sX * sZ;
    rotationMatrix(2, 1) = cZ * sX + cX * sY * sZ;
    rotationMatrix(2, 2) = cX * cY;*/

    rotationMatrix.rotate(angles.z(), 0, 0, 1);
    rotationMatrix.rotate(angles.y(), 0, 1, 0);
    rotationMatrix.rotate(angles.x(), 1, 0, 0);

    if (flagShareTransformations) {
        if (!hasSharedContext()) {
            QList<OpenGLWrapper *> childOpenGLWrappers = getChildOpenGLWrappers();
            for (int i = 0; i < childOpenGLWrappers.size(); i++) {
                OpenGLWrapper *renderSurface = childOpenGLWrappers.at(i);
                if (dynamic_cast<MainRenderer *>(renderSurface)) {
                    ((MainRenderer *) renderSurface)->rotationMatrix = rotationMatrix;
                    ((MainRenderer *) renderSurface)->angles = angles;
                }
            }
        } else {
            ((MainRenderer *) getParentOpenGLWrapper())->setRotation(angles);
        }
    }
}

/**
 * @brief Sets rotation matrix
 * @param[in] rotationMatrix Rotation matrix
 */
void MainRenderer::setRotationMatrix(QMatrix4x4 rotationMatrix)
{
    this->rotationMatrix = rotationMatrix;
    //QVector3D angles = getAngles(rotationMatrix);
    //setRotation(angles);

    if (flagShareTransformations) {
        if (!hasSharedContext()) {
            QList<OpenGLWrapper *> childOpenGLWrappers = getChildOpenGLWrappers();
            for (int i = 0; i < childOpenGLWrappers.size(); i++) {
                OpenGLWrapper *renderSurface = childOpenGLWrappers.at(i);
                if (dynamic_cast<MainRenderer *>(renderSurface)) {
                    ((MainRenderer *) renderSurface)->rotationMatrix = rotationMatrix;
                }
            }
        } else {
            ((MainRenderer *) getParentOpenGLWrapper())->setRotationMatrix(rotationMatrix);
        }
    }
}

/**
 * @brief Returns rotation
 * @param[out] xAngle X angle
 * @param[out] yAngle Y angle
 * @param[out] zAngle Z angle
 */
void MainRenderer::getRotation(float &xAngle, float &yAngle, float &zAngle)
{
    xAngle = angles.x();
    yAngle = angles.y();
    zAngle = angles.z();
}

/**
 * @brief Returns rotation
 * @return 3D vector with rotation
 */
QVector3D MainRenderer::getRotation()
{
    return angles;
}

/**
 * @brief Returns rotation matrix
 * @return Rotation matrix
 */
QMatrix4x4 MainRenderer::getRotationMatrix()
{
    return rotationMatrix;
}

/**
 * @brief Rotates model in scene
 * @param[in] xAngle X angle
 * @param[in] yAngle Y angle
 * @param[in] zAngle Z angle
 */
void MainRenderer::rotate(float xAngle, float yAngle, float zAngle)
{
    rotate(QVector3D(xAngle, yAngle, zAngle));
}

/**
 * @brief Rotates model in scene
 * @param[in] rotation 3D vector with rotation
 */
void MainRenderer::rotate(QVector3D rotation)
{
    setRotation(angles + rotation);
}

/**
 * @brief Sets camera by corners ane eye positions
 * @param[in] leftTop Left top corner of pyramid base
 * @param[in] leftBottom Left bottom corner of pyramid base
 * @param[in] rightTop Right top corner of pyramid base
 * @param[in] rightBottom Right bottom corner of pyramid base
 * @param[in] eye Position of eye - the top of the pyramid
 */
void MainRenderer::setCamera(QVector3D leftTop, QVector3D leftBottom, QVector3D rightTop, QVector3D rightBottom, QVector3D eye)
{
    setCamera(SSIMRenderer::Pyramid(leftTop, leftBottom, rightTop, rightBottom, eye));
}

/**
 * @brief Sets camera by pyramid object
 * @param[in] camera Pyramid to set
 */
void MainRenderer::setCamera(SSIMRenderer::Pyramid camera)
{
    setCamera(camera.getEye(), camera.getCenter());
}

/**
 * @brief Sets camera by eye and center position
 * @param[in] eye Eye position
 * @param[in] center Center position
 */
void MainRenderer::setCamera(QVector3D eye, QVector3D center)
{
    this->center = center;
    this->eye = eye;
    if (this->eye == this->center)
        setCamera(this->eye + (perspective.getViewDir() * perspective.getDistance()), this->center);
    cameraMatrix.setToIdentity();
    cameraMatrix.lookAt(getEye(), getCenter(), perspective.getUpDir());
}

/**
 * @brief Move camera with given distance and center position
 * @param[in] distance Distance form center
 * @param[in] center Center position
 */
void MainRenderer::moveCamera(float distance, QVector3D center)
{
    QVector3D eyeTranslated;
    QVector3D centerTranslated;
    QVector3D centerDir = (getEye() - center).normalized();
    eyeTranslated = getEye() + centerDir * distance;
    centerTranslated = getCenter() + centerDir * distance;
    setCamera(eyeTranslated, centerTranslated);
}

/**
 * @brief Rotates camera arround center with x and y angles
 * @param[in] xAngle X angle
 * @param[in] yAngle Y angle
 * @param[in] center Center position
 */
void MainRenderer::rotateCamera(float xAngle, float yAngle, QVector3D center)
{
    QVector3D eyeRotated;
    QVector3D centerRotated;
    QMatrix4x4 rotation;
    rotation.rotate(xAngle, QVector3D::crossProduct(perspective.getUpDir(), getViewDir()));
    rotation.rotate(yAngle, perspective.getUpDir());

    if (qAbs(QVector3D::dotProduct(rotation * getViewDir(), perspective.getUpDir())) > 0.99f) {
        rotation.setToIdentity();
        rotation.rotate(yAngle, perspective.getUpDir());
    }

    QVector3D eyeVector = (getEye() - center);
    QVector3D centerVector = (getCenter() - center);
    eyeRotated = rotation * eyeVector;
    centerRotated = rotation * centerVector;
    eyeRotated += center;
    centerRotated += center;
    setCamera(eyeRotated, centerRotated);
}

/**
 * @brief Returns eye position
 * @return Eye position
 */
QVector3D MainRenderer::getEye()
{
    if (eye == center)
        setCamera(eye + (perspective.getViewDir() * perspective.getDistance()), center);
    return eye;
}

/**
 * @brief Returns center position
 * @return Center position
 */
QVector3D MainRenderer::getCenter()
{
    if (eye == center)
        setCamera(eye + (perspective.getViewDir() * perspective.getDistance()), center);
    return center;
}

/**
 * @brief Returns middle position
 * @return Middle position
 */
QVector3D MainRenderer::getMiddle()
{
    return perspective.getMiddle();
}

/**
 * @brief Returns view dir
 * @return View dir
 */
QVector3D MainRenderer::getViewDir()
{
    return (getEye() - getCenter()).normalized();
}

/**
 * @brief Sets eye position
 * @param[in] Eye position
 */
void MainRenderer::setEye(const QVector3D &value)
{
    setCamera(value, getCenter());
}

/**
 * @brief Sets center position
 * @param[in] value Center position
 */
void MainRenderer::setCenter(const QVector3D &value)
{
    setCamera(getEye(), value);
}

/**
 * @brief Sets perspective by corners ane eye positions
 * @param[in] leftTop Left top corner of pyramid base
 * @param[in] leftBottom Left bottom corner of pyramid base
 * @param[in] rightTop Right top corner of pyramid base
 * @param[in] rightBottom Right bottom corner of pyramid base
 * @param[in] eye Position of eye - the top of the pyramid
 */
void MainRenderer::setPerspective(QVector3D leftTop, QVector3D leftBottom, QVector3D rightTop, QVector3D rightBottom, QVector3D eye)
{
    perspective = SSIMRenderer::Pyramid(leftTop, leftBottom, rightTop, rightBottom, eye);
    prepareTransformation();
    setCamera(perspective);
}

/**
 * @brief Sets perspective camera by pyramid object
 * @param[in] camera Pyramid to set
 */
void MainRenderer::setPerspective(SSIMRenderer::Pyramid perspective)
{
    this->perspective = perspective;
    points.append(perspective.getCenter());
    prepareTransformation();
    setCamera(perspective);
}

/**
 * @brief Returns perspective
 * @return Setted perspective
 */
SSIMRenderer::Pyramid MainRenderer::getPerspective()
{
    return perspective;
}

/**
 * @brief Adds control point for distances measuring
 * @param[in] value Point position
 */
void MainRenderer::addPoint(const QVector3D &value)
{
    points.append(value);
}

/**
 * @brief Adds control points for distances measuring
 * @param[in] points 2D vector of control points
 */
void MainRenderer::setPoints(QVector<QVector3D> points)
{
    this->points = points;
}

/**
 * @brief Returns controls points
 * @return 2D vector with control points
 */
QVector<QVector3D> MainRenderer::getPoints()
{
    return points;
}

/*
/**
 * @brief MainRenderer::getMinimalDistancesToPoints
 * @return
 *
 * @todo TODO nepouzito, predelat, upravit, zprehlednit
 */
/*QVector<float> MainRenderer::getMinimalDistancesToPoints(void (*f)(float *d, unsigned char *u, int  w, int h))
{
    // @todo TODO comments!
    QVector<float> distances(points.size(), -1);

    if (distances.size() > 0) {
        //QMatrix4x4 r = getRotationMatrix().inverted();
        //float tx, ty, tz;
        //getTranslation(tx, ty, tz);
        //QVector3D t(tx, ty, tz);

        float *silhouettesImage;
        getCurrentSilhouettesImage(silhouettesImage);
        unsigned char *mask = NULL;
        if (f != NULL) {
            mask = new unsigned char[getRenderWidth() * getRenderHeight()]();
            f(silhouettesImage, mask, getRenderWidth(), getRenderHeight());
        }

        QVector<QVector3D> lines;

        for (int i = 0; i < points.size(); i++) {
            lines.append((points.at(i) - perspective.getEye()).normalized());
        }

        for (unsigned int i = 0; i < getRenderWidth() * getRenderHeight(); i++) {
            if (silhouettesImage[4 * i + 3] != 0 && (mask == NULL || mask[i] != 0)) {
                for (int p = 0; p < points.size(); p++) {
                    QVector3D silhouettePoint = QVector3D(silhouettesImage[4 * i + 0], silhouettesImage[4 * i + 1], silhouettesImage[4 * i + 2]);
                    silhouettePoint = translationMatrix * rotationMatrix * silhouettePoint;

                    float distance = silhouettePoint.distanceToLine(points.at(p), lines.at(p));
                    if (distances[p] != -1) {
                        if (distances[p] > distance) {
                            distances[p] = distance;
                        }
                    } else {
                        distances[p] = distance;
                    }
                }
            }

        }
        //qDebug() << points;
        //qDebug() << distances;

        delete[] silhouettesImage;
        if (f != NULL)
            delete[] mask;
    }

    return distances;
}
/*
QVector<float> MainRenderer::getMinimalDistancesToPoints()
{
    QVector<float> distances(points.size(), -1);

    if (distances.size() > 0) {

        //QMatrix4x4 r = getRotationMatrix().inverted();
        //float tx, ty, tz;
        //getTranslation(tx, ty, tz);
        //QVector3D t(tx, ty, tz);

        float *silhouettesImage;
        getCurrentSilhouettesImage(silhouettesImage);

        QVector<QVector3D> lines;

        for (int i = 0; i < points.size(); i++) {
            lines.append((points.at(i) - perspective.getEye()).normalized());
        }

        for (unsigned int i = 0; i < getRenderWidth() * getRenderHeight(); i++) {
            if (silhouettesImage[4 * i + 3] != 0) {
                for (int p = 0; p < points.size(); p++) {
                    QVector3D silhouettePoint = QVector3D(silhouettesImage[4 * i + 0], silhouettesImage[4 * i + 1], silhouettesImage[4 * i + 2]);
                    silhouettePoint = translationMatrix * rotationMatrix * silhouettePoint;

                    float distance = silhouettePoint.distanceToLine(points.at(p), lines.at(p));
                    if (distances[p] != -1) {
                        if (distances[p] > distance) {
                            distances[p] = distance;
                        }
                    } else {
                        distances[p] = distance;
                    }
                }
            }

        }
        //qDebug() << points;
        //qDebug() << distances;

        delete[] silhouettesImage;
    }

    return distances;
}      */

/**
 * @brief Clear control points
 */
void MainRenderer::clearPoints()
{
    points.clear();
}

/**
 * @brief Returns pointer to statistical data object
 * @return The pointer to statistical data object
 */
StatisticalData *MainRenderer::getStatisticalData() const
{
    if (!statisticalData) {
        qCritical() << "MainRenderer::getStatisticalData error: null StatisticalData";
        return 0;
    }

    return statisticalData;
}

/**
 * @brief Returns pointer to mesh object
 * @return The pointer to mesh object
 */
Mesh *MainRenderer::getMesh() const
{
    if (!mesh) {
        qCritical() << "MainRenderer::getMesh error: null Mesh";
        return 0;
    }

    return mesh;
}

/**
 * @brief Main initialze function
 *
 * Initializes OpenGL (shared) resources and others.
 */
void MainRenderer::initialize()
{
    // Important for resources in library
    Q_INIT_RESOURCE(shaders);

    // Get max texture and viewport dims
    GLint dimsV[2], dimT;
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &dimsV[0]);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &dimT);
    maxTextureOrRenderSize[0] = (GLuint) qMin(dimsV[0], dimT);
    maxTextureOrRenderSize[1] = (GLuint) qMin(dimsV[1], dimT);
    cWidth = maxTextureOrRenderSize[0];

    // Set helper variables
    positionDiffLengthLog2 = (GLuint(double(qLn(maxTextureOrRenderSize[0]) / qLn(2))));
    positionDiffLengthMinus1 = maxTextureOrRenderSize[0] - 1;

    // Init first render size
    setRenderHeight(renderHeight);
    setRenderWidth(renderWidth);

    // If current surface uses other main context
    if (hasSharedContext()) {

        // Get structures from parent surface
        MainRenderer* parentOpenGLWrapper = (MainRenderer *) getParentOpenGLWrapper();

        density = parentOpenGLWrapper->density;
        silhouettes = parentOpenGLWrapper->silhouettes;
        computing = parentOpenGLWrapper->computing;
        pyramid = parentOpenGLWrapper->pyramid;
        polygonal = parentOpenGLWrapper->polygonal;
        postprocessing = parentOpenGLWrapper->postprocessing;

        // Texture Objects
        toCompCoeffs = parentOpenGLWrapper->toCompCoeffs;
        toCompVertices = parentOpenGLWrapper->toCompVertices;
        toBerncoeffs = parentOpenGLWrapper->toBerncoeffs;
        toT = parentOpenGLWrapper->toT;
        toPcs = parentOpenGLWrapper->toPcs;

        // Vertex Buffer Objects
        vboVertices = parentOpenGLWrapper->vboVertices;

        vboNormals = parentOpenGLWrapper->vboNormals;
        //vboComputeIndicesX = parentOpenGLWrapper->vboComputeIndicesX;
        //vboComputeIndicesY = parentOpenGLWrapper->vboComputeIndicesY;

        // Element Buffer Objects
        iboElementsTetrahedra = parentOpenGLWrapper->iboElementsTetrahedra;
        iboElementsTriangles = parentOpenGLWrapper->iboElementsTriangles;
        vboVerticesColors = parentOpenGLWrapper->vboVerticesColors;
        iboElementsTrianglesAdjacency = parentOpenGLWrapper->iboElementsTrianglesAdjacency;

        // Texture Buffer Objects
        tboBerncoeffs = parentOpenGLWrapper->tboBerncoeffs;
        tboT = parentOpenGLWrapper->tboT;
        tboPcs = parentOpenGLWrapper->tboPcs;

        // Other shared data
        if (parentOpenGLWrapper->mesh && mesh == 0)
            mesh = parentOpenGLWrapper->mesh;

    } else { // Main context

        density = new Density();
        density->program = 0;
        density->fragmentShader = 0;

        silhouettes = new Silhouettes();
        silhouettes->program = 0;

        computing = new Computing();
        computing->program = 0;

        pyramid = new Pyramid();
        pyramid->program = 0;

        polygonal = new Polygonal();
        polygonal->program = 0;

        postprocessing = new Postprocessing();
        postprocessing->program = 0;

        // Load, create and link shaders
        // Main program for render geometry with density to texture
        density->program = new QOpenGLShaderProgram();
        addShader(density->program, QOpenGLShader::Vertex, ":/vsDensity");
        addShader(density->program, QOpenGLShader::Geometry, ":/gsDensity");
        density->fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
        addShaderFromSource(density->program, density->fragmentShader, fsGenerator.generateFragmentShaderSourceCode(0));
        linkProgram(density->program);

        // Program for render silhouettes
        silhouettes->program = new QOpenGLShaderProgram();
        addShader(silhouettes->program, QOpenGLShader::Vertex, ":/vsSilhouettes");
        addShader(silhouettes->program, QOpenGLShader::Geometry, ":/gsSilhouettes");
        addShader(silhouettes->program, QOpenGLShader::Fragment, ":/fsSilhouettes");
        linkProgram(silhouettes->program);

        // Program for computing coefficients and vertices
        computing->program = new QOpenGLShaderProgram();
        addShader(computing->program, QOpenGLShader::Vertex, ":/vsComputing");
        addShader(computing->program, QOpenGLShader::Fragment, ":/fsComputing");
        linkProgram(computing->program);

        // Program for rendering pyramid
        pyramid->program = new QOpenGLShaderProgram();
        addShader(pyramid->program, QOpenGLShader::Vertex, ":/vsPyramid");
        addShader(pyramid->program, QOpenGLShader::Fragment, ":/fsPyramid");
        linkProgram(pyramid->program);

        // Program for render polygonal model
        polygonal->program = new QOpenGLShaderProgram();
        addShader(polygonal->program, QOpenGLShader::Vertex, ":/vsPolygonal");
        //addShader(polygonal->program, QOpenGLShader::Geometry, ":/gsPolygonal");
        addShader(polygonal->program, QOpenGLShader::Fragment, ":/fsPolygonal");
        linkProgram(polygonal->program);

        // Program for render final texture with textures
        postprocessing->program = new QOpenGLShaderProgram();
        addShader(postprocessing->program, QOpenGLShader::Vertex, ":/vsPostprocessing");
        addShader(postprocessing->program, QOpenGLShader::Fragment, ":/fsPostprocessing");
        linkProgram(postprocessing->program);

        // Program for render final quad with output texture
        postprocessing->programSimple = new QOpenGLShaderProgram();
        addShader(postprocessing->programSimple, QOpenGLShader::Vertex, ":/vsPostprocessingSimple");
        addShader(postprocessing->programSimple, QOpenGLShader::Fragment, ":/fsPostprocessingSimple");
        linkProgram(postprocessing->programSimple);

        // Generate buffers
        // Element buffer for tetrahedra
        iboElementsTetrahedra = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        iboElementsTetrahedra.create();

        // Element buffer for triangles
        iboElementsTriangles = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        iboElementsTriangles.create();

        // Color buffer for triangles
        vboVerticesColors = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        vboVerticesColors.create();

        // Element buffer for adjacent triangles
        iboElementsTrianglesAdjacency = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        iboElementsTrianglesAdjacency.create();

        // Vertex buffer
        vboVertices = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        vboVertices.create();

        vboNormals = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        vboNormals.create();

        //vboComputeIndicesX = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        //vboComputeIndicesX.create();

        //vboComputeIndicesY = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        //vboComputeIndicesY.create();

        // Coefficients texture buffer
        glGenBuffers(1, &tboBerncoeffs);
        glBindBuffer(GL_TEXTURE_BUFFER, tboBerncoeffs);
        glBufferData(GL_TEXTURE_BUFFER, 0, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        // Texture for coefficients
        glGenTextures(1, &toBerncoeffs);
        glBindTexture(GL_TEXTURE_BUFFER, toBerncoeffs);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tboBerncoeffs);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        // Textures for computing - input
        glGenTextures(1, &toCompCoeffs);
        glBindTexture(GL_TEXTURE_2D, toCompCoeffs);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &toCompVertices);
        glBindTexture(GL_TEXTURE_2D, toCompVertices);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        // T matrix texture buffer
        glGenBuffers(1, &tboT);
        glBindBuffer(GL_TEXTURE_BUFFER, tboT);
        glBufferData(GL_TEXTURE_BUFFER, 0, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        // Texture for T matrix
        glGenTextures(1, &toT);
        glBindTexture(GL_TEXTURE_BUFFER, toT);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tboT);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        // Pcs matrix texture buffer
        glGenBuffers(1, &tboPcs);
        glBindBuffer(GL_TEXTURE_BUFFER, tboPcs);
        glBufferData(GL_TEXTURE_BUFFER, 0, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        // Texture for Pcs matrix
        glGenTextures(1, &toPcs);
        glBindTexture(GL_TEXTURE_BUFFER, toPcs);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tboPcs);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        // Init attribute and uniform variables
        getVariablesLocations();
    }

    // Textures for rendering
    generateScreenTexture(&toDensity);
    generateScreenTexture(&toSilhouettes);
    generateScreenTexture(&toPyramid);
    generateScreenTexture(&toPolygonal);

    // Final textures
    glGenTextures(1, &toOutput);
    glBindTexture(GL_TEXTURE_2D, toOutput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getCropWidth(), getCropHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Texture for pyramid image
    glGenTextures(1, &toPyramidImage);
    glBindTexture(GL_TEXTURE_2D, toPyramidImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getRenderWidth(), getRenderHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Vertex Array Object
    glGenVertexArrays(1, &vao);

    // Helper framebuffers
    glGenFramebuffers(1, &fbo);
    glGenFramebuffers(1, &fboOutput);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Render depth buffer
    glGenRenderbuffers(1, &rbo);

    // Render depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getRenderWidth(), getRenderHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Check framebuffer
    if (GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qCritical() << "OpenGL framebuffer error" << QString::number(err);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Framebuffer for computing
    glGenFramebuffers(1, &fboComputing);

    // Settings
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Blending
    //glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
    //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    //glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    if (!hasSharedContext())
        initUniformVariables();
}

/**
 * @brief Main render function
 */
void MainRenderer::render()
{
    prepareRendering();

    // Must be first!!! - depth culling
    if (pyramidEnabled) renderPyramid(getPerspective());
    if (densityEnabled) renderDensity();
    if (polygonalEnabled) renderPolygonal();
    if (silhouettesEnabled) renderSilhouettes();

    renderPostprocessing();
}

/**
 * @brief Renders density
 */
void MainRenderer::renderDensity()
{
    if (!mesh) {
        qWarning() << "MainRenderer::renderDensity warning: null Mesh";
        return;
    }

    if (mesh->getNumberOfTetrahedra() == 0) {
        qWarning() << "MainRenderer::renderDensity warning: Tetrahedral mesh is not available";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toDensity, 0);

    //glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
    glDepthMask(GL_FALSE);

    // Drawing
    density->program->bind();
    // Set matrix to shader
    density->program->setUniformValue(density->uMatrix, matrix);
    density->program->setUniformValue(density->uMatrixInv, matrix.inverted());

    glBindVertexArray(vao);

    iboElementsTetrahedra.bind();

    vboVertices.bind();
    glEnableVertexAttribArray(density->aPosition);
    glVertexAttribPointer(density->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    /*vboComputeIndicesX.bind();
    glEnableVertexAttribArray(density->aIndicesX);
    glVertexAttribIPointer(density->aIndicesX, 3, GL_UNSIGNED_INT, 0, 0);
    vboComputeIndicesY.bind();
    glEnableVertexAttribArray(density->aIndicesY);
    glVertexAttribIPointer(density->aIndicesY, 3, GL_UNSIGNED_INT, 0, 0);*/

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, toBerncoeffs);
    density->program->setUniformValue(density->uBernCoeffs, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, toCompCoeffs);
    density->program->setUniformValue(density->uBernCoeffsDiff, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, toCompVertices);
    density->program->setUniformValue(density->uPositionDiff, 2);

    glDrawElements(GL_LINES_ADJACENCY, mesh->getNumberOfTetrahedra() * 4, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    //glDisableVertexAttribArray(density->aIndicesY);
    //glDisableVertexAttribArray(density->aIndicesX);
    glDisableVertexAttribArray(density->aPosition);

    //vboComputeIndicesY.release();
    //vboComputeIndicesX.release();
    vboVertices.release();
    iboElementsTetrahedra.release();

    glBindVertexArray(0);

    density->program->release();

    //debugTexture("renderDensity");

    //glEnable(GL_MULTISAMPLE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Renders silhouettes
 */
void MainRenderer::renderSilhouettes()
{
    if (!mesh) {
        qWarning() << "MainRenderer::renderSilhouettes warning: null Mesh";
        return;
    }

    if (mesh->getNumberOfTrianglesAdjacency() == 0) {
        qWarning() << "MainRenderer::renderSilhouettes warning: Triangles adjacency are not available";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toSilhouettes, 0);

    //glDisable(GL_BLEND);
    //glEnable(GL_MULTISAMPLE);
    //glDepthMask(GL_FALSE);

    silhouettes->program->bind();
    // Set matrix to shader
    silhouettes->program->setUniformValue(silhouettes->uMatrix, matrix);

    glBindVertexArray(vao);

    iboElementsTrianglesAdjacency.bind();

    vboVertices.bind();
    glEnableVertexAttribArray(silhouettes->aPosition);
    glVertexAttribPointer(silhouettes->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    /*vboComputeIndicesX.bind();
    glEnableVertexAttribArray(silhouettes->aIndicesX);
    glVertexAttribIPointer(silhouettes->aIndicesX, 3, GL_UNSIGNED_INT, 0, 0);
    vboComputeIndicesY.bind();
    glEnableVertexAttribArray(silhouettes->aIndicesY);
    glVertexAttribIPointer(silhouettes->aIndicesY, 3, GL_UNSIGNED_INT, 0, 0);*/

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, toCompVertices);
    silhouettes->program->setUniformValue(silhouettes->uPositionDiff, 0);

    glDrawElements(GL_TRIANGLES_ADJACENCY, mesh->getNumberOfTrianglesAdjacency() * 6, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    //glDisableVertexAttribArray(silhouettes->aIndicesY);
    //glDisableVertexAttribArray(silhouettes->aIndicesX);
    glDisableVertexAttribArray(silhouettes->aPosition);

    //vboComputeIndicesY.release();
    //vboComputeIndicesX.release();
    vboVertices.release();
    iboElementsTrianglesAdjacency.release();

    glBindVertexArray(0);

    silhouettes->program->release();

    //glDepthMask(GL_TRUE);
    //glDisable(GL_MULTISAMPLE);
    //glEnable(GL_BLEND);

    //debugTexture("renderSilhouettes");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Renders polygonal
 */
void MainRenderer::renderPolygonal()
{
    if (!mesh) {
        qWarning() << "MainRenderer::renderPolygonal warning: null Mesh";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toPolygonal, 0);

    //glEnable(GL_BLEND);
    //glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
    //glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
    //glBlendColor(0, 0, 0, 1);
    //glEnable(GL_MULTISAMPLE);
    //glDisable(GL_DEPTH_TEST);
    //glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    polygonal->program->bind();
    // Set matrix to shader
    polygonal->program->setUniformValue(polygonal->uMatrix, matrix);
    polygonal->program->setUniformValue(polygonal->uNormalMatrix, (cameraMatrix * translationMatrix * rotationMatrix).normalMatrix());

    glBindVertexArray(vao);

    iboElementsTriangles.bind();

    vboVertices.bind();
    glEnableVertexAttribArray(polygonal->aPosition);
    glVertexAttribPointer(polygonal->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    /*vboComputeIndicesX.bind();
    glEnableVertexAttribArray(polygonal->aIndicesX);
    glVertexAttribIPointer(polygonal->aIndicesX, 3, GL_UNSIGNED_INT, 0, 0);
    vboComputeIndicesY.bind();
    glEnableVertexAttribArray(polygonal->aIndicesY);
    glVertexAttribIPointer(polygonal->aIndicesY, 3, GL_UNSIGNED_INT, 0, 0);*/
    vboVerticesColors.bind();
    glEnableVertexAttribArray(polygonal->aColor);
    glVertexAttribPointer(polygonal->aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (polygonalLightingEnabled) {
        vboNormals.bind();
        glEnableVertexAttribArray(polygonal->aNormal);
        glVertexAttribPointer(polygonal->aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, toCompVertices);
    polygonal->program->setUniformValue(polygonal->uPositionDiff, 0);

    glDrawElements(GL_TRIANGLES, mesh->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisableVertexAttribArray(polygonal->aColor);
    if (polygonalLightingEnabled) {
        glDisableVertexAttribArray(polygonal->aNormal);
        vboNormals.release();
    }
    //glDisableVertexAttribArray(polygonal->aIndicesY);
    //glDisableVertexAttribArray(polygonal->aIndicesX);
    glDisableVertexAttribArray(polygonal->aPosition);

    //vboComputeIndicesY.release();
    //vboComputeIndicesX.release();
    vboVerticesColors.release();
    vboVertices.release();

    iboElementsTriangles.release();

    glBindVertexArray(0);

    polygonal->program->release();

    glEnable(GL_CULL_FACE);
    //glDepthMask(GL_TRUE);
    //glEnable(GL_DEPTH_TEST);
    //glDisable(GL_MULTISAMPLE);
    //glDisable(GL_BLEND);

    //debugTexture("renderPolygonal");
    //QPixmap::fromImage(getCurrentImage()).save("test.png", "png", 100);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Renders given pyramid
 * @param[in] pyramidObject Given pyramid object
 */
void MainRenderer::renderPyramid(SSIMRenderer::Pyramid pyramidObject)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toPyramid, 0);

    //glEnable(GL_BLEND);
    //glDisable(GL_BLEND);
    //glDepthMask(GL_FALSE);
    //glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);

    pyramid->program->bind();
    pyramid->program->setUniformValue(pyramid->uMatrix, perspectiveMatrix * cameraMatrix);
    pyramid->program->setUniformValue(pyramid->uCorners, pyramidObject.getCorners());
    pyramid->program->setUniformValue(pyramid->uEye, pyramidObject.getEye());
    pyramid->program->setUniformValue(pyramid->uColor, pyramidObject.getColor());

    //glDrawArrays(GL_LINE_LOOP, 0, 12);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, toPyramidImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pyramidObject.getImage().width(), pyramidObject.getImage().height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pyramidObject.getImage().bits());
    pyramid->program->setUniformValue(pyramid->uTexture, 0);
    pyramid->program->setUniformValue(pyramid->uColor, pyramidObject.getColor());

    glBindVertexArray(vao);
    //glDrawArrays(GL_LINE_STRIP, 0, 12);
    glDrawArrays(GL_TRIANGLES, 12, 6);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    pyramid->program->release();

    glEnable(GL_CULL_FACE);
    //glDisable(GL_MULTISAMPLE);
    //glDepthMask(GL_TRUE);
    //glEnable(GL_BLEND);
    //glDisable(GL_BLEND);

    //debugTexture("renderPyramid");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Renders postprocessing
 */
void MainRenderer::renderPostprocessing()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fboOutput);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toOutput, 0);

    glViewport(0, 0, getCropWidth(), getCropHeight());
    glScissor(0, 0, getCropWidth(), getCropHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    // Draw textures on quad
    postprocessing->program->bind();

    float cropWidthR = (float) (getCropX() + getCropWidth()) / (float) getRenderWidth();
    float cropHeightR = (float) (getCropY() + getCropHeight()) / (float) getRenderHeight();
    float cropXR = (float) getCropX() / (float) getRenderWidth();
    float cropYR = (float) getCropY() / (float) getRenderHeight();

    //qDebug() << cropWidthR << cropHeightR << cropXR << cropYR;

    QVector2D leftBottomCorner(cropXR, cropYR);
    QVector2D rightTopCorner(cropWidthR, cropHeightR);

    postprocessing->program->setUniformValue(postprocessing->uLeftBottomCorner, leftBottomCorner);
    postprocessing->program->setUniformValue(postprocessing->uRightTopCorner, rightTopCorner);

    //postprocessing->program->setUniformValue(postprocessing->uLeftBottomCorner, QVector2D(0.0f, 0.0f));
    //postprocessing->program->setUniformValue(postprocessing->uRightTopCorner, QVector2D(1.0f, 1.0f));

    postprocessing->program->setUniformValue(postprocessing->uPyramidEnabled, pyramidEnabled);
    if (pyramidEnabled) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, toPyramid);
        postprocessing->program->setUniformValue(postprocessing->uPyramidTexture, 0);
    }

    postprocessing->program->setUniformValue(postprocessing->uDensityEnabled, densityEnabled);
    if (densityEnabled) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, toDensity);
        postprocessing->program->setUniformValue(postprocessing->uDensityTexture, 1);
    }

    postprocessing->program->setUniformValue(postprocessing->uPolygonalEnabled, polygonalEnabled);
    if (polygonalEnabled) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, toPolygonal);
        postprocessing->program->setUniformValue(postprocessing->uPolygonalTexture, 2);
    }

    postprocessing->program->setUniformValue(postprocessing->uSilhouettesEnabled, silhouettesEnabled);
    if (silhouettesEnabled) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, toSilhouettes);
        postprocessing->program->setUniformValue(postprocessing->uSilhouettesTexture, 3);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    postprocessing->program->release();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (isWindow()) {
        glViewport(0, 0, getRenderWidth(), getRenderHeight());
        glScissor(0, 0, getRenderWidth(), getRenderHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // Draw output texture on screen
        postprocessing->programSimple->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, toOutput);
        postprocessing->programSimple->setUniformValue(postprocessing->uOutputTexture, 0);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        postprocessing->programSimple->release();
    }

    //debugTexture(toOutput);
}

/**
 * @brief Prepares rendering
 */
void MainRenderer::prepareRendering()
{
    // Final matrix
    prepareTransformation();
    matrix = perspectiveMatrix * cameraMatrix * translationMatrix * rotationMatrix;

    // Resize for current width and height
    if (sizeChanged)
        resizeTexturesAndRenderbuffer();
    sizeChanged = false;

    recomputeStatisticalDataIfNeeded();

    // For every window the step can be different
    setRelativeTextureStep();

    // Initialize framebuffer for rendering to texture
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    clearViewport();

    if (pyramidEnabled) clearTexture(toPyramid);
    if (densityEnabled) clearTexture(toDensity);
    if (polygonalEnabled) clearTexture(toPolygonal);
    if (silhouettesEnabled) clearTexture(toSilhouettes);

    // Clear output texture
    glBindFramebuffer(GL_FRAMEBUFFER, fboOutput);
    clearTexture(toOutput);

    // Clear default viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (isWindow()) {
        clearViewport();
    }
}

/**
 * @brief Clears viewport
 */
void MainRenderer::clearViewport()
{
    glEnable(GL_SCISSOR_TEST);
    glViewport(0, 0, getRenderWidth(), getRenderHeight());
    glScissor(getCropX(), getCropY(), getCropWidth(), getCropHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

/**
 * @brief Clears texture
 * @param[in] id Texture id
 */
void MainRenderer::clearTexture(GLuint id)
{
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, id, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

/**
 * @brief Returns camera matrix
 * @return Camera matrix
 */
QMatrix4x4 MainRenderer::getCameraMatrix()
{
    return cameraMatrix;
}

/**
 * @brief Returns perspective matrix
 * @return Perspective matrix
 */
QMatrix4x4 MainRenderer::getPerspetiveMatrix()
{
    prepareTransformation();
    return perspectiveMatrix;
}

/**
 * @brief Convert point to OpenGL relative position
 * @param[in] point Point to convert
 * @return Converted point
 */
QPointF MainRenderer::convertPointToOpenGLRelative(QPointF point)
{
    QPointF pointOutput;
    pointOutput.setX((point.x() / (float) getRenderWidth() - 0.5f) * 2.0f);
    pointOutput.setY((point.y() / (float) getRenderHeight() - 0.5f) * 2.0f);
    return pointOutput;
}

/**
 * @brief Creates bounding box vertex array
 * @param[in] minVertex Min vertex
 * @param[in] maxVertex Max vertex
 * @param[out] verticesArray Output vertices array
 *
 * Unused.
 */
void MainRenderer::createBoundingBoxVertexArray(QVector3D minVertex, QVector3D maxVertex, GLfloat *verticesArray)
{
    verticesArray[0] = minVertex.x();
    verticesArray[1] = minVertex.y();
    verticesArray[2] = maxVertex.z();

    verticesArray[3] = maxVertex.x();
    verticesArray[4] = minVertex.y();
    verticesArray[5] = maxVertex.z();

    verticesArray[6] = maxVertex.x();
    verticesArray[7] = maxVertex.y();
    verticesArray[8] = maxVertex.z();

    verticesArray[9] = minVertex.x();
    verticesArray[10] = maxVertex.y();
    verticesArray[11] = maxVertex.z();

    verticesArray[12] = minVertex.x();
    verticesArray[13] = minVertex.y();
    verticesArray[14] = minVertex.z();

    verticesArray[15] = maxVertex.x();
    verticesArray[16] = minVertex.y();
    verticesArray[17] = minVertex.z();

    verticesArray[18] = maxVertex.x();
    verticesArray[19] = maxVertex.y();
    verticesArray[20] = minVertex.z();

    verticesArray[21] = minVertex.x();
    verticesArray[22] = maxVertex.y();
    verticesArray[23] = minVertex.z();
}

/**
 * @brief Catches event and sets render width and height
 * @param[in] event QEvent object
 * @return True
 */
bool MainRenderer::event(QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        setRenderWidth(((QResizeEvent *) event)->size().width());
        setRenderHeight(((QResizeEvent *) event)->size().height());
    }
    return true;
}

// Private stuff - make comments

/**
 * @brief MainRenderer::init
 */
void MainRenderer::init()
{
    densityEnabled = true;
    silhouettesEnabled = true;
    pyramidEnabled = true;
    polygonalEnabled = false;
    xMirroringEnabled = false;
    polygonalLightingEnabled = true;

    mesh = 0;
    lastBernCoeffsCount = 0;

    cWidth = 0;
    cHeight = 0;

    renderWidth = 1;
    renderHeight = 1;
    cropX = 0;
    cropY = 0;
    cropWidth = 1;
    cropHeight = 1;
    ratio = 1;
    step.setX(1);
    step.setY(1);
    sizeChanged = true;

    intensity = 0.5f;
    lineWidth = 0.005f;
    param = 1.0f;

    flagShareTransformations = true;
    //flagShareRenderSettings = true;

    recomputeCoefficientsDiffFlag = false;
    recomputeVerticesDiffFlag = false;

    matrix.setToIdentity();
    perspectiveMatrix.setToIdentity();
    cameraMatrix.setToIdentity();
    translationMatrix.setToIdentity();
    rotationMatrix.setToIdentity();

    setCamera(perspective);

    maxTextureOrRenderSize[0] = 0;
    maxTextureOrRenderSize[1] = 0;
}

/**
 * @brief MainRenderer::getVariablesLocations
 */
void MainRenderer::getVariablesLocations()
{
    // Density
    density->aPosition = density->program->attributeLocation("aPosition");
    //density->aIndicesX = density->program->attributeLocation("aIndicesX");
    //density->aIndicesY = density->program->attributeLocation("aIndicesY");
    density->uPositionDiff = density->program->uniformLocation("uPositionDiff");
    density->uPositionDiffLengthMinus1 = density->program->uniformLocation("uPositionDiffLengthMinus1");
    density->uPositionDiffLengthLog2 = density->program->uniformLocation("uPositionDiffLengthLog2");
    density->uParam = density->program->uniformLocation("uParam");
    density->uMatrix = density->program->uniformLocation("uMatrix");
    density->uMatrixInv = density->program->uniformLocation("uMatrixInv");
    density->uBernCoeffs = density->program->uniformLocation("uBernCoeffs");
    density->uBernCoeffsDiff = density->program->uniformLocation("uBernCoeffsDiff");
    density->uXMirror = density->program->uniformLocation("uXMirror");

    // Silhouettes
    silhouettes->aPosition = silhouettes->program->attributeLocation("aPosition");
    //silhouettes->aIndicesX = silhouettes->program->attributeLocation("aIndicesX");
    //silhouettes->aIndicesY = silhouettes->program->attributeLocation("aIndicesY");
    silhouettes->uPositionDiff = silhouettes->program->uniformLocation("uPositionDiff");
    silhouettes->uPositionDiffLengthMinus1 = silhouettes->program->uniformLocation("uPositionDiffLengthMinus1");
    silhouettes->uPositionDiffLengthLog2 = silhouettes->program->uniformLocation("uPositionDiffLengthLog2");
    silhouettes->uMatrix = silhouettes->program->uniformLocation("uMatrix");
    silhouettes->uXMirror = silhouettes->program->uniformLocation("uXMirror");

    // Computing
    computing->uT = computing->program->uniformLocation("uT");
    computing->uPcs = computing->program->uniformLocation("uPcs");
    computing->uWidth = computing->program->uniformLocation("uWidth");
    computing->uHeight = computing->program->uniformLocation("uHeight");

    // Pyramid
    pyramid->uMatrix = pyramid->program->uniformLocation("uMatrix");
    pyramid->uCorners = pyramid->program->uniformLocation("uCorners");
    pyramid->uEye = pyramid->program->uniformLocation("uEye");
    pyramid->uColor = pyramid->program->uniformLocation("uColor");
    pyramid->uTexture = pyramid->program->uniformLocation("uTexture");

    // Polygonal
    polygonal->aPosition = polygonal->program->attributeLocation("aPosition");
    //polygonal->aIndicesX = polygonal->program->attributeLocation("aIndicesX");
    //polygonal->aIndicesY = polygonal->program->attributeLocation("aIndicesY");
    polygonal->aColor = polygonal->program->attributeLocation("aColor");
    polygonal->aNormal = polygonal->program->attributeLocation("aNormal");
    polygonal->uPositionDiff = polygonal->program->uniformLocation("uPositionDiff");
    polygonal->uPositionDiffLengthMinus1 = polygonal->program->uniformLocation("uPositionDiffLengthMinus1");
    polygonal->uPositionDiffLengthLog2 = polygonal->program->uniformLocation("uPositionDiffLengthLog2");
    polygonal->uMatrix = polygonal->program->uniformLocation("uMatrix");
    polygonal->uNormalMatrix = polygonal->program->uniformLocation("uNormalMatrix");
    polygonal->uXMirror = polygonal->program->uniformLocation("uXMirror");
    polygonal->uLightingEnabled = polygonal->program->uniformLocation("uLightingEnabled");

    // Post-processing
    postprocessing->uDensityTexture = postprocessing->program->uniformLocation("uDensityTexture");
    postprocessing->uSilhouettesTexture = postprocessing->program->uniformLocation("uSilhouettesTexture");
    postprocessing->uPolygonalTexture = postprocessing->program->uniformLocation("uPolygonalTexture");
    postprocessing->uPyramidTexture = postprocessing->program->uniformLocation("uPyramidTexture");
    postprocessing->uIntensity = postprocessing->program->uniformLocation("uIntensity");
    postprocessing->uLineWidth = postprocessing->program->uniformLocation("uLineWidth");
    postprocessing->uTextureStep = postprocessing->program->uniformLocation("uTextureStep");

    postprocessing->uDensityEnabled = postprocessing->program->uniformLocation("uDensityEnabled");
    postprocessing->uSilhouettesEnabled = postprocessing->program->uniformLocation("uSilhouettesEnabled");
    postprocessing->uPolygonalEnabled = postprocessing->program->uniformLocation("uPolygonalEnabled");
    postprocessing->uPyramidEnabled = postprocessing->program->uniformLocation("uPyramidEnabled");

    postprocessing->uLeftBottomCorner = postprocessing->program->uniformLocation("uLeftBottomCorner");
    postprocessing->uRightTopCorner = postprocessing->program->uniformLocation("uRightTopCorner");

    postprocessing->uOutputTexture = postprocessing->programSimple->uniformLocation("uOutputTexture");
}

/**
 * @brief MainRenderer::initUniformVariables
 */
void MainRenderer::initUniformVariables()
{
    // Set uniform values
    setIntensity(intensity);
    setLineWidth(lineWidth);
    setParam(param);
    setRelativeTextureStep();

    postprocessing->program->bind();
    postprocessing->program->setUniformValue(postprocessing->uDensityEnabled, densityEnabled);
    postprocessing->program->setUniformValue(postprocessing->uSilhouettesEnabled, silhouettesEnabled);
    postprocessing->program->setUniformValue(postprocessing->uPolygonalEnabled, polygonalEnabled);
    postprocessing->program->setUniformValue(postprocessing->uPyramidEnabled, pyramidEnabled);
    postprocessing->program->setUniformValue(postprocessing->uLeftBottomCorner, QVector2D(0.0f, 0.0f));
    postprocessing->program->setUniformValue(postprocessing->uRightTopCorner, QVector2D(1.0f, 1.0f));
    postprocessing->program->release();

    silhouettes->program->bind();
    silhouettes->program->setUniformValue(silhouettes->uPositionDiffLengthLog2, positionDiffLengthLog2);
    silhouettes->program->setUniformValue(silhouettes->uPositionDiffLengthMinus1, positionDiffLengthMinus1);
    silhouettes->program->release();

    polygonal->program->bind();
    polygonal->program->setUniformValue(polygonal->uPositionDiffLengthLog2, positionDiffLengthLog2);
    polygonal->program->setUniformValue(polygonal->uPositionDiffLengthMinus1, positionDiffLengthMinus1);
    polygonal->program->release();

    enableXMirroring(xMirroringEnabled);

    enablePolygonalLighting(polygonalLightingEnabled);
}

/**
 * @brief MainRenderer::generateScreenTexture
 * @param id
 */
void MainRenderer::generateScreenTexture(GLuint *id)
{
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, getRenderWidth(), getRenderHeight(), 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief MainRenderer::setStatisticalData
 * @param statisticalData
 */
void MainRenderer::setStatisticalData(StatisticalData *statisticalData)
{
    if (!statisticalData) {
        qCritical() << "MainRenderer::setStatisticalData error: null StatisticalData";
        return;
    }

    // Compute texture height padding
    cHeight = statisticalData->getNumberOfRows() / cWidth + 1;

    this->statisticalData = statisticalData;

    if (!hasSharedContext()) {

        /*// Create indices for accesing of texture coords in shaders
        GLuint *indicesX = new GLuint[widthComp * heightComp * 3]();
        GLuint *indicesY = new GLuint[widthComp * heightComp * 3]();
        // Crazy magic code...
        int positionDiffLengthLog2 = (int(double(qLn(maxTextureOrRenderSize[0]) / qLn(2))));
        int positionDiffLengthMinus1 = maxTextureOrRenderSize[0] - 1;
        for (unsigned int i = 0; i < heightComp * widthComp; i++) {
            indicesX[i * 3] = (i * 3) & positionDiffLengthMinus1;
            indicesX[i * 3 + 1] = (i * 3 + 1) & positionDiffLengthMinus1;
            indicesX[i * 3 + 2] = (i * 3 + 2) & positionDiffLengthMinus1;

            indicesY[i * 3] = (i * 3) >> positionDiffLengthLog2;
            indicesY[i * 3 + 1] = (i * 3 + 1) >> positionDiffLengthLog2;
            indicesY[i * 3 + 2] = (i * 3 + 2) >> positionDiffLengthLog2;
        }
        vboComputeIndicesX.bind();
        vboComputeIndicesX.allocate(indicesX, sizeof(GLuint) * widthComp * heightComp * 3);
        vboComputeIndicesX.release();
        vboComputeIndicesY.bind();
        vboComputeIndicesY.allocate(indicesY, sizeof(GLuint) * widthComp * heightComp * 3);
        vboComputeIndicesY.release();
        delete[] indicesX;
        delete[] indicesY;*/

        glBindBuffer(GL_TEXTURE_BUFFER, tboT);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(GLfloat) * statisticalData->getNumberOfRows() * statisticalData->getNumberOfParameters(), statisticalData->getTMatrix(), GL_STATIC_DRAW);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        glBindTexture(GL_TEXTURE_BUFFER, toT);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tboT);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        computing->program->bind();
        computing->program->setUniformValue(computing->uWidth, cWidth);
        computing->program->setUniformValue(computing->uHeight, cHeight);
        computing->program->release();
    }
}

/**
 * @brief MainRenderer::recomputeDiff
 */
void MainRenderer::recomputeDiff()
{
    if (!statisticalData) {
        qCritical() << "MainRenderer::recomputeDiff error: null StatisticalData";
        return;
    }

    glBindBuffer(GL_TEXTURE_BUFFER, tboPcs);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(GLfloat) * statisticalData->getNumberOfParameters() * 1, statisticalData->getPcsMatrix(), GL_STATIC_DRAW);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    glBindTexture(GL_TEXTURE_BUFFER, toPcs);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tboPcs);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fboComputing);

    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, cWidth, cHeight);
    //glScissor(0, 0, cWidth, cHeight);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    computing->program->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, toT);
    computing->program->setUniformValue(computing->uT, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, toPcs);
    computing->program->setUniformValue(computing->uPcs, 1);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_BUFFER, 0);

    computing->program->release();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief MainRenderer::recomputeCoefficientsDiff
 */
void MainRenderer::recomputeCoefficientsDiff()
{
    recomputeCoefficientsDiffFlag = false;
    glBindTexture(GL_TEXTURE_2D, toCompCoeffs);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, cWidth, cHeight, 0, GL_RED, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fboComputing);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toCompCoeffs, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    recomputeDiff();
}

/**
 * @brief MainRenderer::recomputeVerticesDiff
 */
void MainRenderer::recomputeVerticesDiff()
{
    recomputeVerticesDiffFlag = false;
    glBindTexture(GL_TEXTURE_2D, toCompVertices);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, cWidth, cHeight, 0, GL_RED, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fboComputing);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, toCompVertices, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    recomputeDiff();
}

/**
 * @brief MainRenderer::compute
 */
void MainRenderer::recomputeStatisticalDataIfNeeded()
{
    // Recomputing if needed
    if (recomputeCoefficientsDiffFlag)
        recomputeCoefficientsDiff();

    if (recomputeVerticesDiffFlag)
        recomputeVerticesDiff();
}

/**
 * @brief MainRenderer::resizeTexturesAndRenderbuffer
 */
void MainRenderer::resizeTexturesAndRenderbuffer()
{
    // Update width and height on window resize
    glBindTexture(GL_TEXTURE_2D, toDensity);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, getRenderWidth(), getRenderHeight(), 0, GL_RGBA, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, toSilhouettes);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, getRenderWidth(), getRenderHeight(), 0, GL_RGBA, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, toPolygonal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, getRenderWidth(), getRenderHeight(), 0, GL_RGBA, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, toPyramid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, getRenderWidth(), getRenderHeight(), 0, GL_RGBA, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, toOutput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getCropWidth(), getCropHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getRenderWidth(), getRenderHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

/**
 * @brief MainRenderer::setRelativeTextureStep
 */
void MainRenderer::setRelativeTextureStep()
{
    postprocessing->program->bind();
    postprocessing->program->setUniformValue(postprocessing->uTextureStep, step);
    postprocessing->program->release();
}

/**
 * @brief MainRenderer::prepareTransformation
 */
void MainRenderer::prepareTransformation()
{
    perspectiveMatrix.setToIdentity();
    perspectiveMatrix.perspective(perspective.getFovy(), ratio, 1.0f, 1000.0f);
    perspectiveMatrix.scale(0.1f);
}

/**
 * @brief MainRenderer::debugFramebuffer
 * @param name
 */
void MainRenderer::debugFramebuffer()
{
    float *data = new float[getRenderWidth() * getRenderHeight() * 4]();
    glReadPixels(0, 0, getRenderWidth(), getRenderHeight(), GL_RGBA, GL_FLOAT, data);

    for (unsigned int i = 0; i < getRenderWidth() * getRenderHeight() * 4; i += 4) {
        if (data[i] + data[i + 1] + data[i + 2] + data[i + 3] != 0) {
            qDebug() << i / 4 << ":" << data[i] << data[i + 1] << data[i + 2] << data[i + 3];
            break;
        }
    }

    delete[] data;
}

/**
 * @brief MainRenderer::debugTexture
 */
void MainRenderer::debugTexture(GLuint id)
{
    uchar *data =  new uchar[getRenderWidth() * getRenderHeight()]();
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (unsigned int i = 0; i < getRenderWidth() * getRenderHeight(); i++) {
        if (data[i] != 0) {
            qDebug() << i << ":" << data[i];
            break;
        }
    }

    delete data;
}

/**
 * @brief MainRenderer::getCurrentImage Read current image in framebuffer
 * @return
 */
QImage MainRenderer::getCurrentImage()
{
    // @todo TODO unused
    QImage image(getRenderWidth(), getRenderHeight(), QImage::Format_RGB888);
    glReadPixels(0, 0, getRenderWidth(), getRenderHeight(), GL_RGB, GL_UNSIGNED_BYTE, image.bits());
    return image.mirrored();
}

/**
 * @brief MainRenderer::getAngles
 * @param matrix
 * @return
 */
QVector3D MainRenderer::getAngles(QMatrix4x4 matrix)
{
    // @todo TODO unused
    QVector3D angles;
    double y = qAsin(matrix(0, 2));
    double cY = cos(y);
    double x = qAsin(matrix(1, 2) / -cY);
    double z = qAsin(matrix(0, 1) / -cY);
    angles.setX(qRadiansToDegrees(x));
    angles.setY(qRadiansToDegrees(y));
    angles.setZ(qRadiansToDegrees(z));
    return angles;
}
}
