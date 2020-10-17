//
// Created by lewis-v on 2020/10/5.
//

#ifndef PLAYER_AUDIOPLAY_H
#define PLAYER_AUDIOPLAY_H


#include "../base/Macros.h"
#include "../base/Base.h"

NS_KP_BEGIN
    class IAudioPlay : public Base {
    public:

        virtual void resume() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual void playFrame(double time, int dataSize, uint8_t *data) = 0;

    protected:

    };

    class AudioPlay : public IAudioPlay {
    public:
        AudioPlay();

        virtual void resume() override;

        virtual void pause() override;

        virtual void stop() override;

        virtual void playFrame(double time, int dataSize, uint8_t *data) override;


    };
NS_KP_END


#endif //PLAYER_AUDIOPLAY_H
