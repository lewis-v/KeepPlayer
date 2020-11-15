//
// Created by lewis-v on 2020/10/7.
//

#include <jni.h>
#include "../../../../../player/base/Macros.h"
#include "../../../../../player/KeepPlayer.h"
#include "../../../../../player/base/Log.h"
#include "KPAndroidLog.h"
#include "GLThreadHelper.h"
#include "JniCallBack.h"
#include "../../../../../player/base/ImageParseUtil.h"

#define JNI_CLASS(FUNC) Java_com_lewis_keepplayer_KeepPlayerNative_##FUNC

void custom_decoderLog(void *ptr, int level, const char *fmt, va_list vl) {
    if (level < AV_LOG_WARNING) {
        KP::logE(fmt, vl);
    } else if (level == AV_LOG_WARNING) {
        KP::logW(fmt, vl);
    } else {
        KP::logI(fmt, vl);
    }
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(init)(JNIEnv *env, jobject thiz,
                jobject callback) {
    av_register_all();
    avformat_network_init();
    KeepPlayer::KPLog::setLogCallBack(new KeepPlayer::KPAndroidLog());
    KP::JniCallBack::init(env, callback);
    av_log_set_callback(custom_decoderLog);
}

extern "C"
JNIEXPORT jlong JNICALL JNI_CLASS(newInstance)(JNIEnv *env,
                                               jobject thiz) {
    //test code
    int w;
    int h;
    KP::logI("jni get image");
    KP::ImageParseUtil::getImageData("1",w,h);
    return (long long) new KP::KeepPlayer();
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(setPath)(JNIEnv *env, jobject thiz, jlong instance, jstring path) {
    if (instance == 0) {
        return;
    }
    auto srcPath = env->GetStringUTFChars(path, nullptr);
    ((KP::KeepPlayer *) instance)->setPath(srcPath);
    env->ReleaseStringUTFChars(path, srcPath);
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(prepare)(JNIEnv *env, jobject thiz, jlong instance) {
    if (instance == 0) {
        return;
    }
    ((KP::KeepPlayer *) instance)->prepare();
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(start)(JNIEnv *env, jobject thiz, jlong instance) {
    if (instance == 0) {
        return;
    }
    ((KP::KeepPlayer *) instance)->start();
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(resume)(JNIEnv *env, jobject thiz, jlong instance) {
    if (instance == 0) {
        return;
    }
    ((KP::KeepPlayer *) instance)->resume();
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(pause)(JNIEnv *env, jobject thiz, jlong instance) {
    if (instance == 0) {
        return;
    }
    ((KP::KeepPlayer *) instance)->pause();
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(stop)(JNIEnv *env, jobject thiz, jlong instance) {
    if (instance == 0) {
        return;
    }
    ((KP::KeepPlayer *) instance)->stop();
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(reset)(JNIEnv *env, jobject thiz, jlong instance) {
    if (instance == 0) {
        return;
    }
    ((KP::KeepPlayer *) instance)->reset();
}


extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(release)(JNIEnv *env, jobject thiz, jlong instance) {
    if (instance == 0) {
        return;
    }
    delete (KP::KeepPlayer *) instance;
    long long key = instance;
    KP::GLThreadHelper::getInstance()->removeGLThread(key);
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(setSurface)(JNIEnv *env, jobject thiz, jlong instance,
                      jobject surface) {
    if (instance == 0) {
        return;
    }
    long long key = instance;
    KP::GLThreadHelper::getInstance()->setGLThread(env, surface, key);
}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(updateSurfaceSize)(JNIEnv *env, jobject thiz,
                                                             jlong instance, jobject surface,
                                                             jint width, jint height) {
    if (instance == 0) {
        return;
    }
    long long key = instance;

}

extern "C"
JNIEXPORT void JNICALL
JNI_CLASS(test)(JNIEnv *env, jobject thiz) {

}