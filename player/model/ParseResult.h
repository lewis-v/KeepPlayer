//
// Created by lewis-v on 2020/8/17.
//

#ifndef PLAYER_PARSERESULT_H
#define PLAYER_PARSERESULT_H
extern "C" {
#include "../../includes/ffmpeg/libswresample/swresample.h"
#include "../../includes/ffmpeg/libavcodec/avcodec.h"
#include "../../includes/ffmpeg/libavformat/avformat.h"
};
#include "../base/Macros.h"

NS_KP_BEGIN
    class ParseAudioResult {
    public:
        ParseAudioResult() = default;
        ~ParseAudioResult();
        AVCodecContext *audioCodeContext = nullptr;
        AVCodec *pCodec = nullptr;
        SwrContext *swrCtx = nullptr;
        int channelCount = -1;
    };

    class ParseVideoResult {
    public:
        ParseVideoResult() = default;
        ~ParseVideoResult();
        AVCodecContext *videoCodeContext = nullptr;
        AVCodec *pCodec = nullptr;
        int fps = 30;
    };

    class ParseResult {
    public:
        ParseResult() = default;

        ~ParseResult();

        AVFormatContext *pFormatContext = nullptr;
        int videoIndex = -1;
        int audioIndex = -1;
        long long duration = 0;
        ParseVideoResult* videoInfo = nullptr;
        ParseAudioResult* audioInfo = nullptr;

        bool hasVideoStream();

        bool hasAudioStream();

    private:
        void release();

    };
NS_KP_END


#endif //PLAYER_PARSERESULT_H
