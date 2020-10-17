//
// Created by lewis-v on 2020/8/17.
//

#ifndef PLAYER_LOG_H
#define PLAYER_LOG_H

#include <functional>
#include "Macros.h"
#include "Base.h"

NS_KP_BEGIN
    class LogCallBack : public Base {
    public:
        virtual void logIml(const int level, const char *format, va_list args) = 0;

        virtual ~LogCallBack() = default;
    };

    class KPLog {
    public:
        static void setLogCallBack(LogCallBack *callBack);
    };

    class SimpleLogCallBack : public LogCallBack {
        void logIml(const int level, const char *format, va_list args) override;
    };

    static const int LOG_LEVEL_D = 3;
    static const int LOG_LEVEL_I = 4;
    static const int LOG_LEVEL_W = 5;
    static const int LOG_LEVEL_E = 6;

    void logI(const char *format, ...);

    void logD(const char *format, ...);

    void logE(const char *format, ...);

    void logW(const char *format, ...);
NS_KP_END

#endif //PLAYER_LOG_H
