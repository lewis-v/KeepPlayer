//
// Created by lewis-v on 2020/10/18.
//

#ifndef PLAYER_IRENDER_H
#define PLAYER_IRENDER_H

#include "../base/Macros.h"
#include "../base/Base.h"

extern "C" {
#include "../../includes/ffmpeg/libavutil/frame.h"
};

NS_KP_BEGIN
    class IRender : public Base {
    public:

        virtual void render(AVFrame *frame, int width, int height) = 0;
    };
NS_KP_END

#endif //PLAYER_IRENDER_H
