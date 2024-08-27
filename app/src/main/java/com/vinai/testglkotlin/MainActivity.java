package com.vinai.testglkotlin;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("test_app_native"); // Tên module bạn đã khai báo trong Android.mk
        System.loadLibrary("test_app_native_renderer"); // Tên module bạn đã khai báo trong Android.mk
    }

    public native String stringFromJNI();
    public native void initSurface(Surface surface);
    public native void deinitSurface();
    public native void surfaceResize();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Sử dụng phương thức JNI
        TextView textView = findViewById(R.id.textView);
        textView.setText(stringFromJNI());

        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
                initSurface(surfaceHolder.getSurface());
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {
                surfaceResize();
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
                deinitSurface();
            }
        });
    }
}