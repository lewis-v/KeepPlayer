package com.lewis.keepplayer

object KeepPlayerNative {
    init {
        System.loadLibrary("KeepPlayer")
        init()
    }

    private external fun init()

    external fun newInstance(): Long

    external fun setPath(instance: Long, path: String)

    external fun prepare(instance: Long)

    external fun start(instance: Long)

    external fun resume(instance: Long)

    external fun pause(instance: Long)

    external fun stop(instance: Long)

    external fun reset(instance: Long)

    external fun release(instance: Long)

    external fun test()


}