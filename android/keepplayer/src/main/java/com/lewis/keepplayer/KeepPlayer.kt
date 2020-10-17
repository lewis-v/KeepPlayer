package com.lewis.keepplayer

class KeepPlayer {

    private var instance = KeepPlayerNative.newInstance()

    fun setPath(path: String) {
        KeepPlayerNative.setPath(instance, path)
    }

    fun prepare() {
        KeepPlayerNative.prepare(instance)
    }

    fun start() {
        KeepPlayerNative.start(instance)
    }

    fun resume() {
        KeepPlayerNative.resume(instance)
    }

    fun pause() {
        KeepPlayerNative.pause(instance)
    }

    fun stop() {
        KeepPlayerNative.stop(instance)
    }

    fun reset() {
        KeepPlayerNative.reset(instance)
    }

    fun release() {
        KeepPlayerNative.release(instance)
        instance = 0
    }
}