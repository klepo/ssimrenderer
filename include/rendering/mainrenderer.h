/**
 * @file        mainrenderer.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The header file with MainRenderer class declaration.
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

#ifndef SSIMR_MAINRENDERER_H
#define SSIMR_MAINRENDERER_H

#include "../ssimrenderer_global.h"

#include "densityfsgenerator/densityfsgenerator.h"
#include "../opengl/openglwrapper.h"
#include "../input/pyramid.h"
#include "../input/mesh.h"
#include "../input/statisticaldata.h"
#include "../input/csvcoeffsfile.h"

#include <QOpenGLBuffer>
#include <QResizeEvent>

namespace SSIMRenderer
{
/**
 * @brief The MainRenderer class represents the main renderering part (core) of SSIMRenderer library
 */
class SHARED_EXPORT MainRenderer : public OpenGLWrapper
{
public:
    // Creates MainRenderer using QSurface and optional parental OpenGLWrapper
    MainRenderer(QSurface *surface, OpenGLWrapper *parentOpenGLWrapper = 0);

    // Creates MainRenderer using QSurface, render width and height and optional parental OpenGLWrapper
    MainRenderer(QSurface *surface, GLuint renderWidth, GLuint renderHeight, OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of MainRenderer object
    virtual ~MainRenderer();

    // Inputs
    void setMesh(Mesh *mesh, GLfloat *colors = NULL, GLfloat *normals = NULL);
    void setVerticesColors(GLfloat *colors);
    void setNormals(GLfloat *normals);

    void setCoefficients(StatisticalData *statisticalData);
    void setVertices(StatisticalData *statisticalData);

    void updateCoefficients(StatisticalData *statisticalData);
    void updateVertices(StatisticalData *statisticalData);

    // Outputs
    virtual long getRecomputedVertices(float *&vertices, bool transformed = false) final;
    virtual QVector<QVector3D> getRecomputedVertices(bool transformed = false) final;
    QVector<bool> getVerticesMask(float *vertices = 0, int numberOfVertices = 0, const QRectF &cropRectangle = QRectF());

    // Export recomputed polygonal STL model
    void exportSTL(const QString &filename, bool transformed = true, QVector<bool> mask = QVector<bool>());

    // Export image
    virtual void saveRenderedImage(const QString &filePath) final;
    virtual QImage getRenderedImage() final;
    virtual void getRenderedRedChannel(float *&data) final;
    virtual void getCurrentSilhouettesImage(float *&image) final;
    GLuint getOutputTextureId() const;

    // Rendering parameters
    void setIntensity(double value);
    void setLineWidth(double value);
    void setParam(double value);

    // Get rendering parameters
    virtual double getIntensity() const final;
    virtual double getLineWidth() const final;
    virtual double getParam() const final;

    // Render size
    void setRenderWidth(GLuint renderWidth);
    void setRenderHeight(GLuint renderHeight);
    void setRenderSize(GLuint renderWidth, GLuint renderHeight);

    virtual GLuint getRenderWidth() const final;
    virtual GLuint getRenderHeight() const final;

    // Crop size
    void setCropWindow(GLuint cropX, GLuint cropY, GLuint cropWidth, GLuint cropHeight);
    void setCropWindow(const QRect &rectangle);

    virtual GLuint getCropX() const final;
    virtual GLuint getCropY() const final;
    virtual GLuint getCropWidth() const final;
    virtual GLuint getCropHeight() const final;
    virtual QRect getCropRectangle() const final;

    // Rendering settings
    void enableDensity(bool value);
    void enableSilhouettes(bool value);
    void enablePyramid(bool value);
    void enablePolygonal(bool value);
    void enableXMirroring(bool value);
    void enablePolygonalLighting(bool value);

    virtual bool isDensityEnabled() const final;
    virtual bool isSilhouettesEnabled() const final;
    virtual bool isPyramidEnabled() const final;
    virtual bool isPolygonalEnabled() const final;
    virtual bool isXMirroringEnabled() const final;
    virtual bool isLightingEnabled() const final;

    // Flag for sharing transformations between multiple windows
    void setFlagShareTransformations(bool value);

    // Scene settings - transformation
    // Translation
    virtual void setTranslation(float x, float y, float z) final;
    virtual void setTranslation(QVector3D position) final;
    virtual void getTranslation(float &x, float &y, float &z) final;
    virtual QVector3D getTranslation() final;
    virtual void translate(float x, float y, float z) final;
    virtual void translate(QVector3D position) final;

    // Rotation
    virtual void setRotation(float xAngle, float yAngle, float zAngle) final;
    virtual void setRotation(QVector3D rotation) final;
    virtual void setRotationMatrix(QMatrix4x4 rotationMatrix) final;
    virtual void getRotation(float &xAngle, float &yAngle, float &zAngle) final;
    virtual QVector3D getRotation() final;
    virtual QMatrix4x4 getRotationMatrix() final;
    virtual void rotate(float xAngle, float yAngle, float zAngle) final;
    virtual void rotate(QVector3D rotation) final;

    // Camera position and rotation
    virtual void setCamera(QVector3D leftTop, QVector3D leftBottom, QVector3D rightTop, QVector3D rightBottom, QVector3D eye) final;
    virtual void setCamera(SSIMRenderer::Pyramid camera) final;
    virtual void setCamera(QVector3D eye, QVector3D center) final;
    virtual void moveCamera(float distance, QVector3D center) final;
    virtual void rotateCamera(float xAngle, float yAngle, QVector3D center) final;

    // Camera base points positions
    virtual QVector3D getEye() final;
    virtual QVector3D getCenter() final;
    virtual QVector3D getMiddle() final;
    virtual QVector3D getViewDir() final;
    virtual void setCenter(const QVector3D &value) final;
    virtual void setEye(const QVector3D &value) final;

    // Perspective projection
    virtual void setPerspective(SSIMRenderer::Pyramid perspective) final;
    virtual void setPerspective(QVector3D leftTop, QVector3D leftBottom, QVector3D rightTop, QVector3D rightBottom, QVector3D eye) final;
    virtual SSIMRenderer::Pyramid getPerspective() final;

    // Control points
    void addPoint(const QVector3D &value);
    void setPoints(QVector<QVector3D> points);
    QVector<QVector3D> getPoints();
    //QVector<float> getMinimalDistancesToPoints(void (*f)(float *, unsigned char *, int, int) = NULL);
    //QVector<float> getMinimalDistancesToPoints();
    void clearPoints();

    StatisticalData *getStatisticalData() const;
    Mesh *getMesh() const;

protected:
    // Initialize and render
    virtual void initialize();
    virtual void render();

    // Render functions
    virtual void renderDensity() final;
    virtual void renderSilhouettes() final;
    virtual void renderPolygonal() final;
    virtual void renderPyramid(SSIMRenderer::Pyramid pyramid) final;
    virtual void renderPostprocessing() final;

    virtual void prepareRendering() final;
    virtual void clearViewport() final;
    virtual void clearTexture(GLuint id) final;

    // Get Matrices
    virtual QMatrix4x4 getCameraMatrix() final;
    virtual QMatrix4x4 getPerspetiveMatrix() final;

    // Helper function for recomputing
    virtual QPointF convertPointToOpenGLRelative(QPointF point) final;

    // Unused
    virtual void createBoundingBoxVertexArray(QVector3D minVertex, QVector3D maxVertex, GLfloat *verticesArray) final;

    // Event
    bool event(QEvent *event);

private:
    // Private stuff
    void init();

    void getVariablesLocations();
    void initUniformVariables();

    void generateScreenTexture(GLuint *id);

    void setStatisticalData(StatisticalData *statisticalData);

    void recomputeDiff();
    void recomputeCoefficientsDiff();
    void recomputeVerticesDiff();

    void recomputeStatisticalDataIfNeeded();

    void resizeTexturesAndRenderbuffer();
    void setRelativeTextureStep();

    void prepareTransformation();

    void debugFramebuffer();
    void debugTexture(GLuint id);

    QImage getCurrentImage();
    QVector3D getAngles(QMatrix4x4 matrix);

    DensityFSGenerator fsGenerator;

    // Rendering density
    struct Density {
        QOpenGLShaderProgram *program;
        QOpenGLShader *fragmentShader;
        GLuint aPosition;
        //GLuint aIndicesX;
        //GLuint aIndicesY;
        GLuint uPositionDiff;
        GLuint uMatrix;
        GLuint uMatrixInv;
        GLuint uBernCoeffs;
        GLuint uBernCoeffsDiff;
        GLuint uPositionDiffLengthMinus1;
        GLuint uPositionDiffLengthLog2;
        GLuint uParam;
        GLuint uXMirror;
    } *density;

    // Rendering silhouettes
    struct Silhouettes {
        QOpenGLShaderProgram *program;
        GLuint aPosition;
        //GLuint aIndicesX;
        //GLuint aIndicesY;
        GLuint uPositionDiff;
        GLuint uPositionDiffLengthMinus1;
        GLuint uPositionDiffLengthLog2;
        GLuint uMatrix;
        //GLuint uLineWidth;
        //GLuint uRatio;
        GLuint uXMirror;
    } *silhouettes;

    // Computing diffs
    struct Computing {
        QOpenGLShaderProgram *program;
        GLuint uT;
        GLuint uPcs;
        GLuint uWidth;
        GLuint uHeight;
    } *computing;

    // Rendering pyramid
    struct Pyramid {
        QOpenGLShaderProgram *program;
        GLuint uMatrix;
        GLuint uCorners;
        GLuint uEye;
        GLuint uColor;
        GLuint uTexture;
    } *pyramid;

    // Polygonal
    struct Polygonal {
        QOpenGLShaderProgram *program;
        GLuint aPosition;
        //GLuint aIndicesX;
        //GLuint aIndicesY;
        GLuint aColor;
        GLuint aNormal;
        GLuint uPositionDiff;
        GLuint uPositionDiffLengthMinus1;
        GLuint uPositionDiffLengthLog2;
        GLuint uMatrix;
        GLuint uNormalMatrix;
        GLuint uXMirror;
        GLuint uLightingEnabled;
    } *polygonal;

    // Post-processing
    struct Postprocessing {
        QOpenGLShaderProgram *program;
        QOpenGLShaderProgram *programSimple;
        GLuint uDensityTexture;
        GLuint uSilhouettesTexture;
        GLuint uPolygonalTexture;
        GLuint uPyramidTexture;

        GLuint uDensityEnabled;
        GLuint uSilhouettesEnabled;
        GLuint uPolygonalEnabled;
        GLuint uPyramidEnabled;

        GLuint uOutputTexture;

        GLuint uLeftBottomCorner;
        GLuint uRightTopCorner;

        GLuint uIntensity;
        GLuint uLineWidth;
        GLuint uTextureStep;
    } *postprocessing;

    // Frame Buffer Objects
    GLuint fbo;
    GLuint fboOutput;
    GLuint fboComputing;

    // Texture Objects
    GLuint toDensity;
    GLuint toSilhouettes;
    GLuint toPyramid;
    GLuint toPolygonal;
    GLuint toOutput;
    GLuint toPyramidImage;
    // Shared
    GLuint toCompCoeffs;
    GLuint toCompVertices;
    GLuint toBerncoeffs;
    GLuint toT;
    GLuint toPcs;

    // Element Buffer Objects
    QOpenGLBuffer iboElementsTetrahedra;
    QOpenGLBuffer iboElementsTriangles;
    QOpenGLBuffer iboElementsTrianglesAdjacency;

    // Vertex Buffer Objects
    QOpenGLBuffer vboVertices;
    QOpenGLBuffer vboVerticesColors;
    //QOpenGLBuffer vboComputeIndicesX;
    //QOpenGLBuffer vboComputeIndicesY;
    QOpenGLBuffer vboNormals;

    // Texture Buffer Objects
    GLuint tboBerncoeffs;
    GLuint tboT;
    GLuint tboPcs;

    // Render Buffer Object
    GLuint rbo;

    // Vertex Array Object
    GLuint vao;

    // Geometry info
    Mesh *mesh;

    // Statistical data for recomputing
    StatisticalData *statisticalData;

    // Last coefficients count
    GLuint lastBernCoeffsCount;

    // Compute sizes
    GLuint maxTextureOrRenderSize[2];
    GLuint positionDiffLengthLog2;
    GLuint positionDiffLengthMinus1;
    GLuint cWidth;
    GLuint cHeight;

    // Points for lines
    QVector<QVector3D> points;

    // Render Settings
    float intensity;
    float lineWidth;
    float param;

    bool densityEnabled;
    bool silhouettesEnabled;
    bool pyramidEnabled;
    bool polygonalEnabled;
    bool xMirroringEnabled;
    bool polygonalLightingEnabled;

    // Flags for computing
    bool recomputeCoefficientsDiffFlag;
    bool recomputeVerticesDiffFlag;

    // Render size stuff
    GLuint renderWidth;
    GLuint renderHeight;
    GLuint cropX;
    GLuint cropY;
    GLuint cropWidth;
    GLuint cropHeight;
    float ratio;
    QVector2D step;
    bool sizeChanged;

    // Transformation
    QMatrix4x4 matrix;
    QMatrix4x4 perspectiveMatrix;
    SSIMRenderer::Pyramid perspective;
    QMatrix4x4 cameraMatrix;
    QVector3D eye;
    QVector3D center;
    QVector3D up;
    QMatrix4x4 translationMatrix;
    QMatrix4x4 rotationMatrix;
    QVector3D angles;

    // Shared flags
    bool flagShareTransformations;
    bool flagShareRenderSettings;

    Q_DISABLE_COPY(MainRenderer)
};
}

#endif // SSIMR_MAINRENDERER_H
