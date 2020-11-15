//
// Created by lewis-v on 2020/10/17.
//

extern "C" {
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
}

#include "VideoDecoder.h"

#include "../base/Macros.h"
#include "../base/Log.h"


NS_KP_BEGIN


    VideoDecoder::VideoDecoder(ParseResult *playInfo) : IVideoDecoder(playInfo) {
        videoFrame = av_frame_alloc();

    }

    VideoDecoder::~VideoDecoder() {
        if (videoFrame != nullptr) {
            av_frame_free(&videoFrame);
            videoFrame = nullptr;
        }
    }

    void VideoDecoder::decodeIml(AVPacket &avPacket) {
        auto ret = avcodec_send_packet(playInfo->videoInfo->videoCodeContext, &avPacket);
        if (ret == 0) {
            while (avcodec_receive_frame(playInfo->videoInfo->videoCodeContext, videoFrame) == 0) {
                double currentTime = videoFrame->pts *
                                     av_q2d(playInfo->pFormatContext->streams[playInfo->videoIndex]->time_base);//使用stream的时间基,frame的时间基是转成44100的,这里需要使用转之前的才是准确的
                if (decoderListener != nullptr ) {
                    decoderListener->onDecodeFinish(currentTime, videoFrame);
                }
                av_frame_unref(videoFrame);
            }
        } else {
            logE("video decode Iml ret:%d", ret);
        }
        av_packet_unref(&avPacket);
    }


NS_KP_END