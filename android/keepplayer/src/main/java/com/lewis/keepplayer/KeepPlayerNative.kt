package com.lewis.keepplayer

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES30
import android.opengl.GLUtils
import android.util.Log
import android.view.Surface
import java.lang.Exception
import java.util.concurrent.ConcurrentHashMap

object KeepPlayerNative  {

    private val callback = CallBack()

    init {
        System.loadLibrary("KeepPlayer")
        init(callback)
    }

    fun setContext(context: Context) {
        callback.context = context
    }

    private external fun init(callBack:CallBack)

    external fun newInstance(): Long

    external fun setPath(instance: Long, path: String)

    external fun prepare(instance: Long)

    external fun start(instance: Long)

    external fun resume(instance: Long)

    external fun pause(instance: Long)

    external fun stop(instance: Long)

    external fun reset(instance: Long)

    external fun release(instance: Long)

    external fun setSurface(instance: Long, surface: Surface)

    external fun updateSurfaceSize(instance: Long, surface: Surface, width: Int, height: Int)

    external fun seekTo(instance: Long, seekTime: Long);

    external fun test()


    class CallBack {
        var context:Context? = null

        fun getBitmap(path: String): Bitmap? {
            try {
                //test code
                val bit =  BitmapFactory.decodeResource(context!!.resources, R.drawable.ic_launcher)
                return bit
//                return BitmapFactory.decodeFile(path) ?: return null
            } catch (e: Exception) {
                e.printStackTrace()
            }
            return null
        }

        fun loadTexture(path: String): Int {
            val bitmap = BitmapFactory.decodeFile(path) ?: return 0
            val textureHandle = IntArray(1)

            GLES30.glGenTextures(1, textureHandle, 0)
            if (textureHandle[0] != 0) {
                // Bind to the texture in OpenGL
                GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, textureHandle[0])
                // Set filtering
                GLES30.glTexParameteri(
                    GLES30.GL_TEXTURE_2D,
                    GLES30.GL_TEXTURE_MIN_FILTER,
                    GLES30.GL_NEAREST
                )
                GLES30.glTexParameteri(
                    GLES30.GL_TEXTURE_2D,
                    GLES30.GL_TEXTURE_MAG_FILTER,
                    GLES30.GL_NEAREST
                )
                // Load the bitmap into the bound texture.

                GLUtils.texImage2D(GLES30.GL_TEXTURE_2D, 0, bitmap, 0)
            }

            if (textureHandle[0] == 0) {
                throw RuntimeException("Error loading texture.")
            }

            return textureHandle[0]
        }
    }
}