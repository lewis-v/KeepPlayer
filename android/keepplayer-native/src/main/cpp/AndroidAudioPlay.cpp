//
// Created by lewis on 2020/10/7.
//

#include "AndroidAudioPlay.h"
#include "../../../../../player/base/Log.h"
#include "../../../../../player/base/Config.h"

NS_KP_BEGIN

    void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
        if (context == nullptr) return;
        auto *play = (AndroidAudioPlay *) context;
        play->pushBuffer();
    }


    AndroidAudioPlay::AndroidAudioPlay(ParseResult *playInfo) {
        this->playInfo = playInfo;
        if (!createEngine()) {
            return;
        }
        if (!createMix()) {
            return;
        }
        if (!createPlay()) {
            return;
        }
    }

    void AndroidAudioPlay::playFrame(double time, int dataSize, uint8_t *data) {
        if (dataSize <= 0) {
            return;
        }
        //todo 这里可能会加上一个buffer复用
        auto curr = new AudioBuffer();
        curr->data = static_cast<uint8_t *>(malloc(dataSize * sizeof(uint8_t)));
        memcpy(curr->data, data, dataSize * sizeof(uint8_t));
        curr->time = time;
        curr->size = dataSize;
        std::unique_lock<std::mutex> lck(queueMutex);
        while (bufferQueue.size() > MAX_CACHE_AV_FRAME_SIZE) {
            queueCond.wait(lck);
        }
        bufferQueue.emplace(curr);
    }

    bool AndroidAudioPlay::createEngine() {
        SLresult result;//返回结果
        result = slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);//第一步创建引擎
        if (SL_RESULT_SUCCESS != result) {
            logE("createEngine slCreateEngine err %d", result);
            return false;
        }
        result = (*engineObject)->Realize(engineObject,
                                          SL_BOOLEAN_FALSE);//实现（Realize）engineObject接口对象
        if (SL_RESULT_SUCCESS != result) {
            logE("createEngine Realize err %d", result);
            return false;
        }
        result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
                                               &engineEngine);//通过engineObject的GetInterface方法初始化engineEngine
        if (SL_RESULT_SUCCESS != result) {
            logE("createEngine GetInterface err %d", result);
            return false;
        }
        return true;
    }

    bool AndroidAudioPlay::createMix() {
        const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
        const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
        const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
        SLresult result;//返回结果
        result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
        if (SL_RESULT_SUCCESS != result) {
            logE("createMix CreateOutputMix err %d", result);
            return false;
        }
        result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
        if (SL_RESULT_SUCCESS != result) {
            logE("createMix Realize err %d", result);
            return false;
        }
        result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                                  &outputMixEnvironmentalReverb);
        if (SL_RESULT_SUCCESS != result) {
            logE("createMix GetInterface err %d", result);
            return false;
        }
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        if (SL_RESULT_SUCCESS != result) {
            logE("createMix SetEnvironmentalReverbProperties err %d", result);
            return false;
        }
        return true;
    }

    bool AndroidAudioPlay::createPlay() {
        SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
        SLDataSink audioSnk = {&outputMix, NULL};
        SLresult result;//返回结果

        SLDataLocator_AndroidSimpleBufferQueue android_queue = {
                SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
        auto channelMask = SL_SPEAKER_FRONT_CENTER;
        if (playInfo->audioInfo->audioCodeContext->channels == 2) {
            channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        }

        SLDataFormat_PCM pcm = {
                SL_DATAFORMAT_PCM,//播放pcm格式的数据
                static_cast<SLuint32>(playInfo->audioInfo->channelCount),//2,//2个声道（立体声）
                static_cast<SLuint32>(playInfo->audioInfo->audioCodeContext->sample_rate *
                                      1000),//SL_SAMPLINGRATE_44_1,//44100hz的频率
                SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
                SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
                channelMask,//SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
                SL_BYTEORDER_LITTLEENDIAN//结束标志
        };
        SLDataSource slDataSource = {&android_queue, &pcm};


        const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
        const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

        result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource,
                                                    &audioSnk, 3, ids, req);
        if (SL_RESULT_SUCCESS != result) {
            logE("createPlay CreateAudioPlayer err %d", result);
            return false;
        }
        //初始化播放器
        result = (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
        if (SL_RESULT_SUCCESS != result) {
            logE("createPlay CreateAudioPlayer err %d", result);
            return false;
        }

//    得到接口后调用  获取Player接口
        result = (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);
        if (SL_RESULT_SUCCESS != result) {
            logE("createPlay CreateAudioPlayer err %d", result);
            return false;
        }

        //    注册回调缓冲区 获取缓冲队列接口
        result = (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE,
                                                  &pcmBufferQueue);
        if (SL_RESULT_SUCCESS != result) {
            logE("createPlay CreateAudioPlayer err %d", result);
            return false;
        }
        //    获取音量接口
        (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmPlayerVolume);
        //缓冲接口回调
        result = (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue,
                                                     pcmBufferCallBack,
                                                     this);
        if (SL_RESULT_SUCCESS != result) {
            logE("createPlay CreateAudioPlayer err %d", result);
            return false;
        }
        return true;
    }

    void AndroidAudioPlay::pushBuffer() {
        if (pcmBufferQueue != nullptr) {
            std::unique_lock<std::mutex> lck(queueMutex);
            if (!bufferQueue.empty()) {
                auto *audioNode = bufferQueue.front();
                (*pcmBufferQueue)->Enqueue(pcmBufferQueue, audioNode->data,
                                           static_cast<SLuint32>(audioNode->size *
                                                                 sizeof(uint8_t)));
//                logI("pushBuffer %d size:%d", audioNode->data->size * sizeof(uint8_t), size);
                KP_SAFE_DELETE(audioNode->data)
                audioNode->data = nullptr;
                if (progressSync != nullptr) {
                    progressSync->setProgress(audioNode->time);
                }
                bufferQueue.pop();
                queueCond.notify_one();
            } else {
                auto* data = static_cast<uint8_t *>(malloc(sizeof(uint8_t)));
                (*pcmBufferQueue)->Enqueue(pcmBufferQueue, data,
                                           sizeof(uint8_t));
                free(data);
//                logD("audio push buffer null");
            }
        }
    }

    void AndroidAudioPlay::resume() {
        auto result = (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
        if (SL_RESULT_SUCCESS != result) {
            logE("AndroidAudioPlay resume err %d", result);
        }
        pushBuffer();
    }

    void AndroidAudioPlay::pause() {
        auto result = (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
        if (SL_RESULT_SUCCESS != result) {
            logE("AndroidAudioPlay pause err %d", result);
        }
    }

    void AndroidAudioPlay::stop() {
        auto result = (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_STOPPED);
        if (SL_RESULT_SUCCESS != result) {
            logE("AndroidAudioPlay stop err %d", result);
        }
        clearNode();
    }

    void AndroidAudioPlay::clearNode() {
        std::unique_lock<std::mutex> lck(queueMutex);
        while (!bufferQueue.empty()) {
            KP_SAFE_DELETE(bufferQueue.front());
            bufferQueue.pop();
        }
        queueCond.notify_one();
    }

    AndroidAudioPlay::~AndroidAudioPlay() {
        stop();
        if (pcmPlayerObject != nullptr) {
            (*pcmPlayerObject)->Destroy(pcmPlayerObject);
            pcmPlayerObject = nullptr;
            pcmBufferQueue = nullptr;
            pcmPlayerPlay = nullptr;
            pcmPlayerVolume = nullptr;
        }
        if (outputMixObject != nullptr) {
            (*outputMixObject)->Destroy(outputMixObject);
            outputMixObject = nullptr;
            outputMixEnvironmentalReverb = nullptr;
        }
        if (engineObject != nullptr) {
            (*engineObject)->Destroy(engineObject);
            engineObject = nullptr;
            engineEngine = nullptr;
        }
    }

    void AndroidAudioPlay::flush() {
        clearNode();
    }


NS_KP_END

