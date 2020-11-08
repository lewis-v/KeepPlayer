//
// Created by liuyiwei on 2020/7/8.
//

#include "JniCallBack.h"
#include "../../../../../player/base/Log.h"

NS_KP_BEGIN

    static JniCallBack *jniCallBackInstance = nullptr;

    JniCallBack::JniCallBack(JNIEnv *env, jobject object) {
        auto result = env->GetJavaVM(&javaVm);
        logI("jniCallBack result:%d", result);
        jListener = env->NewGlobalRef(object);
        jclass clazz = env->GetObjectClass(object);
        jGetBitmap = env->GetMethodID(clazz, "getBitmap",
                                      "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        jLoadTexture = env->GetMethodID(clazz, "loadTexture", "(Ljava/lang/String;)I");
    }

    JniCallBack *JniCallBack::getInstance() {
        return jniCallBackInstance;
    }

    void JniCallBack::init(JNIEnv *env, jobject object) {
        if (jniCallBackInstance == nullptr) {
            jniCallBackInstance = new JniCallBack(env, object);
        }
    }

    jobject JniCallBack::getBitmap(const std::string& path) {
        JNIEnv *env = getEnv();
        if (env != nullptr) {
            jobject result = env->CallObjectMethod(jListener, jGetBitmap,
                                                   env->NewStringUTF(path.c_str()));
          return result;
        }
        logI("getBitmap env is null");
        return nullptr;
    }

    int JniCallBack::loadTexture(const std::string& path) {
        JNIEnv *env = getEnv();
        if (env != nullptr) {
            jint result = env->CallIntMethod(jListener, jLoadTexture,
                                                   env->NewStringUTF(path.c_str()));
            return result;
        }
        return 0;
    }

    void JniCallBack::onParseError(long object, int code,  const std::string& msg) {
        JNIEnv *env = getEnv();
        if (env != nullptr) {
            env->CallVoidMethod(jListener, jOnParseError, (jlong)object,(jint)code, env->NewStringUTF(msg.c_str()));
        }
    }

    JniCallBack::~JniCallBack() {
        JNIEnv *env = getEnv();
        if (env != nullptr) {
            env->DeleteGlobalRef(jListener);
        }
    }

    JNIEnv *JniCallBack::getEnv() {
        JNIEnv *env = nullptr;
        int status = 0;
        if ((status = javaVm->GetEnv((void **)&env, JNI_VERSION_1_6)) == JNI_OK) {
            return env;
        }
        logI("getEnv 1.6 err :%d", status);
        if ((status = javaVm->GetEnv((void **)&env, JNI_VERSION_1_4)) == JNI_OK) {
            return env;
        }
        logI("getEnv 1.4 err :%d", status);
        if ((status = javaVm->GetEnv((void **)&env, JNI_VERSION_1_2)) == JNI_OK){
            return env;
        }
        logI("getEnv 1.2 err :%d", status);
        return env;
    }


NS_KP_END