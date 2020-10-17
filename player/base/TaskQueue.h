//
// Created by lewis-v on 2020/8/17.
//

#ifndef PLAYER_TASKQUEUE_H
#define PLAYER_TASKQUEUE_H


#include "../base/Macros.h"
#include <mutex>
#include <vector>
#include <condition_variable>
#include <functional>

NS_KP_BEGIN
    class TaskQueue {
    public:
        TaskQueue() = default;

        ~TaskQueue();

        void postTask(const std::function<void()> &task);

        void init();

        void clearAll();

        void update();

        void waitNext();

        void waitNextWithMaxTime(int maxTime);

        void notifyAll();

        bool isEmpty();

        int size();

    private:
        std::vector<std::function<void()>> _tasks;
        std::mutex _performMutex;
        std::condition_variable _performCond;

    };
NS_KP_END


#endif //PLAYER_TASKQUEUE_H
