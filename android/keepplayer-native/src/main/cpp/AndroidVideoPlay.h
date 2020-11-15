//
// Created by lewis-v on 2020/10/18.
//

#ifndef KEEPPLAYER_ANDROIDVIDEOPLAY_H
#define KEEPPLAYER_ANDROIDVIDEOPLAY_H

extern "C" {

#include <libswscale/swscale.h>
};

#include <queue>
#include <mutex>
#include "../../../../../player/base/Macros.h"
#include "../../../../../player/media/VideoPlay.h"
#include "../../../../../player/model/ParseResult.h"
#include "../../../../../player/model/BaseNode.h"
#include "../../../../../player/render/IRender.h"

NS_KP_BEGIN
    class AndroidVideoPlay : public IVideoPlay {
    public:
        AndroidVideoPlay(ParseResult *playInfo, long long key);

        ~AndroidVideoPlay();

        virtual void resume() override;

        virtual void pause() override;

        virtual void stop() override;

        virtual void playFrame(double time, AVFrame* videoFrame) override;

        virtual void flush() override ;

    private:
#ifdef RENDER_RGB
        static const AVPixelFormat FORMAT = AV_PIX_FMT_RGB24;//AV_PIX_FMT_YUV420P;//AV_PIX_FMT_RGBA
#else
        static const AVPixelFormat FORMAT = AV_PIX_FMT_YUV420P;//AV_PIX_FMT_YUV420P;//AV_PIX_FMT_RGBA
#endif
        long long glThreadKey;
        std::queue<AVFrame*> bufferQueue;
        ParseResult *playInfo = nullptr;
        IRender* render = nullptr;
        std::mutex queueMutex;
        std::condition_variable queueCond;
        double audioMaxDiffTime = 33;

        SwsContext *swsContext = nullptr;

        int targetWidth = 360;
        int targetHeight = 640;

        bool onDrawFrame();
    };
NS_KP_END

#endif //KEEPPLAYER_ANDROIDVIDEOPLAY_H
