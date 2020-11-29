//
// Created by lewis-v on 2020/8/26.
//

#ifndef PLAYER_PACKETREAD_H
#define PLAYER_PACKETREAD_H


#include "../base/Base.h"
#include "../model/ParseResult.h"
#include "../base/TaskQueue.h"
#include "PacketQueue.h"
#include "../base/BaseQueueController.h"
#include <thread>

NS_KP_BEGIN


    class IPacketReadListener : public Base {
    public:
        virtual bool onReadVideo(AVPacket avPacket) = 0;

        virtual bool onReadAudio(AVPacket avPacket) = 0;

        virtual void onFlush() = 0;

        virtual void onReadProgress(double currentReadTime) = 0;
    };

    class SimplePacketReadListener : public IPacketReadListener {
    public:
        SimplePacketReadListener(std::function<bool(AVPacket)> readVideo,
                                 std::function<bool(AVPacket)> readAudio,
                                 std::function<void()> flush,
                                 std::function<void(double)> readProgress) : readAudio(readAudio),
                                                                   readVideo(readVideo),
                                                                   flush(flush),
                                                                   readProgress(readProgress) {

        }

        virtual bool onReadVideo(AVPacket avPacket) override {
            return readVideo(avPacket);
        }

        virtual bool onReadAudio(AVPacket avPacket) override {
            return readAudio(avPacket);
        }

        virtual void onFlush() override {
            flush();
        }

        virtual void onReadProgress(double currentReadTime) override {
            readProgress(currentReadTime);
        }

    protected:
        std::function<bool(AVPacket)> readVideo;
        std::function<bool(AVPacket)> readAudio;
        std::function<void()> flush;
        std::function<void(double)> readProgress;
    };

    class IPacketRead : public Base {
    protected:
        IPacketReadListener *readListener = nullptr;
    public:
        ~IPacketRead() override {
            KP_SAFE_DELETE(readListener);
        }

        virtual void resume() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual void seek(long long time) = 0;

        virtual bool isPause() = 0;

        virtual bool isResume() = 0;

        void setListener(IPacketReadListener *listener) {
            KP_SAFE_DELETE(this->readListener);
            this->readListener = listener;
        }

    };

    class PacketRead : public IPacketRead, BaseQueueController {
    public:
        explicit PacketRead(ParseResult *playInfo, IPacketReadListener *readListener = nullptr);

        ~PacketRead() override;

        void resume() override;

        void pause() override;

        void stop() override;

        void seek(long long time) override;

        bool isPause() override;

        bool isResume() override;

        virtual std::string getTag() override {
            return "PacketRead";
        }

        virtual int getWaitNextTIme() override ;

    private:
        ParseResult *playInfo = nullptr;
        AVPacket avPacket{};
        bool isComplete = false;
        double audioReadTime = 0;
        double videoReadTime = 0;

        void onQueueRun() override;

        void readIml();

        void notifyIml();

        void onQueueResume() override;

        void onQueuePause() override;

        void onQueueStop() override;
    };
NS_KP_END


#endif //PLAYER_PACKETREAD_H
