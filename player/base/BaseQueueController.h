//
// Created by lewis-v on 2020/9/6.
//

#ifndef PLAYER_BASEQUEUECONTROLLER_H
#define PLAYER_BASEQUEUECONTROLLER_H


#include <thread>
#include "../base/Macros.h"
#include "Base.h"
#include "TaskQueue.h"

NS_KP_BEGIN
    class BaseQueueController : public Base {
    public:
        static const int QUEUE_STATUS_PAUSE = 0;
        static const int QUEUE_STATUS_RESUME = 1;

        BaseQueueController();

        ~BaseQueueController();

        virtual void queueResume();

        virtual void queuePause();

        virtual void queueStop();

        virtual bool isQueuePause();

        virtual bool isQueueResume();

        virtual std::string getTag() {
            return "";
        }

    protected:
        TaskQueue taskQueue;

        virtual void onQueueResume() {};

        virtual void onQueuePause() {};

        virtual void onQueueStart() {};

        virtual void onQueueRun() {};

        virtual void onQueueStop() {};

        void stopSync();

        virtual int getWaitNextTIme() {
            return 0;
        }

    private:
        std::thread th;
        int status = QUEUE_STATUS_PAUSE;
        bool isRun = true;
        bool isRunFinish = false;

        void runIml();
    };
NS_KP_END


#endif //PLAYER_BASEQUEUECONTROLLER_H
