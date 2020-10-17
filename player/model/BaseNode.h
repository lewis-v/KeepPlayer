//
// Created by lewis-v on 2020/10/7.
//

#ifndef PLAYER_BASENODE_H
#define PLAYER_BASENODE_H


#include "../base/Macros.h"
#include "../base/Base.h"

NS_KP_BEGIN
    template<typename T>
    class BaseNode : public Base {
    public:
        T data;
        BaseNode<T> *next = nullptr;

    };
NS_KP_END


#endif //PLAYER_BASENODE_H
