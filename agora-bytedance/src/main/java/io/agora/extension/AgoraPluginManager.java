package io.agora.extension;

import android.content.Context;

import androidx.annotation.Keep;

import java.io.File;
import java.io.IOException;

@Keep
public class AgoraPluginManager {
    public static native int setParameters(String parameters);
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
}
