//
// Created by lewis-v on 2020/10/17.
//

#ifndef PLAYER_VIDEODECODER_H
#define PLAYER_VIDEODECODER_H

extern "C" {

#include <libswscale/swscale.h>
};

#include <string>
#include "../base/Macros.h"
#include "IDecoder.h"
#include "../model/ParseResult.h"
#include "PacketQueue.h"

NS_KP_BEGIN
    class IVideoDecoderListener : public Base {
    public:
        virtual void onDecodeFinish(double time, AVFrame* videoFrame) = 0;
    };

    class SimpleVideoDecoderListener : public IVideoDecoderListener {
    public:
        SimpleVideoDecoderListener(std::function<void(double, AVFrame* )> func) {
            decodeFinish = func;
        }

        virtual void onDecodeFinish(double time, AVFrame* videoFrame) override {
            decodeFinish(time, videoFrame);
        }

    protected:
        std::function<void(double, AVFrame* )> decodeFinish;
    };

    class IVideoDecoder : public SimpleDecoder {
    public:
        IVideoDecoder(ParseResult *playInfo) : SimpleDecoder(playInfo) {}

        ~IVideoDecoder() {
            KP_SAFE_DELETE(decoderListener)
        }

        virtual void setVideoDecoderListener(IVideoDecoderListener *listener) {
            KP_SAFE_DELETE(decoderListener)
            decoderListener = listener;
        }

    protected:
        IVideoDecoderListener *decoderListener = nullptr;
    };

    class VideoDecoder : public IVideoDecoder {
    public:
        VideoDecoder(ParseResult *playInfo);

        ~VideoDecoder();

        virtual std::string getTag() override {
            return "VideoDecoder";
        }

    private:
        AVFrame *videoFrame;

        void decodeIml(AVPacket &avPacket);
    };
NS_KP_END


#endif //PLAYER_VIDEODECODER_H
