//
// Created by lewis-v on 2020/8/17.
//

#ifndef PLAYER_INIT_H
#define PLAYER_INIT_H


#include <string>
#include "../base/Macros.h"
#include "../model/ParseResult.h"
#include "../model/Result.h"

NS_KP_BEGIN

    class Parse {
    public:
        static Result<ParseResult *> parseSource(const std::string& sourcePath);

        static Result<bool> initAudioStream(ParseResult *result, int index);

        static Result<bool> initVideoStream(ParseResult *result, int index);
    };
NS_KP_END


#endif //PLAYER_PARSE_H
