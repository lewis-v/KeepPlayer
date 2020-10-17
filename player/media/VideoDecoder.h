//
// Created by lewis-v on 2020/10/17.
//

#ifndef PLAYER_VIDEODECODER_H
#define PLAYER_VIDEODECODER_H


#include <string>
#include "../base/Macros.h"
#include "IDecoder.h"
#include "../model/ParseResult.h"
#include "PacketQueue.h"

NS_KP_BEGIN
    class VideoDecoder : public IDecoder {
    public:
//        VideoDecoder(ParseResult *playInfo);
//
//        ~VideoDecoder();
//
//        virtual void resume() override;
//
//        virtual void pause() override;
//
//        virtual void stop() override;
//
//        virtual void addPacket(AVPacket avPacket) override;
//
//        virtual std::string getTag() override {
//            return "VideoDecoder";
//        }

    private:
        PacketQueue packetQueue;
        ParseResult *playInfo = nullptr;
    };
NS_KP_END


#endif //PLAYER_VIDEODECODER_H
