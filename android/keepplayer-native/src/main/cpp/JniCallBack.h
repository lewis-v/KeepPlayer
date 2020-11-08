//
// Created by liuyiwei on 2020/7/8.
//

#ifndef FREQUENCYSPECTRUMDRAW_JNICALLBACK_H
#define FREQUENCYSPECTRUMDRAW_JNICALLBACK_H

#include "../../../../../player/base/Macros.h"
#include <jni.h>
#include <string>
#include <vector>

NS_KP_BEGIN

    class ImageResultData {
    public:
        int width;
        int height;
        char* data = nullptr;
        int len;
        ~ImageResultData() {
            if (data != nullptr) {

            }
        }
    };

    class JniCallBack {
    public:
        static JniCallBack *getInstance();

        static void init(JNIEnv *env, jobject object);

        JniCallBack(JNIEnv *env, jobject object);

        ~JniCallBack();

        jobject getBitmap(const std::string& path);

        int loadTexture(const std::string& path);

        void onParseError(long object, int code, const std::string& msg);

        JNIEnv *getEnv();

    private:
        JavaVM *javaVm;
        jobject jListener;
        jmethodID jGetBitmap;
        jmethodID jLoadTexture;
        jmethodID jOnParseError;
    };
NS_KP_END

#endif //FREQUENCYSPECTRUMDRAW_JNICALLBACK_H
