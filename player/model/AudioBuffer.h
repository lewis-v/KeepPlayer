//
// Created by lewis-v on 2020/10/7.
//

#ifndef PLAYER_AUDIOBUFFER_H
#define PLAYER_AUDIOBUFFER_H


#include <cstdlib>
#include "../base/Macros.h"
#include "../base/Base.h"

NS_KP_BEGIN
    class AudioBuffer : public Base {
    public:
        double time;
        int size;
        uint8_t *data = nullptr;

        ~AudioBuffer() {
            if (data != nullptr) {
                free(data);
            }
        }
    };
NS_KP_END


#endif //PLAYER_AUDIOBUFFER_H
