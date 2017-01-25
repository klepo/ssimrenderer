/**
 * @file        openglwrapper.cpp
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2015
 *
 * @brief       The implementation file containing the OpenGLWrapper class.
 *
 * OpenGL context is created here. Rendering is doing here.
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

#include "opengl/openglwrapper.h"

namespace SSIMRenderer
{
/**
 * @brief Creates a OpenGLWrapper with surface and optional parental OpenGLWrapper
 * @param[in] surface Surface usually of type QOffscreenSurface or QWindow
 * @param[in] parentOpenGLWrapper Parenthal OpenGLWrapper
 */
OpenGLWrapper::OpenGLWrapper(QSurface *surface, OpenGLWrapper *parentOpenGLWrapper)
    : parentOpenGLWrapper(0)
    , context(0)
    , logger(0)
    , initialized(false)
    , activeSurface(0)
    , lastRenderTime(0)
{
    if (parentOpenGLWrapper) {
        this->parentOpenGLWrapper = parentOpenGLWrapper;
        parentOpenGLWrapper->childOpenGLWrappers.append(this);
    }

    initSurface(surface);
}

/**
 * @brief Destructor of OpenGLWrapper object
 *
 * Does nothing.
 */
OpenGLWrapper::~OpenGLWrapper()
{
    //@todo TODO BUG
    //delete context;
    //delete logger;
}

/**
 * @brief Main render loop function
 * @return Elapsed time in milliseconds (integer)
 *
 * Calls render() function
 */
int OpenGLWrapper::renderNow()
{
    checkInitAndMakeCurrentContext();

    if (activeSurface->surfaceClass() == QSurface::Window && (!((QWindow *) activeSurface)->isExposed() || !((QWindow *) activeSurface)->isVisible()) && hasSharedContext())
        return 0;

    glFinish();

    double elapsedMs = 0;
    QElapsedTimer timer;
    timer.start();

    //glFinish();
    //context->makeCurrent(activeSurface);
    render();
    glFinish();

    if ((activeSurface->surfaceClass() == QSurface::Window && ((QWindow *) activeSurface)->isExposed()) /*|| activeSurface->surfaceClass() == QSurface::Offscreen*/)
        context->swapBuffers(activeSurface);

    qint64 elapsed = timer.nsecsElapsed();
    elapsedMs = (double) elapsed / 1000000.0;
    //elapsedMs = elapsed / 1000.0;
    lastRenderTimeDouble = elapsedMs;
    lastRenderTime = elapsedMs;

    return elapsedMs;
}

/**
 * @brief Is used shared context?
 * @return True if object uses any parenthal shared context
 */
bool OpenGLWrapper::hasSharedContext()
{
    bool ret = false;
    if (context->shareContext())
        ret = true;
    return ret;
}

/**
 * @brief Is surface class QSurface::Window?
 * @return True if surface class is type of QSurface::Window
 */
bool OpenGLWrapper::isWindow()
{
    return (activeSurface->surfaceClass() == QSurface::Window);
}

/**
 * @brief Enables or disables logging messages
 * @param[in] enable Boolean flag
 */
void OpenGLWrapper::enableLogMessages(bool enable)
{
    if (hasDebugExtension()) {
        if (!enable)
            logger->stopLogging();
        else
            logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }
}

/**
 * @brief Is logging enabled?
 * @return True if logging is enabled
 */
bool OpenGLWrapper::isloggingEnabled()
{
    return logger->isLogging();
}

/**
 * @brief Returns pointer to parental OpenGLWrapper
 * @return Pointer to parental OpenGLWrapper
 */
OpenGLWrapper *OpenGLWrapper::getParentOpenGLWrapper()
{
    if (hasParentOpenGLWrapper())
        return parentOpenGLWrapper;
    else
        return NULL;
}

/**
 * @brief Returns list of shared OpenGLWrappers
 * @return List of shared OpenGLWrappers
 */
QList<OpenGLWrapper *> OpenGLWrapper::getChildOpenGLWrappers()
{
    return childOpenGLWrappers;
}

/**
 * @brief Returns last render time in integers
 * @return Last render time in integers
 */
int OpenGLWrapper::getLastRenderTime() const
{
    return lastRenderTime;
}

