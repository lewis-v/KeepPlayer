//
// Created by lewis-v on 2020/8/22.
//

#ifndef PLAYER_PLAYLISTENER_H
#define PLAYER_PLAYLISTENER_H


#include "base/Macros.h"
#include "KeepPlayer.h"
#include "base/Base.h"

NS_KP_BEGIN

    class PlayListenerAdapter
            : public KeepPlayer::IPlayPrepareListener,
              KeepPlayer::IPlayCompletionListener,
              KeepPlayer::IPlayBufferListener,
              KeepPlayer::IPlaySeekCompleteListener,
              KeepPlayer::IPlayErrorListener,
              KeepPlayer::IPlayInfoListener {
    public:
        virtual void onPrepare(KeepPlayer *play) override {

        }

        virtual void onCompletion(KeepPlayer *play) override {

        }

        virtual void onBufferingUpdate(KeepPlayer *play, int percent) override {

        }

        virtual void onSeekComplete(KeepPlayer *play) override {

        }

        virtual void onError(KeepPlayer *play, int what, int extra) override {

        }

        virtual void onInfo(KeepPlayer *play, int what, int extra) override {

        }
    };

    class PlayListenerFunctionAdapter : public PlayListenerAdapter {
    public:
        //todo 析构函数需要进行处理
        virtual void onPrepare(KeepPlayer *play) override {
            onPrepareFunction(play);
        }

        virtual void onCompletion(KeepPlayer *play) override {
            onCompletionFunction(play);
        }

        virtual void onBufferingUpdate(KeepPlayer *play, int percent) override {
            onBufferingUpdateFunction(play, percent);
        }

        virtual void onSeekComplete(KeepPlayer *play) override {
            onSeekCompleteFunction(play);
        }

        virtual void onError(KeepPlayer *play, int what, int extra) override {
            onErrorFunction(play, what, extra);
        }

        virtual void onInfo(KeepPlayer *play, int what, int extra) override {
            onInfoFunction(play, what, extra);
        }

    private:
        std::function<void(KeepPlayer *)> onPrepareFunction;
        std::function<void(KeepPlayer *)> onCompletionFunction;
        std::function<void(KeepPlayer *, int)> onBufferingUpdateFunction;
        std::function<void(KeepPlayer *)> onSeekCompleteFunction;
        std::function<void(KeepPlayer *, int, int)> onErrorFunction;
        std::function<void(KeepPlayer *, int, int)> onInfoFunction;
    };

NS_KP_END


#endif //PLAYER_PLAYLISTENER_H
