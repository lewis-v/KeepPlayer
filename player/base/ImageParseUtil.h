//
// Created by liuyiwei on 2020/7/8.
//

#ifndef FREQUENCYSPECTRUMDRAW_IMAGEPARSEUTIL_H
#define FREQUENCYSPECTRUMDRAW_IMAGEPARSEUTIL_H

#include <string>
#include "../base/Macros.h"

#ifdef ANDROID
#include "../../android/keepplayer-native/src/main/cpp/JniCallBack.h"
#endif

NS_KP_BEGIN
    class ImageParseUtil {
    public:
        static void *getImageData(const std::string& filePath, int &width, int &height);

        static int loadTexture(std::string filePath);
    };
NS_KP_END

#endif //FREQUENCYSPECTRUMDRAW_IMAGEPARSEUTIL_H
