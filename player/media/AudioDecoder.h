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

    class IAudioDecoder : public Base {
    public:
        virtual void setAudioDecoderListener(IAudioDecoderListener *listener) {
            KP_SAFE_DELETE(decoderListener)
            decoderListener = listener;
        }

        virtual void start() = 0;

        virtual void resume() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual void addPacket(AVPacket avPacket) = 0;

    protected:
        IAudioDecoderListener *decoderListener = nullptr;
    };

    class AudioDecoder : public IAudioDecoder, BaseQueueController {
    public:
        AudioDecoder(ParseResult *playInfo) ;

        ~AudioDecoder();

        virtual void start() override;

        virtual void resume() override;

        virtual void pause() override;

        virtual void stop() override;

        virtual void addPacket(AVPacket avPacket) override ;

        virtual std::string getTag() override {
            return "AudioDecoder";
        }
    private:
        PacketQueue packetQueue;
        ParseResult *playInfo = nullptr;
        uint8_t *audio_out_buffer = nullptr;
        AVFrame *audioFrame = nullptr;
        int currentBufferSize = 1024;

        void decodeIml(AVPacket avPacket);

        void onQueueRun() override;

        void onQueueResume() override;

        void onQueuePause() override;

        void onQueueStop() override;
    };
NS_KP_END


#endif //PLAYER_AUDIODECODER_H
