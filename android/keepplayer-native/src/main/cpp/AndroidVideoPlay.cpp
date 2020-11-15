//
// Created by lewis-v on 2020/10/18.
//

extern "C" {
#include <libavutil/imgutils.h>
}

#include "AndroidVideoPlay.h"
#include "GLThreadHelper.h"
#include "../../../../../player/base/Config.h"

#ifdef RENDER_RGB
#include "../../../../../player/render/RGBGLRender.h"
#else

#include "../../../../../player/render/YUVGLRender.h"

#endif


NS_KP_BEGIN
    AndroidVideoPlay::AndroidVideoPlay(ParseResult *playInfo, long long key) {
        this->playInfo = playInfo;
        glThreadKey = key;
#ifdef RENDER_RGB
        targetWidth = playInfo->videoInfo->videoCodeContext->coded_width;
        targetWidth = playInfo->videoInfo->videoCodeContext->coded_height;
        swsContext = sws_getContext(playInfo->videoInfo->videoCodeContext->width,
                                    playInfo->videoInfo->videoCodeContext->height,
                                    playInfo->videoInfo->videoCodeContext->pix_fmt,
                                    targetWidth,//playInfo->videoInfo->videoCodeContext->width,
                                    targetHeight,//playInfo->videoInfo->videoCodeContext->height,
                                    FORMAT,
                                    SWS_BICUBIC,
                                    nullptr, nullptr, nullptr);
#endif
        if (playInfo->hasAudioStream()) {//有音频流才计算音视频间隔上限
            audioMaxDiffTime = 1000 / playInfo->videoInfo->fps * 1.3;//设置为相差大于1.3的帧时间才认为要等下次在渲染,这里刚好为1的话很有可能在下次来的时候时间就小于音频时间了
        }
        auto glThread = GLThreadHelper::getInstance()->getGLThread(glThreadKey);
        if (glThread) {
            glThread->setFps(playInfo->videoInfo->fps);
            glThread->setFrameDrawCallback([&] {
                return onDrawFrame();
            });
        }
    }

    AndroidVideoPlay::~AndroidVideoPlay() {
        flush();
        KP_SAFE_DELETE(render)
        if (swsContext != nullptr) {
            sws_freeContext(swsContext);
        }
    }

    void AndroidVideoPlay::resume() {
        auto glThread = GLThreadHelper::getInstance()->getGLThread(glThreadKey);
        if (glThread) {
            glThread->queueResume();
        }
    }

    void AndroidVideoPlay::pause() {
        auto glThread = GLThreadHelper::getInstance()->getGLThread(glThreadKey);
        if (glThread) {
            glThread->queuePause();
        }
    }

    void AndroidVideoPlay::stop() {
        auto glThread = GLThreadHelper::getInstance()->getGLThread(glThreadKey);
        if (glThread) {
            glThread->queueStop();
        }
        flush();
    }

    void AndroidVideoPlay::playFrame(double time, AVFrame *videoFrame) {
#ifdef RENDER_RGB
        std::unique_lock<std::mutex> lock(queueMutex);
        while (bufferQueue.size() > MAX_CACHE_AV_FRAME_SIZE) {
            queueCond.wait(lock);
        }
        auto videoScaleFrame = av_frame_alloc();
        av_image_alloc(videoScaleFrame->data, videoScaleFrame->linesize,
                       targetWidth,//playInfo->videoInfo->videoCodeContext->width,
                       targetHeight,//playInfo->videoInfo->videoCodeContext->height,
                       FORMAT, 16);
        auto result = sws_scale(swsContext, (const uint8_t *const *) videoFrame->data,
                                videoFrame->linesize, 0,
                                playInfo->videoInfo->videoCodeContext->height,
                                videoScaleFrame->data,
                                videoScaleFrame->linesize
        );
        logI("frame w:%d  h%d  cw:%d   ch:%d  result:%d linesizeSrc:%d  linesizeDst:%d targetW:%d  targetH:%d", playInfo->videoInfo->videoCodeContext->width,
             playInfo->videoInfo->videoCodeContext->height,
             playInfo->videoInfo->videoCodeContext->coded_width,
             playInfo->videoInfo->videoCodeContext->coded_height,
             result,
             videoFrame->linesize[0],
             videoScaleFrame->linesize[0],
             targetWidth,
             targetHeight
        );
        videoScaleFrame->width = targetWidth;
        videoScaleFrame->height = targetHeight;
        videoScaleFrame->linesize[0] = videoFrame->linesize[0];
        videoScaleFrame->linesize[1] = videoFrame->linesize[1];
        videoScaleFrame->linesize[2] = videoFrame->linesize[2];
        bufferQueue.push(videoScaleFrame);
#else
        std::unique_lock<std::mutex> lock(queueMutex);
        while (bufferQueue.size() > MAX_CACHE_AV_FRAME_SIZE) {
            queueCond.wait(lock);
        }
        auto* f = av_frame_clone(videoFrame);
        bufferQueue.emplace(f);
#endif
    }

    bool AndroidVideoPlay::onDrawFrame() {
        auto result = false;
        if (render == nullptr) {
#ifdef RENDER_RGB
            render = new RGBGLRender();
#else
            render = new YUVGLRender();
#endif
        }
        std::unique_lock<std::mutex> lock(queueMutex);
        if (!bufferQueue.empty()) {
            AVFrame *frame = nullptr;
            if (progressSync == nullptr) {//不做同步
                frame = bufferQueue.front();
            } else if (playInfo->hasAudioStream()) {//有音频流是需要参照音频的进度
                while (!bufferQueue.empty()) {
                    frame = bufferQueue.front();
                    double currentTime = frame->pts *
                                         av_q2d(playInfo->pFormatContext->streams[playInfo->videoIndex]->time_base);
                    auto audioProgress = progressSync->getProgress();
                    if (currentTime < audioProgress) {
                        av_frame_free(&frame);
                        frame = nullptr;
                        bufferQueue.pop();
                    }
                    else if (currentTime - audioProgress > audioMaxDiffTime) {//这帧超前音频太多,那就先不渲染,等下次循环再看看有没有到
                        return result;
                    } else {
                        break;
                    }
                }
            } else {//没有音频流的时候通过视频流来设置播放进度
                frame = bufferQueue.front();
                double currentTime = frame->pts *
                                     av_q2d(playInfo->pFormatContext->streams[playInfo->videoIndex]->time_base);
                progressSync->setProgress(currentTime);
            }
            if (frame == nullptr) {
                queueCond.notify_one();
                return result;
            }
            if (render != nullptr) {
                result = true;
                render->render(frame, frame->width,
                               frame->height);
            }
            bufferQueue.pop();
            av_frame_free(&frame);
            queueCond.notify_one();
        }
        return result;
    }

    void AndroidVideoPlay::flush() {
        std::unique_lock<std::mutex> lock(queueMutex);
        while (!bufferQueue.empty()) {
            av_frame_free(&bufferQueue.front());
            bufferQueue.pop();
        }
        queueCond.notify_one();
    }

NS_KP_END

