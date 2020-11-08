package com.lewis.keepplayer

import android.view.Surface

interface ISurfaceListener {
    fun onCreate(surface: Surface, width: Int, height: Int)

    fun onChange(surface: Surface, width: Int, height: Int)

    fun onDestroy(surface: Surface)
}