package com.example.administrator.ndkstudydemo;

import android.util.Log;


public class NativeUtils {



    private static final String TAG = "NativeUtils";
    public static int staticProp = -1;
    public int prop = -1;


    public String getStringFromJava(String str){
        Log.i(TAG, "getStringFromJava: "+ str);
        return "Hello C , I am from Java.";
    }

    public static String getStringFromJavaStatic(String str){
        Log.i(TAG, "getStringFromJavaStatic: " + str);
        return "Hello C , I am from Java static.";
    }

    public native void accessJavaClassProp();

    public native void callJavaClassMethod();

    public static native void accessStaticJavaProp();

    public static native void callStaticJavaMethod();

}
