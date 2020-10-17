//
// Created by lewis-v on 2020/8/17.
//

#include "Log.h"
#include <cstdio>
#include <cstdarg>

NS_KP_BEGIN

    static LogCallBack *logCallBack = nullptr;

    void KPLog::setLogCallBack(LogCallBack *callBack) {
        logCallBack = callBack;
    }

    void logIml(const int level, const char *format, va_list args) {
        if (logCallBack == nullptr) {
            vprintf(format, args);
        } else {
            logCallBack->logIml(level, format, args);
        }
    }

    void logI(const char *format, ...) {
        va_list args;
        va_start(args, format);
        logIml(LOG_LEVEL_I, format, args);
        va_end(args);
    }

    void logD(const char *format, ...) {
        va_list args;
        va_start(args, format);
        logIml(LOG_LEVEL_D, format, args);
        va_end(args);
    }

    void logE(const char *format, ...) {
        va_list args;
        va_start(args, format);
        logIml(LOG_LEVEL_E, format, args);
        va_end(args);
    }

    void logW(const char *format, ...) {
        va_list args;
        va_start(args, format);
        logIml(LOG_LEVEL_W, format, args);
        va_end(args);
    }

    void SimpleLogCallBack::logIml(const int level, const char *format, va_list args) {
        printf(format, args);
    }
NS_KP_END