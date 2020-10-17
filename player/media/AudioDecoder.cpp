//
// Created by lewis-v on 2020/8/31.
//

#include "AudioDecoder.h"

#include "../base/Macros.h"
#include "../base/Log.h"

NS_KP_BEGIN

    void AudioDecoder::start() {

    }

    void AudioDecoder::resume() {
        queueResume();
    }

    void AudioDecoder::pause() {
        queuePause();
    }

    void AudioDecoder::stop() {
        queueStop();
        packetQueue.clear();
    }

    void AudioDecoder::onQueueResume() {

    }

    void AudioDecoder::onQueuePause() {

    }

    void AudioDecoder::onQueueStop() {

    }

    void AudioDecoder::onQueueRun() {
        if (playInfo == nullptr) {
            logI("AudioDecoder onQueueRun playInfo is null");
            return;
        }
        AVPacket avPacket{};
        logI("pop");
        auto isSuccess = packetQueue.pop(&avPacket);
        logI("pop end");
        if (!isSuccess) {
            logW("AudioDecoder onQueueRun pop not success");
            return;
        }
        decodeIml(avPacket);
    }

    AudioDecoder::AudioDecoder(ParseResult *playInfo) {
        this->playInfo = playInfo;
        audio_out_buffer = (uint8_t *) av_malloc(currentBufferSize * sizeof(uint8_t));
        audioFrame = av_frame_alloc();
    }

    AudioDecoder::~AudioDecoder() {
    logI("~AudioDecoder start");
        packetQueue.clear();
        stopSync();
        if (audioFrame != nullptr) {
            av_frame_free(&audioFrame);
            audioFrame = nullptr;
        }
        if (audio_out_buffer != nullptr) {
            av_free(audio_out_buffer);
            audio_out_buffer = nullptr;
        }
        KP_SAFE_DELETE(decoderListener)
        logI("~AudioDecoder end");
    }

    void AudioDecoder::decodeIml(AVPacket avPacket) {
        auto ret = avcodec_send_packet(playInfo->audioInfo->audioCodeContext, &avPacket);
        if (ret == 0) {
            while (avcodec_receive_frame(playInfo->audioInfo->audioCodeContext, audioFrame) == 0) {
                int out_buffer_size = av_samples_get_buffer_size(nullptr,
                                                                 playInfo->audioInfo->channelCount,
                                                                 audioFrame->nb_samples,
                                                                 AV_SAMPLE_FMT_S16, 1);
                if (out_buffer_size > currentBufferSize) {//扩容
                    logI("decoder buffer resize old :%d new :%d", currentBufferSize,
                         out_buffer_size);
                    uint8_t *prt = (uint8_t *) av_realloc(audio_out_buffer,
                                                          out_buffer_size * sizeof(uint8_t));
                    if (prt != nullptr) {
                        currentBufferSize = out_buffer_size;
                        audio_out_buffer = prt;
                        logI("decode resize finish");
                    } else {
                        logE("the end decoder buffer resize old :%d new :%d but it return is null",
                             currentBufferSize, out_buffer_size);
                        continue;
                    }
                }
                swr_convert(playInfo->audioInfo->swrCtx, &(audio_out_buffer),
                            playInfo->audioInfo->audioCodeContext->sample_rate,
                            (const uint8_t **) audioFrame->data, audioFrame->nb_samples);
                double currentTime = audioFrame->pts *
                                     av_q2d(playInfo->pFormatContext->streams[playInfo->audioIndex]->time_base);//使用stream的时间基,frame的时间基是转成44100的,这里需要使用转之前的才是准确的
                decoderListener != nullptr ? decoderListener->onDecodeFinish(currentTime,
                                                                             currentBufferSize,
                                                                             audio_out_buffer)
                                           : void();
            }
        } else {
            logE("audio decode Iml ret:%d", ret);
        }
    }

    void AudioDecoder::addPacket(AVPacket avPacket) {
        packetQueue.push(avPacket);
    }
NS_KP_END

