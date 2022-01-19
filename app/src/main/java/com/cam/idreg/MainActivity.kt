package com.cam.idreg

import android.content.res.Resources
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.cam.idrecong.OpenCVUtils

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val bitmap = BitmapFactory.decodeResource(resources,R.drawable.id_test);
        val s = OpenCVUtils.idRecognise(bitmap,this)
        Log.e("CAM",s)
    }
}