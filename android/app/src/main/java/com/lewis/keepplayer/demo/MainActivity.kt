package com.lewis.keepplayer.demo

import android.os.Bundle
import android.util.Log
import android.view.Surface
import androidx.appcompat.app.AppCompatActivity
import com.lewis.keepplayer.ISurfaceListener
import com.lewis.keepplayer.KeepPlayer
import com.lewis.keepplayer.KeepPlayerNative
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    companion object {
        private const val TAG = "MainActivity"
    }

    val player by lazy { KeepPlayer() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        KeepPlayerNative.setContext(this)
        surface.surfaceListener = object: ISurfaceListener {
            override fun onCreate(surface: Surface, width: Int, height: Int) {
                player.setSurface(surface)
            }

            override fun onChange(surface: Surface, width: Int, height: Int) {
                player.updateSurfaceSize(surface, width, height)
            }

            override fun onDestroy(surface: Surface) {

            }

        }

        bt1.setOnClickListener {
            player.setPath("https://tt-ugc-v-cdnqn.52tt.com/videos-dst/5f75a91722f3270001c1f4dc_0_1601546519_0")
            Thread {

                player.prepare()
                Log.i(TAG, "prepare finish")
                player.start()
                Log.i(TAG, "start finish")
            }.start()
        }

        bt2.setOnClickListener {
            player.resume()
        }

        bt3.setOnClickListener {
            player.pause()
        }

        bt4.setOnClickListener {
            player.stop()
        }

        bt5.setOnClickListener {
            player.reset()
        }
    }
}
