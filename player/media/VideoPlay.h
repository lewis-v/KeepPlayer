//
// Created by lewis-v on 2020/10/17.
//

#ifndef PLAYER_VIDEOPLAY_H
#define PLAYER_VIDEOPLAY_H

extern "C" {
#include <libavutil/frame.h>
};
#include "../base/Macros.h"
#include "../base/Base.h"

NS_KP_BEGIN
    class IVideoPlay : public Base {
    public:

        virtual void resume() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual void playFrame(double time, AVFrame *videoFrame) = 0;

    protected:

    };

    class VideoPlay : public IVideoPlay {
    public:
        VideoPlay();

        virtual void resume() override;

        virtual void pause() override;

        virtual void stop() override;

        virtual void playFrame(double time, AVFrame *videoFrame) override;


    };
NS_KP_END


#endif //PLAYER_VIDEOPLAY_H
