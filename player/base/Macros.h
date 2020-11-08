//
// Created by lewis-v on 2020/8/17.
//
#ifndef PLAYER_MACROS_H
#define PLAYER_MACROS_H

#define NS_KP_BEGIN                     namespace KeepPlayer {
#define NS_KP_END                       }
#define KP                              KeepPlayer
#define USING_NS_KP                     using namespace KeepPlayer
#define KP_SAFE_DELETE(p)               if(p) {delete (p); (p) = nullptr;}

#define KP_FF_TIME_MS(ts)               (av_rescale(ts, 1000, AV_TIME_BASE))
#define KP_MS_FF_TIME(ms)               (av_rescale(ms, AV_TIME_BASE, 1000))

#endif //PLAYER_MACROS_H
