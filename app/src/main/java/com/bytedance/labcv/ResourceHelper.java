// Copyright (C) 2018 Beijing Bytedance Network Technology Co., Ltd.
package com.bytedance.labcv;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.annotation.NonNull;

import java.io.File;

public class ResourceHelper {

    public static final String RESOURCE = "resource";
    public static final String FILTER_RESOURCE = "FilterResource.bundle/Filter";
    private static final String LICENSE_NAME = "labcv_test_20200630_20200831_com.bytedance.labcv.demo_labcv_test_v3.9.2.1.licbag";

    private static String getResourcePath(Context context) {
        return context.getExternalFilesDir("assets").getAbsolutePath() + File.separator + RESOURCE;
    }

    public static String getModelDir(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "ModelResource.bundle"), "");
        return file.getAbsolutePath();
    }

    public static String getLicensePath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "LicenseBag.bundle"), LICENSE_NAME);
        return file.getAbsolutePath();
    }

    public static String getStickersPath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "stickers");
        return file.getAbsolutePath();
    }

    public static String getAnimojiPath(final Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "animoji");
        return file.getAbsolutePath();
    }

    public static String getGamePath(Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "game");
        return file.getAbsolutePath();
    }

    public static String getComposeMakeupComposerPath(@NonNull final Context context) {
        return getResourcePath(context) + File.separator + "ComposeMakeup.bundle" + File.separator + "ComposeMakeup/composer";
    }

    public static String getComposePath(@NonNull final Context context) {
        return getResourcePath(context) + File.separator + "ComposeMakeup.bundle"  + File.separator + "ComposeMakeup/";
    }


    public static File[] getFilterResources(@NonNull final Context context) {
        return getResources(context, FILTER_RESOURCE);
    }


    public static File[] getResources(@NonNull final Context context, String type) {
        File file = new File(new File(getResourcePath(context), type), "");
        if (file.exists() && file.isDirectory())
            return file.listFiles();
        return new File[0];
    }

    public static String getStickerPath(@NonNull final Context context,String name) {
        return getStickersPath(context) + File.separator + name;

    }

    public static  String getAnimojiPath(final Context context, String name) {
        return getAnimojiPath(context) + File.separator + name;
    }

    public static String getGamePath(Context context, String name) {
        return getGamePath(context) + File.separator + name;
    }

    public static boolean isResourceReady(@NonNull final Context context, int versionCode) {

        SharedPreferences preferences=context.getSharedPreferences("user", Context.MODE_PRIVATE);
        boolean resourceReady = preferences.getBoolean("resource", false);
        int preVersioncode = preferences.getInt("versionCode", 0);

        // 如果上次已经拷贝过 继续检查版本号
        // Continue to check the version number if it was copied last time
        if (resourceReady && versionCode == preVersioncode){
            return true;
        }
        return false;
    }

    public static void setResourceReady(@NonNull final Context context, boolean isReady, int versionCode){
        SharedPreferences preferences=context.getSharedPreferences("user", Context.MODE_PRIVATE);

        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean("resource", isReady);
        editor.putInt("versionCode", versionCode);
        editor.commit();
    }

    public static String getDownloadedStickerDir(@NonNull final Context context){
        File file = new File(new File(getResourcePath(context), "download"), "sticker");
        if (!file.exists()) {
            file.mkdirs();

        }
        return file.getAbsolutePath();


    }

    public static String getFilterResourcePathByName(@NonNull final Context context, String filterName){
        File file = new File(new File(getResourcePath(context), FILTER_RESOURCE), "");

        return file.getAbsolutePath() + File.separator + filterName;
    }




}
