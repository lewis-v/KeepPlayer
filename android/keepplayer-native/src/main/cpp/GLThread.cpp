//
// Created by lewis-v on 2020/10/18.
//

#include "GLThread.h"
#include "../../../../../player/base/Log.h"

NS_KP_BEGIN

    GLThread::GLThread(JNIEnv *env, jobject surface) {
        std::unique_lock<std::mutex> lck(glMutex);
        nativeWindow = ANativeWindow_fromSurface(env, surface);
        initSuccess = initGL() == 0;
        isInitFinish = true;
        condInitFinish.notify_all();
    }

    int GLThread::initGL() {
        eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (eglDisplay == EGL_NO_DISPLAY) {
            logI("eglGetDisplay failure.");
            return -1;
        }

        logD("eglGetDisplay ok");

        EGLint majorVersion;
        EGLint minorVersion;
        EGLBoolean success = eglInitialize(eglDisplay, &majorVersion,
                                           &minorVersion);
        if (!success) {
            logI("eglInitialize failure.");
            return -1;
        }
        logD("eglInitialize ok");

        GLint numConfigs;
        EGLConfig config;
        const EGLint CONFIG_ATTRIBS[] = {EGL_BUFFER_SIZE, EGL_DONT_CARE,
                                         EGL_RED_SIZE, 5, EGL_GREEN_SIZE, 6, EGL_BLUE_SIZE, 5,
                                         EGL_DEPTH_SIZE, 16, EGL_ALPHA_SIZE, EGL_DONT_CARE,
                                         EGL_STENCIL_SIZE,
                                         EGL_DONT_CARE, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                         EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE // the end
        };
        success = eglChooseConfig(eglDisplay, CONFIG_ATTRIBS, &config, 1,
                                  &numConfigs);
        if (!success) {
            logI("eglChooseConfig failure.");
            return -1;
        }
        logD("eglChooseConfig ok");

        const EGLint attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT,
                                      attribs);
        if (eglContext == EGL_NO_CONTEXT) {
            logI("eglCreateContext failure, error is %d", eglGetError());
            return -1;
        }
        logD("eglCreateContext ok");

        success = eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID,
                                     &eglFormat);
        if (!success) {
            logI("eglGetConfigAttrib failure.");
            return -1;
        }
        logD("eglGetConfigAttrib ok");

        eglSurface = eglCreateWindowSurface(eglDisplay, config,
                                            nativeWindow, 0);
        if (nullptr == eglSurface) {
            logI("eglCreateWindowSurface failure.");
            return -1;
        }
        logD("eglCreateWindowSurface ok");
        return 0;
    }

    int GLThread::deleteGL() {
        if (!eglDisplay) {
            return 0;
        }
        EGLBoolean success;
        if (eglSurface) {
            success = eglDestroySurface(eglDisplay, eglSurface);
            if (!success) {
                logI("eglDestroySurface failure.");
                return -1;
            }
        }
        if (eglContext) {
            success = eglDestroyContext(eglDisplay, eglContext);
            if (!success) {
                logI("eglDestroySurface failure.");
                return -1;
            }
        }

        success = eglTerminate(eglDisplay);
        if (!success) {
            logI("eglDestroySurface failure.");
            return -1;
        }

        eglSurface = nullptr;
        eglContext = nullptr;
        eglDisplay = nullptr;

        return 0;
    }

    GLThread::~GLThread() {
        stopSync();
        if (nativeWindow) {
            ANativeWindow_release(nativeWindow);
        }
    }

    void GLThread::post(const std::function<void()> &task) {
        taskQueue.postTask(task);
    }

    void GLThread::resetSurface(JNIEnv *env, jobject surface) {
        std::unique_lock<std::mutex> lck(glMutex);
        deleteGL();
        if (nativeWindow) {
            ANativeWindow_release(nativeWindow);
        }
        nativeWindow = ANativeWindow_fromSurface(env, surface);
        initSuccess = initGL() == 0;
    }

    void GLThread::setFps(int fps) {
        if (fps == 0) {
            logE("GLThread set fps 0");
            return;
        }
        this->fps = fps;
        delayTime = 1000 / fps;
    }

    void GLThread::setFrameDrawCallback(std::function<bool()> callback) {
        this->frameDrawCallback = callback;
    }

    void GLThread::onQueueRun() {
        BaseQueueController::onQueueRun();
        std::unique_lock<std::mutex> lck(glMutex);
        glClearColor(0, 0, 0, 0);
        if (frameDrawCallback()) {
            eglSwapBuffers(eglDisplay, eglSurface);
        }
    }

    int GLThread::getWaitNextTIme() {
        return delayTime;
    }

    void GLThread::onQueueStart() {
        BaseQueueController::onQueueStart();
        std::unique_lock<std::mutex> lck(glMutex);
        if (!isInitFinish) {
            condInitFinish.wait(lck);//等待初始化完成,这个gl线程才算start成功
        }
        EGLBoolean success = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
        if (!success) {
            logE("eglMakeCurrent err");
        } else {
            logD("eglMakeCurrent success");
        }
    }

    void GLThread::onQueueStop() {
        BaseQueueController::onQueueStop();
        deleteGL();
    }

NS_KP_END

