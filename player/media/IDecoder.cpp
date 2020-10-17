//
// Created by lewis-v on 2020/10/17.
//

#include "IDecoder.h"
#include "../base/Log.h"

NS_KP_BEGIN

    void SimpleDecoder::resume() {
        queueResume();
    }

    void SimpleDecoder::pause() {
        queuePause();
    }

    void SimpleDecoder::stop() {
        queueStop();
        packetQueue.clear();
    }

    void SimpleDecoder::onQueueResume() {

    }

    void SimpleDecoder::onQueuePause() {

    }

    void SimpleDecoder::onQueueStop() {

    }

    void SimpleDecoder::onQueueRun() {
        if (playInfo == nullptr) {
            logI("AudioDecoder onQueueRun playInfo is null");
            return;
        }
        AVPacket avPacket{};
        auto isSuccess = packetQueue.pop(&avPacket);
        if (!isSuccess) {
            logW("AudioDecoder onQueueRun pop not success");
            return;
        }
        decodeIml(avPacket);
    }

    void SimpleDecoder::addPacket(AVPacket avPacket) {
        packetQueue.push(avPacket);
    }

    SimpleDecoder::SimpleDecoder(ParseResult *playInfo) {
        this->playInfo = playInfo;
    }

    SimpleDecoder::~SimpleDecoder() {
        packetQueue.clear();
        stopSync();
    }

NS_KP_END