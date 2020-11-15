//
// Created by lewis-v on 2020/9/6.
//

#include "BaseQueueController.h"

#include "../base/Macros.h"
#include "Log.h"

NS_KP_BEGIN

    BaseQueueController::BaseQueueController() {
        th = std::thread(&BaseQueueController::runIml, this);
//        th.detach();
    }

    BaseQueueController::~BaseQueueController() {

        stopSync();
    }

    bool BaseQueueController::isQueuePause() {
        return status == QUEUE_STATUS_PAUSE;
    }

    bool BaseQueueController::isQueueResume() {
        return status == QUEUE_STATUS_RESUME;
    }

    void BaseQueueController::runIml() {
        onQueueStart();
        while (isRun) {
            taskQueue.update();
            if (!isRun) {
                break;
            }
            if (isQueuePause()) {//如果是暂停的状态就直接等待下一次任务的触发
                taskQueue.waitNext();
                continue;
            }
            onQueueRun();
            int next = getWaitNextTIme();
            if (next >= 0) {
                taskQueue.waitNextWithMaxTime(next);
            } else {
                taskQueue.waitNext();
            }
        }
        onQueueStop();
        isRunFinish = true;
    }

    void BaseQueueController::queueResume() {
        taskQueue.postTask([&] {
            status = QUEUE_STATUS_RESUME;
            onQueueResume();
        });
    }

    void BaseQueueController::queuePause() {
        taskQueue.postTask([&] {
            status = QUEUE_STATUS_PAUSE;
            onQueuePause();
        });
    }

    void BaseQueueController::queueStop() {
        if (!isRun) return;
        isRun = false;
        taskQueue.postTask([&] {

        });

    }

    void BaseQueueController::stopSync() {
        isRun = false;
        taskQueue.clearAll();
        if (th.joinable()) {
            th.join();
        }
    }

NS_KP_END

