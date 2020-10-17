//
// Created by lewis-v on 2020/10/8.
//

#include "KPAndroidLog.h"
#include "android/log.h"

NS_KP_BEGIN
    void KPAndroidLog::logIml(const int level, const char *format, va_list args) {
        __android_log_vprint(level, "KP", format, args);
    }

NS_KP_END