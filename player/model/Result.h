//
// Created by lewis-v on 2020/8/17.
//

#ifndef PLAYER_RESULT_H
#define PLAYER_RESULT_H

/**
 * 专门用来返回的对象,里面可以带错误信息
 */
#include <string>
#include "../base/Macros.h"

NS_KP_BEGIN
    template<typename T>
    class Result {
    public:
        int code = 0;
        std::string msg = "";
        T data;
        int extra = 0;

        bool isSuccess();
    };

    template<typename T>
    bool Result<T>::isSuccess() {
        return code == 0;
    }
NS_KP_END


#endif //PLAYER_RESULT_H
