package io.agora.extension;

import android.content.Context;

import androidx.annotation.Keep;

import java.io.File;
import java.io.IOException;

public class AgoraByteDanceNative {
    public static native int setContext(Context context);
    public static native int setParameters(String parameters);
    public static native int releaseContext();
    public static native boolean isEnable();
    public static native int setEnable(boolean enable);
    public static AgoraByteDanceDataReceiver dataReceiver;

    public static void copyResource(Context context) {
        String path = "resource";
        File dstFile = context.getExternalFilesDir("assets");
        FileUtils.clearDir(new File(dstFile, path));

        try {
            FileUtils.copyAssets(context.getAssets(), path, dstFile.getAbsolutePath());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Keep
    public static void onDataCallback(String data) {

        if (dataReceiver != null) {
            dataReceiver.onDataReceive(data);
        }
    }
}
