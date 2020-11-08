//
// Created by lewis-v on 2020/10/18.
//

#include "GLThreadHelper.h"

NS_KP_BEGIN
    void GLThreadHelper::setGLThread(JNIEnv *env, jobject surface, long long key) {
        auto thread = glThreadMap.find(key);
        if (thread != glThreadMap.end()) {
            delete thread->second;
        }
        glThreadMap[key] = new GLThread(env, surface);
    }

    void GLThreadHelper::removeGLThread(long long key) {
        glThreadMap.erase(key);
    }

    GLThread *GLThreadHelper::getGLThread(long long key) {
        return glThreadMap[key];
    }

    GLThreadHelper* instance = nullptr;
    GLThreadHelper *GLThreadHelper::getInstance() {
        if (instance == nullptr) {
            instance = new GLThreadHelper();
        }
        return instance;
    }

NS_KP_END