#include <jni.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>

using namespace std;
using namespace cv;
Size numSize = Size(16, 24);

void BitmapToMat2(JNIEnv *env, jobject &bitmap, Mat &mat, jboolean needUnPremultiplyAlpha) {
    AndroidBitmapInfo info;
    void *pixels = 0;
    Mat &dst = mat;
    try {
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        dst.create(info.height, info.width, CV_8UC4);
        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (needUnPremultiplyAlpha) cvtColor(tmp, dst, COLOR_mRGBA2RGBA);
            else tmp.copyTo(dst);
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            cvtColor(tmp, dst, COLOR_BGR5652RGBA);
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch (const cv::Exception &e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nBitmapToMat}");
        return;
    }
}
bool compareFacesByHist(const Mat &srcImage, const Mat &tempalteImage) {
    int sums = 0;
    for (int i = 0; i < srcImage.rows; i++) {
        for (int j = 0; j < srcImage.cols; j++) {
            sums += abs(srcImage.at<uchar>(i, j) - tempalteImage.at<uchar>(i, j));
        }
    }
    float s = sums / (srcImage.rows * srcImage.cols + 0.0);
    return s < 20;
}

string getNums(Mat src, vector<Mat> oriMats) {
    if (src.empty()) {
        return "";
    }

    resize(src, src, Size(640, 400));

//    灰度图
    cvtColor(src, src, COLOR_BGR2GRAY);
    Mat dst;
//    二值化
    threshold(src, dst, 150, 255, THRESH_BINARY);

    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(20, 10));
    erode(dst, dst, erodeElement);

    vector<vector<Point>> contours;
    findContours(dst, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    for (auto &contour: contours) {
        Rect rect = boundingRect(contour);
        if (rect.width > rect.height * 9) {
            dst = src(rect);
        }
    }

    threshold(dst, dst, 150, 255, THRESH_BINARY);
    Mat erods;
    erodeElement = getStructuringElement(MORPH_RECT, Size(2, 2));

    dilate(dst, erods, erodeElement);
    erode(erods, erods, erodeElement);
    erode(erods, erods, erodeElement);
    erode(erods, erods, erodeElement);
    erode(erods, erods, erodeElement);

    contours.clear();
    findContours(erods, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    vector<Mat> mats;
    for (auto &contour: contours) {
        Rect rect = boundingRect(contour);
        if (rect.width > 5 && rect.height > 5 && rect.height >= rect.width &&
            rect.width * rect.height >= 200) {
            mats.push_back(dst(rect));
        }
    }
    vector<int> result;
    for (int i = 0; i < mats.size(); i++) {
        Mat m = mats.at(i);
        resize(m, m, numSize);
        for (int j = 0; j < oriMats.size(); j++) {
            if (compareFacesByHist(m, oriMats.at(j))) {
                result.push_back(j);
            }
        }
    }
    string s;
    for (int i = result.size() - 1; i >= 0; i--) {
        s.append(format("%d", result[i]));
    }
    return s;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_cam_idrecong_OpenCVUtils_00024Companion_idRecognise(JNIEnv *env, jobject thiz,
                                                             jobject bitmap, jobject list) {
    Mat m;
    BitmapToMat2(env, bitmap, m, false);
    vector<Mat> mats;
    jclass clazz = env->GetObjectClass(list);
//    get(int index)
    jmethodID methodId = env->GetMethodID(clazz, "get", "(I)Ljava/lang/Object;");
    for (int i = 0; i < 10; i++) {
        Mat x;
        jobject oriBitmap = (env->CallObjectMethod(list, methodId, i));
        BitmapToMat2(env, oriBitmap, x, false);
        cvtColor(x, x, COLOR_BGR2GRAY);
        resize(x, x, numSize);
        mats.push_back(x);
    }
    try {
        string s = getNums(m, mats);
        return env->NewStringUTF(s.c_str());
    } catch (Exception e) {
        return env->NewStringUTF("error");
    }
}