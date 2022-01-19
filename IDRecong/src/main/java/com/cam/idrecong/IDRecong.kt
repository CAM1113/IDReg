package com.cam.idrecong


import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory

class OpenCVUtils {
    companion object {
        init {
            System.loadLibrary("opencv")
        }

        private external fun idRecognise(bitmap: Bitmap, list:List<Bitmap>): String

        fun idRecognise(bitmap: Bitmap,context: Context): String{
            val list = ArrayList<Bitmap>()
            for(i in 0..9){
                list.add(BitmapFactory.decodeStream(context.assets.open("$i.jpg")))
            }
            return idRecognise(bitmap,list);
        }
    }
}
