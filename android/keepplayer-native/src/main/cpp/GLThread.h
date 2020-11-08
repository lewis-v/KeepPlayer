//
// Created by lewis-v on 2020/10/18.
//

#ifndef KEEPPLAYER_GLTHREAD_H
#define KEEPPLAYER_GLTHREAD_H

#include <jni.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <thread>
#include "../../../../../player/base/GLUtil.h"
#include "../../../../../player/base/Macros.h"
#include "../../../../../player/base/Base.h"
#include "../../../../../player/base/TaskQueue.h"
#include "../../../../../player/base/BaseQueueController.h"

NS_KP_BEGIN
    class GLThread : public Base, public BaseQueueController {
    public:
        GLThread(JNIEnv *env, jobject surface);

        ~GLThread();

        void post(const std::function<void()> &task);

        void resetSurface(JNIEnv *env, jobject surface);

        void setFps(int fps);

        void setFrameDrawCallback(std::function<bool()> callback);

    protected:
        virtual void onQueueStart() override;

        virtual void onQueueStop() override ;

        virtual void onQueueRun() override ;

        virtual int getWaitNextTIme() override ;

    private:
        ANativeWindow *nativeWindow = nullptr;
        EGLDisplay eglDisplay = nullptr;
        EGLContext eglContext = nullptr;
        EGLint eglFormat;
        EGLSurface eglSurface = nullptr;

        bool initSuccess = false;
        bool isInitFinish = false;
        int fps = 30;
        int delayTime = 1000 / fps;
        std::mutex glMutex;
        std::condition_variable condInitFinish;
        std::function<bool()> frameDrawCallback = {};

        int initGL();

        int deleteGL();
    };
NS_KP_END

#endif //KEEPPLAYER_GLTHREAD_H