/**
 * @brief Return last render time in doubles
 * @return Last render time in doubles
 */
double OpenGLWrapper::getLastRenderTimeDouble() const
{
    return lastRenderTimeDouble;
}

/**
 * @brief Returns HGLRC - handle to native OpenGL rendering context
 * @return HGLRC - handle to native OpenGL rendering context
 */
HGLRC OpenGLWrapper::getHGLRC() const
{
    return hGLRC;
}

/**
 * @brief Returns HDC - handle to the device context
 * @return HDC - handle to the device context
 */
HDC OpenGLWrapper::getHDC() const
{
    return hDC;
}

/**
 * @brief Checks initialization of OpenGL context and makes context current
 */
void OpenGLWrapper::checkInitAndMakeCurrentContext()
{
    if (!context)
        createContext();

    context->makeCurrent(activeSurface);

    if (!initialized) {
        initialized = true;
        initialize();
    }
}

/**
 * @brief Converts OpenGL error codes to string
 * @param[in] errorCode OpenGL error code
 * @return Error string
 */
QString OpenGLWrapper::getGLErrorString(GLenum errorCode) const
{
    QString ret = "UNKNOWN ERROR";
    switch (errorCode) {
    case GL_INVALID_ENUM:
        ret = "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        ret = "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        ret = "GL_INVALID_OPERATION";
        break;
    case GL_STACK_OVERFLOW:
        ret = "GL_STACK_OVERFLOW";
        break;
    case GL_STACK_UNDERFLOW:
        ret = "GL_STACK_UNDERFLOW";
        break;
    case GL_OUT_OF_MEMORY:
        ret = "GL_OUT_OF_MEMORY";
        break;
    case GL_TABLE_TOO_LARGE:
        ret = "GL_TABLE_TOO_LARGE";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        ret = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    default:
        break;
    }
    return ret;
}

/**
 * @brief Converts OpenGL framebuffer status error codes to string
 * @param[in] errorCode OpenGL framebuffer status error code
 * @return Framebuffer error string
 */
QString OpenGLWrapper::getGLFramebufferStatusString(GLenum errorCode) const
{
    QString ret = "UNKNOWN ERROR";
    switch (errorCode) {
    case GL_FRAMEBUFFER_COMPLETE:
        ret = "GL_FRAMEBUFFER_COMPLETE";
        break;
    case GL_FRAMEBUFFER_UNDEFINED:
        ret = "GL_FRAMEBUFFER_UNDEFINED";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        ret = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        ret = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        ret = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        ret = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        ret = "GL_FRAMEBUFFER_UNSUPPORTED";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        ret = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        ret = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
        break;
    default:
        break;
    }
    return ret;
}

/**
 * @brief Adds shader from source file to program
 * @param[in, out] program OpenGL shader program
 * @param[in] type Shader type
 * @param[in] filename Path to shader source file
 */
void OpenGLWrapper::addShader(QOpenGLShaderProgram *program, QOpenGLShader::ShaderType type, QString filename)
{
    bool status;
    status = program->addShaderFromSourceFile(type, filename);
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader compile and add error" << program->log();
}

/**
 * @brief Creates shader from string source and adds shader to program
 * @param[in, out] program OpenGL shader program
 * @param[in, out] shader OpenGL shader
 * @param[in] source Shader string source
 */
void OpenGLWrapper::addShaderFromSource(QOpenGLShaderProgram *program, QOpenGLShader *shader, QString source)
{
    bool status;
    status = shader->compileSourceCode(source);
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader compile error" << shader->log();
    status = program->addShader(shader);
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader add error" << program->log();
}

/**
 * @brief Links shader program
 * @param[in, out] program OpenGL shader program
 */
void OpenGLWrapper::linkProgram(QOpenGLShaderProgram *program)
{
    bool status;
    status = program->link();
    if (!status && !isloggingEnabled())
        qCritical() << "OpenGL shader link error" << program->log();
}

/**
 * @brief Initializes surface
 * @param[in, out] surface Surface
 */
