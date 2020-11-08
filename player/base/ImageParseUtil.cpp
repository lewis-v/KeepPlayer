//
// Created by liuyiwei on 2020/7/8.
//

#include "ImageParseUtil.h"

#include "../base/Log.h"

#ifdef ANDROID

#include "../../android/keepplayer-native/src/main/cpp/JniCallBack.h"

#include <android/bitmap.h>

#include <utility>

#else

#include "FSCallback.h"

#endif

NS_KP_BEGIN
void * tempResult = nullptr;
    void *ImageParseUtil::getImageData(const std::string& filePath, int &width, int &height) {
        if (tempResult != nullptr) {
            logI("temp is not null");
            return tempResult;
        }
        if (filePath.empty()) {
            return nullptr;
        }
#ifdef ANDROID
        logI("getImageData by android");
        jobject bit = JniCallBack::getInstance()->getBitmap(filePath);
        if (bit != nullptr) {
            void *result;
            uint32_t *result2 = nullptr;
            AndroidBitmapInfo bitInfo;
            JNIEnv *env = JniCallBack::getInstance()->getEnv();
            if (env != nullptr) {
                AndroidBitmap_getInfo(env, bit, &bitInfo);
                width = bitInfo.width;
                height = bitInfo.height;
                int ret = AndroidBitmap_lockPixels(env, bit, &result);
                if(ANDROID_BITMAP_RESULT_SUCCESS == ret) {
                    logI("AndroidBitmap_lockPixels success");
                } else {
                    logE("AndroidBitmap_lockPixels fail %d", ret);
                    return nullptr;
                }
                int pixelsCount = bitInfo.width * bitInfo.height;
                result2 = static_cast<uint32_t *>(malloc(sizeof(uint32_t) * pixelsCount));
                memcpy(result2, (uint32_t *) result, sizeof(uint32_t) * pixelsCount);

                AndroidBitmap_unlockPixels(env, bit);
            }
            tempResult = result2;
            logI("set temp");
            return result2;
        }
//        auto imageData = JniCallBack::getInstance()->getBitmap(std::move(filePath));
//        return imageData;

#else
        void *data = FSCallback::getImageData(filePath, width, height);
        if (data) {
            return data;
        }
#endif
        return nullptr;
    }

    int ImageParseUtil::loadTexture(std::string filePath) {
        if (filePath.empty()) {
            return -1;
        }
#ifdef ANDROID
        return JniCallBack::getInstance()->loadTexture(filePath);
#else
    return 0;
#endif
    }
NS_KP_END

