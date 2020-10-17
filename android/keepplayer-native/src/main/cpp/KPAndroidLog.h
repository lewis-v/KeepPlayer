//
// Created by lewis-v on 2020/10/8.
//

#ifndef KEEPPLAYER_KPANDROIDLOG_H
#define KEEPPLAYER_KPANDROIDLOG_H

#include "../../../../../player/base/Macros.h"
#include "../../../../../player/base/Base.h"
#include "../../../../../player/base/Log.h"

NS_KP_BEGIN
    class KPAndroidLog : public LogCallBack {
    public:
        void logIml(const int level, const char *format, va_list args) override;
    };
NS_KP_END

#endif //KEEPPLAYER_KPANDROIDLOG_H
