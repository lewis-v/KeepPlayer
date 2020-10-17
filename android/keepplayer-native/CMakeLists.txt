cmake_minimum_required(VERSION 3.10.2)

MESSAGE("current dir ${CMAKE_SOURCE_DIR}")
set(KEEP_DIR ${CMAKE_SOURCE_DIR}/../../player)

#跨平台的项目代码
list(APPEND SRC_LIST
        ${KEEP_DIR}/KeepPlayer.h
        ${KEEP_DIR}/KeepPlayer.cpp
        ${KEEP_DIR}/PlayListener.h
        ${KEEP_DIR}/PlayListener.cpp
        ${KEEP_DIR}/base/Log.cpp
        ${KEEP_DIR}/base/Log.h
        ${KEEP_DIR}/base/Base.cpp
        ${KEEP_DIR}/base/Base.h
        ${KEEP_DIR}/base/TaskQueue.cpp
        ${KEEP_DIR}/base/TaskQueue.h
        ${KEEP_DIR}/base/BaseQueueController.cpp
        ${KEEP_DIR}/base/BaseQueueController.h
        ${KEEP_DIR}/base/Config.h
        ${KEEP_DIR}/base/Macros.h
        ${KEEP_DIR}/media/Parse.h
        ${KEEP_DIR}/media/Parse.cpp
        ${KEEP_DIR}/media/AudioDecoder.h
        ${KEEP_DIR}/media/AudioDecoder.cpp
        ${KEEP_DIR}/media/PacketRead.h
        ${KEEP_DIR}/media/PacketRead.cpp
        ${KEEP_DIR}/media/PacketQueue.h
        ${KEEP_DIR}/media/PacketQueue.cpp
        ${KEEP_DIR}/media/AudioPlay.cpp
        ${KEEP_DIR}/media/AudioPlay.h
        ${KEEP_DIR}/media/VideoPlay.cpp
        ${KEEP_DIR}/media/VideoPlay.h
        ${KEEP_DIR}/media/VideoDecoder.cpp
        ${KEEP_DIR}/media/VideoDecoder.h
        ${KEEP_DIR}/media/IDecoder.h
        ${KEEP_DIR}/media/IDecoder.cpp
        ${KEEP_DIR}/model/ParseResult.h
        ${KEEP_DIR}/model/ParseResult.cpp
        ${KEEP_DIR}/model/Result.h
        ${KEEP_DIR}/model/Result.cpp
        ${KEEP_DIR}/model/AudioBuffer.cpp
        ${KEEP_DIR}/model/AudioBuffer.h
        ${KEEP_DIR}/model/BaseNode.cpp
        ${KEEP_DIR}/model/BaseNode.h
        )

set(KEEP_ANDROID_DIR ${CMAKE_SOURCE_DIR}/src/main/cpp)
list(APPEND SRC_LIST
        ${KEEP_ANDROID_DIR}/JniIml.cpp
        ${KEEP_ANDROID_DIR}/AndroidAudioPlay.cpp
        ${KEEP_ANDROID_DIR}/AndroidAudioPlay.h
        ${KEEP_ANDROID_DIR}/KPAndroidLog.cpp
        ${KEEP_ANDROID_DIR}/KPAndroidLog.h
)

find_library(
        log-lib
        log)
list(
        APPEND LINK_LIB
        KeepPlayer
        OpenSLES
        android
        ${log-lib}
)



set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../keepplayer/libs/${ANDROID_ABI}/)

add_library(libffmpeg
        SHARED
        IMPORTED)
set_target_properties(libffmpeg
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/cpp/ffmpeg/${ANDROID_ABI}/libijkffmpeg.so)
include_directories(${CMAKE_SOURCE_DIR}/../../includes/ffmpeg)
list(
        APPEND LINK_LIB
        libffmpeg
)


add_library(
        KeepPlayer
        SHARED
        ${SRC_LIST}
)



target_link_libraries(
        ${LINK_LIB})