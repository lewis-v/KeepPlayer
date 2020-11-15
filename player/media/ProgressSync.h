//
// Created by lewis-v on 2020/11/15.
//

#ifndef PLAYER_PROGRESSSYNC_H
#define PLAYER_PROGRESSSYNC_H


#include "../base/Macros.h"
#include "../base/Base.h"

NS_KP_BEGIN
    class ProgressSync : Base {
    public:
        void setProgress(double time);

        double getProgress();

    private:
        double currentTime;
    };
NS_KP_END


#endif //PLAYER_PROGRESSSYNC_H
