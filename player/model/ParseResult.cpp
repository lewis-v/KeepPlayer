//
// Created by lewis-v on 2020/8/17.
//
extern "C" {
#include "../../includes/ffmpeg/libavcodec/avcodec.h"
};
#include "ParseResult.h"
#include "../base/Macros.h"

NS_KP_BEGIN

    void ParseResult::release() {
        if (pFormatContext != nullptr) {
            avformat_close_input(&pFormatContext);
            avformat_free_context(pFormatContext);
        }
        KP_SAFE_DELETE(audioInfo)
        KP_SAFE_DELETE(videoInfo)
    }

    ParseResult::~ParseResult() {
        release();
    }

    bool ParseResult::hasVideoStream() {
        return videoIndex != -1;
    }

    bool ParseResult::hasAudioStream() {
        return audioIndex != -1;
    }

    ParseAudioResult::~ParseAudioResult() {
        if (audioCodeContext != nullptr) {
            avcodec_close(audioCodeContext);
            avcodec_free_context(&audioCodeContext);
            audioCodeContext = nullptr;
        }
        if (swrCtx != nullptr) {
            swr_free(&swrCtx);
            swrCtx = nullptr;
        }
    }

    ParseVideoResult::~ParseVideoResult() {
        if (videoCodeContext != nullptr) {
            avcodec_close(videoCodeContext);
            avcodec_free_context(&videoCodeContext);
            videoCodeContext = nullptr;
        }
    }

NS_KP_END



