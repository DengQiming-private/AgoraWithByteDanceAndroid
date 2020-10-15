package io.agora.extension.provider;

import android.content.Context;
import android.util.Log;

public class ByteDanceExtensionProvider{
    static {
        System.loadLibrary("native-lib");
    }
    public static long getFilterProvider(Context context) {
        return nativeCreateProvider(context);
    }

    public static native long nativeCreateProvider(Context context);
}
