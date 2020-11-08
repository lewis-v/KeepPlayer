//
// Created by lewis-v on 2020/10/18.
//

#ifndef PLAYER_VIDEOYUVBUFFER_H
#define PLAYER_VIDEOYUVBUFFER_H


#include <cstdlib>
#include "../base/Macros.h"
#include "../base/Base.h"

NS_KP_BEGIN
    class VideoYUVBuffer : public Base {
    public:
        double time;
        uint8_t *yFrame = nullptr;
        int yLineSize;
        uint8_t *uFrame = nullptr;
        int uLineSize;
        uint8_t *vFrame = nullptr;
        int vLineSize;

        ~VideoYUVBuffer() {
            if (yFrame) {
                free(yFrame);
            }
            if (uFrame) {
                free(uFrame);
            }
            if (vFrame) {
                free(vFrame);
            }
        }
    };
NS_KP_END


#endif //PLAYER_VIDEOYUVBUFFER_H
