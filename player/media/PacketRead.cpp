//
// Created by lewis-v on 2020/8/26.
//

#include "PacketRead.h"

#include "../base/Macros.h"
#include "../base/Log.h"

extern "C" {
#include "../../includes/ffmpeg/libavutil/mathematics.h"
#include "../../includes/ffmpeg/libavutil/avutil.h"
};
NS_KP_BEGIN
    static const BUFFER_ENOUGH_QUEUE_SIZE = 100;//等待足够的buffer才算够

    void PacketRead::resume() {
        queueResume();
    }

    void PacketRead::pause() {
        queuePause();
    }

    void PacketRead::seek(long long time) {
        taskQueue.postTask([&] {
            if (playInfo != nullptr) {
                readListener != nullptr ? readListener->onFlush() : void();
                auto pos = KP_MS_FF_TIME(time);
                auto ret = avformat_seek_file(playInfo->pFormatContext, -1, INT64_MIN, pos,
                                              INT64_MAX, 0);
                logI("seek time %lld  pos %lld", time, pos);
                if (ret < 0) {
                    logE("seek fail ret:%d time:%lld", ret, time);
                } else {
                    isComplete = false;
                }
            }
        });
    }

    void PacketRead::onQueueRun() {
        if (playInfo == nullptr) {
            return;
        }
        if (!isComplete) {
            readIml();
        }
    }

    void PacketRead::readIml() {
        int ret = av_read_frame(playInfo->pFormatContext, &avPacket);
        if (ret < 0) {
            switch (ret) {
                case AVERROR_EXIT://退出操作

                    break;
                case AVERROR_EOF://读到结尾了
                    logI("read complete");
                    isComplete = true;
                    avPacket.data = nullptr;
                    avPacket.size = 0;
                    //丢data为null的packet去刷新ffmpeg内部缓存
                    if (playInfo->hasVideoStream()) {
                        avPacket.stream_index = playInfo->videoIndex;
                        notifyIml();
                    }
                    if (playInfo->hasAudioStream()) {
                        avPacket.stream_index = playInfo->audioIndex;
                        notifyIml();
                    }
                    break;
                default://其他错误

                    break;
            }
            return;
        }
        notifyIml();
        if (isComplete) {
            audioReadTime = playInfo->duration;
            videoReadTime = playInfo->duration;
        }
        if (readListener != nullptr) {
            readListener->onReadProgress(std::min(audioReadTime, videoReadTime));
        }
    }

    PacketRead::~PacketRead() {
        stop();
        KP_SAFE_DELETE(readListener)
    }

    bool PacketRead::isPause() {
        return isQueuePause();
    }

    bool PacketRead::isResume() {
        return isQueueResume();
    }

    void PacketRead::onQueueResume() {
        av_read_play(playInfo->pFormatContext);
    }

    void PacketRead::onQueuePause() {
        av_read_pause(playInfo->pFormatContext);
    }

    void PacketRead::stop() {
        queueStop();
    }

    void PacketRead::onQueueStop() {
        //todo clear
    }

    void PacketRead::notifyIml() {
        if (avPacket.stream_index == playInfo->videoIndex) {
            videoReadTime = av_q2d(playInfo->pFormatContext->streams[playInfo->videoIndex]->time_base) * avPacket.dts;
            if (readListener == nullptr || !readListener->onReadVideo(avPacket)) {
                av_packet_unref(&avPacket);
            }
        } else if (avPacket.stream_index == playInfo->audioIndex) {
            audioReadTime = av_q2d(playInfo->pFormatContext->streams[playInfo->audioIndex]->time_base) * avPacket.dts;
            if (readListener == nullptr || !readListener->onReadAudio(avPacket)) {
                av_packet_unref(&avPacket);
            }
        } else {
            av_packet_unref(&avPacket);
        }
    }

    int PacketRead::getWaitNextTIme() {
        if (isComplete) {
            return -1;
        }
        return BaseQueueController::getWaitNextTIme();
    }

    PacketRead::PacketRead(ParseResult *playInfo, IPacketReadListener *readListener): playInfo(playInfo) {
        setListener(readListener);
        if (!playInfo->hasAudioStream()) {
            audioReadTime = playInfo->duration;
        }
        if (!playInfo->hasVideoStream()) {
            videoReadTime = playInfo->duration;
        }
    }

NS_KP_END
