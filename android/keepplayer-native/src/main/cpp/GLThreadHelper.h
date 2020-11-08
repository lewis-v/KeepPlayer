//
// Created by lewis-v on 2020/10/18.
//

#ifndef KEEPPLAYER_GLTHREADHELPER_H
#define KEEPPLAYER_GLTHREADHELPER_H

#include <jni.h>
#include "../../../../../player/base/Macros.h"
#include "../../../../../player/base/Base.h"
#include "GLThread.h"
#include <map>

NS_KP_BEGIN
    class GLThreadHelper : public Base {
    public:

        static GLThreadHelper* getInstance();

        void setGLThread(JNIEnv *env, jobject surface, long long key);

        void removeGLThread(long long key);

        GLThread * getGLThread(long long key);

    private:
        std::map<long long, GLThread*> glThreadMap;
    };
NS_KP_END

#endif //KEEPPLAYER_GLTHREADHELPER_H
