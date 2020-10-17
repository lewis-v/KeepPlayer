//
// Created by lewis-v on 2020/8/17.
//

#include "TaskQueue.h"

#include "../base/Macros.h"

NS_KP_BEGIN

    void TaskQueue::init() {
        update();
    }

    void TaskQueue::postTask(const std::function<void()> &task) {
        std::unique_lock<std::mutex> lck(_performMutex);
        _tasks.push_back(task);
        _performCond.notify_one();
    }

    void TaskQueue::clearAll() {
        std::unique_lock<std::mutex> lck(_performMutex);
        _tasks.clear();
        _performCond.notify_all();

    }

    bool TaskQueue::isEmpty() {
        return _tasks.empty();
    }

    void TaskQueue::update() {
        if (!_tasks.empty()) {
            _performMutex.lock();
            auto temp = _tasks;
            _tasks.clear();
            _performMutex.unlock();
            for (const auto &function : temp) {
                function();
            }
        }
    }

    void TaskQueue::waitNext() {
        std::unique_lock<std::mutex> lck(_performMutex);
        if (_tasks.empty()) {
            _performCond.wait(lck);
        }
    }

    void TaskQueue::waitNextWithMaxTime(int maxTime) {
        std::unique_lock<std::mutex> lck(_performMutex);
        if (_tasks.empty()) {
            _performCond.wait_for(lck, std::chrono::milliseconds(maxTime));
        }
    }

    TaskQueue::~TaskQueue() {
        clearAll();
    }

    void TaskQueue::notifyAll() {
        _performCond.notify_all();
    }

    int TaskQueue::size() {
        return _tasks.size();
    }
NS_KP_END