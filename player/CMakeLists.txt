cmake_minimum_required(VERSION 3.15)
project(player)

set(CMAKE_CXX_STANDARD 11)

add_executable(player
        main.cpp
        base/Config.h
        base/Macros.h
        base/Log.cpp
        base/Log.h
        KeepPlayer.cpp
        KeepPlayer.h
        base/TaskQueue.cpp
        base/TaskQueue.h
        media/Parse.cpp
        media/Parse.h
        model/ParseResult.cpp
        model/ParseResult.h
        model/Result.cpp
        model/Result.h
        PlayListener.cpp
        PlayListener.h
        base/Base.cpp
        base/Base.h
        media/PacketRead.cpp
        media/PacketRead.h
        media/PacketQueue.cpp
        media/PacketQueue.h
        media/AudioDecoder.cpp
        media/AudioDecoder.h
        base/BaseQueueController.cpp
        base/BaseQueueController.h
        media/AudioPlay.cpp
        media/AudioPlay.h
        model/AudioBuffer.cpp
        model/AudioBuffer.h
        model/BaseNode.cpp
        model/BaseNode.h media/VideoDecoder.cpp media/VideoDecoder.h media/VideoPlay.cpp media/VideoPlay.h media/IDecoder.h media/IDecoder.cpp)

include_directories(${CMAKE_SOURCE_DIR}/../includes/ffmpeg)