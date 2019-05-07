package com.example.administrator.ndkstudydemo;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-library");
    }

    private NativeUtils nativeUtils;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        nativeUtils = new NativeUtils();
//        Log.i(TAG, "old prop : " + nativeUtils.prop);
//        nativeUtils.accessJavaClassProp();
//        Log.i(TAG, "new prop : " + nativeUtils.prop);
        Log.d(TAG, "old static prop : " + NativeUtils.staticProp);
        NativeUtils.accessStaticJavaProp();
        Log.d(TAG, "new static prop : " + NativeUtils.staticProp);
//        nativeUtils.callJavaClassMethod();
        NativeUtils.callStaticJavaMethod();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String doProcess(String input);

    public void doCLick(View view) {
        doProcess("Hello ");
    }
}
