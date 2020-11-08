//
// Created by lewis on 2020/8/17.
//

#ifndef PLAYER_KEEPPLAYER_H
#define PLAYER_KEEPPLAYER_H


#include <string>
#include "base/Macros.h"
#include "base/Base.h"
#include "media/PacketRead.h"
#include "media/AudioDecoder.h"
#include "media/AudioPlay.h"
#include "media/VideoDecoder.h"
#include "media/VideoPlay.h"

NS_KP_BEGIN


    class KeepPlayer : public Base {
    public:
        KeepPlayer() = default;

        ~KeepPlayer();

        void setPath(const std::string &path);

        void prepare();

        void prepareAsync();

        void start();

        void resume();

        void pause();

        void stop();

        void reset();

        void seek(long long time);


        class IPlayPrepareListener : public Base {
        public:
            virtual void onPrepare(KeepPlayer *play) = 0;
        };

        class IPlayCompletionListener : public Base {
        public:
            virtual void onCompletion(KeepPlayer *play) = 0;
        };

        class IPlayBufferListener : public Base {
        public:
            virtual void onBufferingUpdate(KeepPlayer *play, int percent) = 0;
        };

        class IPlaySeekCompleteListener : public Base {
        public:
            virtual void onSeekComplete(KeepPlayer *play) = 0;
        };

        class IPlayErrorListener : public Base {
        public:
            virtual void onError(KeepPlayer *play, int what, int extra) = 0;
        };

        class IPlayInfoListener : public Base {
        public:
            virtual void onInfo(KeepPlayer *play, int what, int extra) = 0;
        };

    private:
        std::string path = "";

        IPlayPrepareListener *prepareListener = nullptr;
        IPlayCompletionListener *completionListener = nullptr;
        IPlayBufferListener *bufferListener = nullptr;
        IPlaySeekCompleteListener *seekCompleteListener = nullptr;
        IPlayErrorListener *errorListener = nullptr;
        IPlayInfoListener *infoListener = nullptr;

        ParseResult* parseResult = nullptr;
        IPacketRead *read = nullptr;

        IAudioDecoder *audioDecoder = nullptr;
        IAudioPlay *audioPlay = nullptr;

        IVideoDecoder *videoDecoder = nullptr;
        IVideoPlay *videoPlay = nullptr;

        void prepareFinish();

    };
NS_KP_END


#endif //PLAYER_KEEPPLAYER_H
