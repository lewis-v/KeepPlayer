package com.lewis.keepplayer

import android.view.Surface

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

    fun seekTo(time: Long) {
        KeepPlayerNative.seekTo(instance, time)
    }

    fun release() {
        KeepPlayerNative.release(instance)
        instance = 0
    }

    fun setSurface(surface: Surface) {
        KeepPlayerNative.setSurface(instance, surface)
    }

    fun updateSurfaceSize(surface: Surface, width: Int, height: Int) {
        KeepPlayerNative.updateSurfaceSize(instance, surface, width, height)
    }
}