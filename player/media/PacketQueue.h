//
// Created by lewis-v on 2020/8/31.
//

#ifndef PLAYER_PACKETQUEUE_H
#define PLAYER_PACKETQUEUE_H
extern "C" {
#include "../../includes/ffmpeg/libavcodec/avcodec.h"
};

#include "../base/Macros.h"
#include "../base/Base.h"
#include <queue>
#include <mutex>
#include <condition_variable>

NS_KP_BEGIN
    class IPacketQueue : public Base {
    public:
        virtual void push(AVPacket &packet) = 0;

        virtual bool pop(AVPacket *packet) = 0;

        virtual void clear() = 0;

        virtual int size() = 0;
    };

    class PacketQueue : public IPacketQueue {
    public:
        ~PacketQueue() override;

        void push(AVPacket &packet) override;

        bool pop(AVPacket *packet) override;

        void clear() override;

        int size() override;

    private:
        std::queue<AVPacket> queue;
        std::mutex queueMutex;
        std::condition_variable cond;
    };
NS_KP_END


#endif //PLAYER_PACKETQUEUE_H
