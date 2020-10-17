//
// Created by lewis-v on 2020/8/31.
//

#include <queue>
#include "PacketQueue.h"

#include "../base/Macros.h"
#include "../base/Log.h"

NS_KP_BEGIN
    PacketQueue::~PacketQueue() {
        std::unique_lock<std::mutex> lck(queueMutex);
        while (!queue.empty()) {
            av_packet_unref(&queue.front());
            queue.pop();
        }
        cond.notify_all();
    }

    void PacketQueue::push(AVPacket &packet) {
        std::unique_lock<std::mutex> lck(queueMutex);
        queue.push(packet);
        cond.notify_one();
    }

    bool PacketQueue::pop(AVPacket* packet) {
        std::unique_lock<std::mutex> lck(queueMutex);
        if (queue.empty()) {
            cond.wait(lck);
        }
        if (queue.empty()) return false;
        auto result = queue.front();
        queue.pop();
        av_packet_move_ref(packet, &result);
        return true;
    }

    void PacketQueue::clear() {
        std::unique_lock<std::mutex> lck(queueMutex);
        while (!queue.empty()) {
            av_packet_unref(&queue.front());
            queue.pop();
        }
        cond.notify_all();
    }

    int PacketQueue::size() {
        return queue.size();
    }


NS_KP_END

