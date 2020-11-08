//
// Created by lewis-v on 2020/8/31.
//

#ifndef PLAYER_AUDIODECODER_H
#define PLAYER_AUDIODECODER_H


#include "../base/Macros.h"
#include "../base/Base.h"
#include "PacketQueue.h"
#include "../base/BaseQueueController.h"
#include "../model/ParseResult.h"
#include "IDecoder.h"
#include <thread>

NS_KP_BEGIN
    class IAudioDecoderListener : public Base {
    public:
        virtual void onDecodeFinish(double time, int dataSize, uint8_t *data) = 0;
    };

    class SimpleAudioDecoderListener: public IAudioDecoderListener {
    public:
        SimpleAudioDecoderListener(std::function<void(double, int,uint8_t*)> func) {
            decodeFinish = func;
        }
        virtual void onDecodeFinish(double time, int dataSize, uint8_t *data) override {
            decodeFinish(time, dataSize, data);
        }

    protected:
        std::function<void(double, int,uint8_t*)> decodeFinish;
    };

    class IAudioDecoder : public SimpleDecoder {
    public:
        IAudioDecoder(ParseResult *playInfo) : SimpleDecoder(playInfo){}

        ~IAudioDecoder() {
            KP_SAFE_DELETE(decoderListener)
        }

        virtual void setAudioDecoderListener(IAudioDecoderListener *listener) {
            KP_SAFE_DELETE(decoderListener)
            decoderListener = listener;
        }

    protected:
        IAudioDecoderListener *decoderListener = nullptr;
    };

    class AudioDecoder : public IAudioDecoder, BaseQueueController {
    public:
        AudioDecoder(ParseResult *playInfo) ;

        ~AudioDecoder();

        virtual std::string getTag() override {
            return "AudioDecoder";
        }
    private:
        uint8_t *audioOutBuffer = nullptr;
        AVFrame *audioFrame = nullptr;
        int currentBufferSize = 1024;

        void decodeIml(AVPacket avPacket);
    };
NS_KP_END


#endif //PLAYER_AUDIODECODER_H
