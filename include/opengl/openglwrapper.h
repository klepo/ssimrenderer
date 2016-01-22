/**
 * @file        openglwrapper.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The header file with OpenGLWrapper class declaration.
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

#ifndef SSIMR_OPENGLWRAPPER_H
#define SSIMR_OPENGLWRAPPER_H

#include "../ssimrenderer_global.h"

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShader>
#include <QOpenGLDebugLogger>
#include <QWindow>
#include <QOffscreenSurface>
#include <QElapsedTimer>

namespace SSIMRenderer
{
// Set OpenGL version and profile
/// OpenGL major version
static const unsigned int OPENGL_MAJOR = 3;

/// OpenGL minor version
static const unsigned int OPENGL_MINOR = 3;

/// Selection of OpenGL profile (QSurfaceFormat::CoreProfile)
static const QSurfaceFormat::OpenGLContextProfile OPENGL_PROFILE = QSurfaceFormat::CoreProfile;

/// Qt OpenGL functions selection (QOpenGLFunctions_3_3_Core)
typedef QOpenGLFunctions_3_3_Core OPENGL_FUNCTIONS;

/**
 * @brief The OpenGLWrapper class represents the wrapper for OpenGL
 */
class SHARED_EXPORT OpenGLWrapper : public OPENGL_FUNCTIONS
{
public:
    // Creates a OpenGLWrapper object with surface and optional parental OpenGLWrapper
    OpenGLWrapper(QSurface *surface, OpenGLWrapper *parentOpenGLWrapper = 0);

    // Destructor of OpenGLWrapper object
    virtual ~OpenGLWrapper();

    // Main render function
    virtual int renderNow() final;

    // Is used shared context?
    virtual bool hasSharedContext() final;

    // Is surface class QSurface::Window?
    virtual bool isWindow() final;

    // Enables logging messages
    virtual void enableLogMessages(bool enable) final;

    // Is logging enabled?
    virtual bool isloggingEnabled() final;

    // Returns parental OpenGLWrapper
    virtual OpenGLWrapper *getParentOpenGLWrapper() final;

    // Returns list of shared OpenGLWrappers
    virtual QList<OpenGLWrapper *> getChildOpenGLWrappers() final;

    // Returns last render time in integers
    virtual int getLastRenderTime() const final;

    // Return last render time in doubles
    virtual double getLastRenderTimeDouble() const final;

    // Returns HGLRC - handle to native OpenGL rendering context
    virtual HGLRC getHGLRC() const final;

    // Returns HDC - handle to the device context
    virtual HDC getHDC() const final;

protected:
    /// Pure virtual render function
    virtual void render() = 0;

    /// Pure virtual initialize function
    virtual void initialize() = 0;

    // Checks initialization of OpenGL context and makes context current
    virtual void checkInitAndMakeCurrentContext() final;

    // Get GL error strings
    virtual QString getGLErrorString(GLenum errorCode) const final;
    virtual QString getGLFramebufferStatusString(GLenum errorCode) const final;

    // Helper functions for creating and linking shaders
    virtual void addShader(QOpenGLShaderProgram *program, QOpenGLShader::ShaderType type, QString filename) final;
    virtual void addShaderFromSource(QOpenGLShaderProgram *program, QOpenGLShader *shader, QString source) final;
    virtual void linkProgram(QOpenGLShaderProgram *program) final;

private:
    void initSurface(QSurface *surface);
    void createContext();
    bool isOpenGLVersionSupported();
    bool hasParentOpenGLWrapper();
    bool hasDebugExtension();
    void qSleep(int ms);
    static void messageLogged(const QOpenGLDebugMessage &message);

    QOpenGLContext *context;
    QSurface *activeSurface;
    HGLRC hGLRC;
    HDC hDC;
    QOpenGLDebugLogger *logger;
    OpenGLWrapper *parentOpenGLWrapper;
    QList<OpenGLWrapper *> childOpenGLWrappers;
    bool initialized;
    int lastRenderTime;
    float lastRenderTimeDouble;

    Q_DISABLE_COPY(OpenGLWrapper)
};
}

#endif // SSIMR_OPENGLWRAPPER_H

