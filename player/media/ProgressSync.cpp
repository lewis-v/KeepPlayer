//
// Created by lewis-v on 2020/11/15.
//

#include "ProgressSync.h"

#include "../base/Macros.h"

NS_KP_BEGIN
    void ProgressSync::setProgress(double time) {
        currentTime = time;
    }

    double ProgressSync::getProgress() {
        return currentTime;
    }

NS_KP_END