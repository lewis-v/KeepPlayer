package com.lewis.keepplayer.demo

import android.os.Bundle
import android.util.Log
import android.view.Surface
import android.widget.SeekBar
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
//            player.setPath("https://vkceyugu.cdn.bspapp.com/VKCEYUGU-imgbed/762a380c-152b-4880-a460-1c5cd8190e6f.mp4")
//            player.setPath("https://tt-ugc-v-cdnqn.52tt.com/videos-shumei/5fa7da33be5398000170ce5c_0_1604835891_0")
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
        seek.setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                Log.i(TAG, "onProgressChanged $progress")
                player.seekTo(3*1000);
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
                Log.i(TAG, "onStartTrackingTouch ${seekBar?.progress}")
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                Log.i(TAG, "onStopTrackingTouch ${seekBar?.progress}")
            }

        })
    }
}
