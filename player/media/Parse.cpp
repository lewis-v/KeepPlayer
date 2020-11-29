//
// Created by lewis-v on 2020/8/17.
//

#include "Parse.h"
#include "../base/Macros.h"
#include "../base/Log.h"
#include "../model/ParseResult.h"

extern "C" {
#include "../../includes/ffmpeg/libavformat/avformat.h"
#include "../../includes/ffmpeg/libavutil/frame.h"
#include "../../includes/ffmpeg/libswscale/swscale.h"
#include "../../includes/ffmpeg/libavutil/imgutils.h"
#include "../../includes/ffmpeg/libswresample/swresample.h"
};
NS_KP_BEGIN

    Result<ParseResult *> Parse::parseSource(const std::string& sourcePath) {
        auto result = Result<ParseResult *>();
        result.data = new ParseResult();
        result.data->pFormatContext = avformat_alloc_context();
        int errCode = 0;
        AVDictionary *opts = nullptr;
        if (sourcePath.find("http") == 0) {
            av_dict_set(&opts, "rtsp_transport", "tcp", 0); //设置tcp or udp，默认一般优先tcp再尝试udp
            av_dict_set(&opts, "stimeout", "7000000", 0);//设置超时7秒
            logI("use http");
        } else if (sourcePath.find("rtsp") == 0) {
            av_dict_set(&opts, "timeout", "7000000", 0);//设置超时7秒
            logI("use rtsp");
        }
        //todo opts设置
        if ((errCode = avformat_open_input(&(result.data->pFormatContext), sourcePath.c_str(),
                                           nullptr,
                                           &opts)) < 0) {
            logE("avformat_open_input err %d path:%s", errCode, sourcePath.c_str());
            delete result.data;
            if (opts != nullptr) {
                av_dict_free(&opts);
            }
            result.data = nullptr;
            result.code = -1;
            result.msg = "avformat_open_input err";
            return result;
        }
        if (opts != nullptr) {
            av_dict_free(&opts);
        }
        if ((errCode = avformat_find_stream_info(result.data->pFormatContext, nullptr)) < 0) {
            logE("avformat_find_stream_info err %d", errCode);
            delete result.data;
            result.data = nullptr;
            result.code = -1;
            result.msg = "avformat_find_stream_info err";
            return result;
        }
        for (int i = 0; i < result.data->pFormatContext->nb_streams; ++i) {
            AVMediaType mediaType = result.data->pFormatContext->streams[i]->codecpar->codec_type;
            if (mediaType == AVMEDIA_TYPE_VIDEO) {//视频流
                logI("find video stream %d", i);
                result.data->videoIndex = i;
            } else if (mediaType == AVMEDIA_TYPE_AUDIO) {//音频流
                logI("find audio stream %d", i);
                result.data->audioIndex = i;
            } else {
                logW("find stream index %d this stream is not support type %d", i, mediaType);
            }
        }
        if (result.data->hasAudioStream()) {
            auto initResult = initAudioStream(result.data, result.data->audioIndex);
            if (initResult.code != 0) {
                result.code = initResult.code;
                result.msg = initResult.msg;
                return result;
            }
            logI("init audio finish");
        } else {
            logI("no audio stream");
        }
        if (result.data->hasVideoStream()) {
            auto initResult = initVideoStream(result.data, result.data->videoIndex);
            if (initResult.code != 0) {
                result.code = initResult.code;
                result.msg = initResult.msg;
                return result;
            }
            logI("init video finish");
        } else {
            logI("no video stream");
        }
        result.data->duration = result.data->pFormatContext->duration / AV_TIME_BASE;

//        av_dump_format(result.data->pFormatContext, 0, sourcePath.c_str(), false);
        return result;
    }

    Result<bool > Parse::initAudioStream(ParseResult *result, int index) {
        auto voidResult = Result<bool>();
        auto audioResult = new ParseAudioResult();
        audioResult->audioCodeContext = avcodec_alloc_context3(nullptr);
        int errCode = 0;
        if ((errCode = avcodec_parameters_to_context(audioResult->audioCodeContext,
                                                     result->pFormatContext->streams[index]->codecpar)) !=
            0) {
            logE("avcodec_parameters_to_context is fail errCode:%d", errCode);
            delete audioResult;
            voidResult.code = errCode;
            voidResult.msg = "avcodec_parameters_to_context err";
            return voidResult;
        }
        audioResult->pCodec = avcodec_find_decoder(
                result->pFormatContext->streams[index]->codecpar->codec_id);
        if (audioResult->pCodec == nullptr) {
            logE("avcodec_find_decoder is return null");
            delete audioResult;
            voidResult.code = errCode;
            voidResult.msg = "avcodec_find_decoder err";
            return voidResult;
        }
        if ((errCode = avcodec_open2(audioResult->audioCodeContext, audioResult->pCodec, nullptr)) <
            0) {
            logE("avcodec_open2 errCode:%d", errCode);
            delete audioResult;
            voidResult.code = errCode;
            voidResult.msg = "avcodec_open2 err";
            return voidResult;
        }
        av_codec_set_pkt_timebase(audioResult->audioCodeContext,
                                  result->pFormatContext->streams[index]->time_base);
        //音频转换配置
        audioResult->swrCtx = swr_alloc();
        //重采样设置选项
        //输入的采样格式
        enum AVSampleFormat in_sample_fmt = audioResult->audioCodeContext->sample_fmt;
        //输出的采样格式 16bit PCM
        enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
        //输入的采样率
        int in_sample_rate = audioResult->audioCodeContext->sample_rate;
        //输入的声道布局
        uint64_t in_ch_layout =
                audioResult->audioCodeContext->channel_layout == 0 ? av_get_default_channel_layout(
                        audioResult->audioCodeContext->channels)
                                                                   : audioResult->audioCodeContext->channel_layout;
        //输出的声道布局
        uint64_t out_ch_layout = in_ch_layout;

        swr_alloc_set_opts(audioResult->swrCtx, out_ch_layout, out_sample_fmt, in_sample_rate,
                           in_ch_layout, in_sample_fmt,
                           in_sample_rate, 0, nullptr);
        if ((errCode = swr_init(audioResult->swrCtx)) < 0) {
            logE("swr_init errCode:%d", errCode);
            delete audioResult;
            voidResult.code = errCode;
            voidResult.msg = "swr_init err";
            return voidResult;
        }
        audioResult->channelCount = av_get_channel_layout_nb_channels(out_ch_layout);
        result->audioInfo = audioResult;
        return voidResult;
    }

    Result<bool> Parse::initVideoStream(ParseResult *result, int index) {
        auto voidResult = Result<bool>();
        auto videoInfo = new ParseVideoResult();
        videoInfo->videoCodeContext = avcodec_alloc_context3(nullptr);
        int errCode = 0;
        if ((errCode = avcodec_parameters_to_context(videoInfo->videoCodeContext,
                                                     result->pFormatContext->streams[index]->codecpar)) !=
            0) {
            logE("avcodec_parameters_to_context is fail errCode:%d", errCode);
            delete videoInfo;
            voidResult.code = errCode;
            voidResult.msg = "avcodec_parameters_to_context err";
            return voidResult;
        }
        videoInfo->pCodec = avcodec_find_decoder(
                result->pFormatContext->streams[index]->codecpar->codec_id);
        if (videoInfo->pCodec == nullptr) {
            logE("avcodec_find_decoder is return null");
            delete videoInfo;
            voidResult.code = errCode;
            voidResult.msg = "avcodec_find_decoder err";
            return voidResult;
        }
        if ((errCode = avcodec_open2(videoInfo->videoCodeContext, videoInfo->pCodec, nullptr)) <
            0) {
            logE("avcodec_open2 errCode:%d", errCode);
            delete videoInfo;
            voidResult.code = errCode;
            voidResult.msg = "avcodec_open2 err";
            return voidResult;
        }
        av_codec_set_pkt_timebase(videoInfo->videoCodeContext,
                                  result->pFormatContext->streams[index]->time_base);
        //todo fps获取
        result->videoInfo = videoInfo;
        return voidResult;
    }
NS_KP_END

