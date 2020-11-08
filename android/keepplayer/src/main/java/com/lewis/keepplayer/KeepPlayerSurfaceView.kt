package com.lewis.keepplayer

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

class KeepPlayerSurfaceView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : SurfaceView(context, attrs, defStyleAttr), SurfaceHolder.Callback {

    init {
        holder.addCallback(this)
    }

    var surfaceListener: ISurfaceListener? = null

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        holder?.let {
            surfaceListener?.onChange(it.surface, width, height)
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        holder?.let {
            surfaceListener?.onDestroy(it.surface)
        }
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        holder?.let {
            surfaceListener?.onCreate(it.surface, width, height)
        }
    }
}