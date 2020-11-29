//
// Created by lewis on 2020/8/17.
//

#include "KeepPlayer.h"

#include "base/Macros.h"
#include "media/Parse.h"
#include "base/Log.h"
#include "../android/keepplayer-native/src/main/cpp/AndroidAudioPlay.h"
#include "../android/keepplayer-native/src/main/cpp/AndroidVideoPlay.h"

NS_KP_BEGIN


    void KeepPlayer::setPath(const std::string &path) {
        this->path = path;
    }

    void KeepPlayer::prepare() {
        logI("prepare start %s", path.c_str());
        if (parseResult != nullptr) {
            logW("prepare fail parseResult is not null");
            return;
        }
        if (!path.empty()) {
            auto result = Parse::parseSource(path);
            if (result.isSuccess()) {
                logI("prepare success path:%s", path.c_str());
                parseResult = result.data;
                prepareFinish();
                prepareListener != nullptr ? prepareListener->onPrepare(this) : void();
            } else {
                errorListener != nullptr ? errorListener->onError(this, result.code, result.extra)
                                         : void();
            }
        } else {
            logE("prepare but path is empty");
        }
    }

    void KeepPlayer::prepareAsync() {
        //todo 异步处理
        prepare();
    }

    void KeepPlayer::start() {
        resume();
    }

    void KeepPlayer::resume() {
        if (read != nullptr) {
            read->resume();
        }
        if (audioDecoder != nullptr) {
            audioDecoder->resume();
        }
        if (audioPlay != nullptr) {
            audioPlay->resume();
        }
        if (videoDecoder != nullptr) {
            videoDecoder->resume();
        }
        if (videoPlay != nullptr) {
            videoPlay->resume();
        }
    }

    void KeepPlayer::pause() {
        if (read != nullptr) {
            read->pause();
        }
        if (audioDecoder != nullptr) {
            audioDecoder->pause();
        }
        if (audioPlay != nullptr) {
            audioPlay->pause();
        }
        if (videoDecoder != nullptr) {
            videoDecoder->pause();
        }
        if (videoPlay != nullptr) {
            videoPlay->pause();
        }
    }

    void KeepPlayer::stop() {
        if (read != nullptr) {
            read->stop();
        }
        if (audioDecoder != nullptr) {
            audioDecoder->stop();
        }
        if (audioPlay != nullptr) {
            audioPlay->stop();
        }
        if (videoDecoder != nullptr) {
            videoDecoder->stop();
        }
        if (videoPlay != nullptr) {
            videoPlay->stop();
        }
    }

    void KeepPlayer::reset() {
        path = "";
        KP_SAFE_DELETE(read)
        KP_SAFE_DELETE(audioDecoder)
        KP_SAFE_DELETE(audioPlay)
        KP_SAFE_DELETE(videoDecoder)
        KP_SAFE_DELETE(videoPlay)
        KP_SAFE_DELETE(parseResult)
    }

    void KeepPlayer::seek(long long time) {
        if (read != nullptr) {
            read->seek(time);
        }
    }

    void KeepPlayer::prepareFinish() {
        if (parseResult == nullptr) {
            logE("start but parseResult is null");
            return;
        }
        if (read != nullptr) {
            logE("start but read is not null");
            return;
        }
        read = new PacketRead(parseResult);
        progressSync = new ProgressSync();
        if (parseResult->hasAudioStream()) {
            audioDecoder = new AudioDecoder(parseResult);
            audioPlay = new AndroidAudioPlay(parseResult);
            audioDecoder->setAudioDecoderListener(
                    new SimpleAudioDecoderListener([&](double time, int dataSize, uint8_t *data) {
                        if (audioPlay == nullptr) {
                            return;
                        }
                        audioPlay->playFrame(time, dataSize, data);
                    }));
            audioPlay->setProgressSync(progressSync);

        }
        if (parseResult->hasVideoStream()) {
            videoDecoder = new VideoDecoder(parseResult);
            videoPlay = new AndroidVideoPlay(parseResult, (long long) this);
            videoDecoder->setVideoDecoderListener(
                    new SimpleVideoDecoderListener([&](double time, AVFrame *videoFrame) {
                        if (videoPlay == nullptr) {
                            return;
                        }
                        videoPlay->playFrame(time, videoFrame);
                    }));
            videoPlay->setProgressSync(progressSync);
        }

        read->setListener(new SimplePacketReadListener(
                [&](AVPacket avPacket) {
                    //video read
                    if (videoDecoder == nullptr) {
                        return false;
                    }
                    videoDecoder->addPacket(avPacket);
                    return true;
                },
                [&](AVPacket avPacket) {
                    //audio read
                    if (audioDecoder == nullptr) {
                        return false;
                    }
                    audioDecoder->addPacket(avPacket);
                    return true;
                },
                [&] {
                    //flush
                    if (audioDecoder != nullptr) {
                        audioDecoder->flush();
                    }
                    if (audioPlay != nullptr) {
                        audioPlay->flush();
                    }
                    if (videoDecoder != nullptr) {
                        videoDecoder->flush();
                    }
                    if (videoPlay != nullptr) {
                        videoPlay->flush();
                    }
                },
                [&] (double currentReadTime){
                    //read progress

                }
        ));


    }

    KeepPlayer::~KeepPlayer() {
        KP_SAFE_DELETE(read)
        KP_SAFE_DELETE(audioDecoder)
        KP_SAFE_DELETE(audioPlay)
        KP_SAFE_DELETE(videoDecoder)
        KP_SAFE_DELETE(videoPlay)

        KP_SAFE_DELETE(parseResult)

        KP_SAFE_DELETE(progressSync)

        KP_SAFE_DELETE(prepareListener)
        KP_SAFE_DELETE(completionListener)
        KP_SAFE_DELETE(bufferListener)
        KP_SAFE_DELETE(seekCompleteListener)
        KP_SAFE_DELETE(errorListener)
        KP_SAFE_DELETE(infoListener)
    }

NS_KP_END
