//
// Created by lewis on 2020/10/7.
//

#ifndef KEEPPLAYER_ANDROIDAUDIOPLAY_H
#define KEEPPLAYER_ANDROIDAUDIOPLAY_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <mutex>
#include <queue>
#include "../../../../../player/base/Macros.h"
#include "../../../../../player/media/AudioPlay.h"
#include "../../../../../player/model/BaseNode.h"
#include "../../../../../player/model/AudioBuffer.h"
#include "../../../../../player/model/ParseResult.h"

NS_KP_BEGIN
    class AndroidAudioPlay : public IAudioPlay {
    public:
        AndroidAudioPlay(ParseResult *playInfo);

        ~AndroidAudioPlay();

        virtual void resume() override;

        virtual void pause() override;

        virtual void stop() override;

        virtual void flush() override ;

        void pushBuffer();

        virtual void playFrame(double time, int dataSize, uint8_t *data) override;

    private:
        SLObjectItf engineObject = nullptr;
        SLEngineItf engineEngine = nullptr;

        SLObjectItf outputMixObject = nullptr;
        SLEnvironmentalReverbItf outputMixEnvironmentalReverb = nullptr;

        SLObjectItf pcmPlayerObject = nullptr;
        SLPlayItf pcmPlayerPlay = nullptr;
        SLAndroidSimpleBufferQueueItf pcmBufferQueue = nullptr;
        SLVolumeItf pcmPlayerVolume = nullptr;

        std::queue<AudioBuffer *> bufferQueue;
        ParseResult *playInfo = nullptr;

        std::mutex queueMutex;
        std::condition_variable queueCond;

        void clearNode();

        bool createEngine();

        bool createMix();

        bool createPlay();
    };
NS_KP_END

#endif //KEEPPLAYER_ANDROIDAUDIOPLAY_H