void OpenGLWrapper::initSurface(QSurface *surface) {
    activeSurface = surface;
    QSurfaceFormat surfaceFormat;

    if (activeSurface->surfaceClass() == QSurface::Window) {
        ((QWindow *) activeSurface)->setSurfaceType(QWindow::OpenGLSurface);
        surfaceFormat = ((QWindow *) activeSurface)->requestedFormat();
    } else {
        surfaceFormat = ((QOffscreenSurface *) activeSurface)->requestedFormat();
    }

    surfaceFormat.setVersion(OPENGL_MAJOR, OPENGL_MINOR);
    surfaceFormat.setProfile(OPENGL_PROFILE);
    surfaceFormat.setOption(QSurfaceFormat::DebugContext);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setRedBufferSize(8);
    surfaceFormat.setGreenBufferSize(8);
    surfaceFormat.setBlueBufferSize(8);
    surfaceFormat.setAlphaBufferSize(8);
    surfaceFormat.setSamples(4);

    if (activeSurface->surfaceClass() == QSurface::Window) {
        ((QWindow *) activeSurface)->setFormat(surfaceFormat);
        ((QWindow *) activeSurface)->create();
    } else {
        ((QOffscreenSurface *) activeSurface)->setFormat(surfaceFormat);
        ((QOffscreenSurface *) activeSurface)->create();
    }
}

/**
 * @brief OpenGLWrapper::createContext
 */
void OpenGLWrapper::createContext()
{
    if (!context) {
        context = new QOpenGLContext();

        if (activeSurface->surfaceClass() == QSurface::Window) {
            context->setFormat(((QWindow *) activeSurface)->requestedFormat());
        } else {
            context->setFormat(((QOffscreenSurface *) activeSurface)->requestedFormat());
        }

        if (hasParentOpenGLWrapper()) {
            parentOpenGLWrapper->checkInitAndMakeCurrentContext();
            context->setShareContext(parentOpenGLWrapper->context);
        }

        context->create();
        context->makeCurrent(activeSurface);
        //qDebug() << context->supportsThreadedOpenGL();

        hGLRC = 0;
        hDC = 0;
#ifdef USE_OPENCL
        hGLRC = wglGetCurrentContext();
        hDC = wglGetCurrentDC();
#endif // USE_OPENCL

        if (hasDebugExtension()) {
            logger = new QOpenGLDebugLogger();
            logger->initialize();
            QObject::connect(logger, &QOpenGLDebugLogger::messageLogged, SSIMRenderer::OpenGLWrapper::messageLogged);
            logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        }

        if (!isOpenGLVersionSupported()) {
            qCritical("Could not obtain required OpenGL context version");
            Q_ASSERT_X(false, "OpenGL error", "Could not obtain required OpenGL context version");
            exit(1);
        } else {
            initializeOpenGLFunctions();
        }
    }
}

/**
 * @brief Checks OpenGL version support (defined in OPENGL_FUNCTIONS)
 * @return True if version is supported
 */
bool OpenGLWrapper::isOpenGLVersionSupported()
{
    OPENGL_FUNCTIONS* funcs = 0;
    funcs = context->versionFunctions<OPENGL_FUNCTIONS>();
    return funcs;
}

/**
 * @brief Has this object parental OpenGLWrapper and shared context?
 * @return True if this object has parental OpenGLWrapper
 */
bool OpenGLWrapper::hasParentOpenGLWrapper()
{
    bool ret = false;
    if (parentOpenGLWrapper && parentOpenGLWrapper->context)
        ret = true;
    return ret;
}

/**
 * @brief Has OpenGL debug extension?
 * @return True if OpenGL has GL_KHR_debug extension
 */
bool OpenGLWrapper::hasDebugExtension()
{
    return context->hasExtension(QByteArrayLiteral("GL_KHR_debug"));
}

/**
 * @brief Helper sleep function
 * @param[in] ms Number of milliseconds to sleep
 */
void OpenGLWrapper::qSleep(int ms)
{
    Q_ASSERT(ms > 0);

#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

/**
 * @brief Debug message logged callback function (slot)
 * @param[in] message Debug message
 */
void OpenGLWrapper::messageLogged(const QOpenGLDebugMessage &message)
{
    if (message.type() == QOpenGLDebugMessage::ErrorType)
        qCritical() << message;
    /*else if (message.type() == QOpenGLDebugMessage::PerformanceType)
        qDebug() << message;*/
}
}
