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
        auto glThread = GLThreadHelper::getInstance()->getGLThread(glThreadKey);
        if (glThread) {
            glThread->setFps(30);
            glThread->setFrameDrawCallback([&] {
                return onDrawFrame();
            });
        }
    }

    AndroidVideoPlay::~AndroidVideoPlay() {
        while (!bufferQueue.empty()) {
            av_frame_free(&bufferQueue.front());
            bufferQueue.pop();
        }
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
            auto *frame = bufferQueue.front();
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

NS_KP_END

