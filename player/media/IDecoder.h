//
// Created by lewis-v on 2020/10/17.
//

#include "../base/Macros.h"
#include "../base/Base.h"
#include "../../includes/ffmpeg/libavcodec/avcodec.h"
#include "../base/BaseQueueController.h"
#include "../model/ParseResult.h"
#include "PacketQueue.h"

#ifndef PLAYER_IDECODER_H
#define PLAYER_IDECODER_H

NS_KP_BEGIN
    class IDecoder : public Base {
        virtual void resume() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual void addPacket(AVPacket avPacket) = 0;
    };

    class SimpleDecoder : public IDecoder, BaseQueueController {
    public:
        SimpleDecoder() = default;

        SimpleDecoder(ParseResult *playInfo);

        ~SimpleDecoder();

        virtual void resume() override;

        virtual void pause() override;

        virtual void stop() override;

        virtual void addPacket(AVPacket avPacket) override;

    protected:
        PacketQueue packetQueue;
        ParseResult *playInfo = nullptr;

        virtual void decodeIml(AVPacket &avPacket) = 0;

        void onQueueRun() override;

        void onQueueResume() override;

        void onQueuePause() override;

        void onQueueStop() override;
    };
NS_KP_END


#endif //PLAYER_IDECODER_H