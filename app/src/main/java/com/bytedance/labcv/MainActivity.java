package com.bytedance.labcv;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;

import io.agora.extension.AgoraByteDanceDataReceiver;
import io.agora.extension.AgoraByteDanceNative;
import io.agora.extension.ResourceHelper;
import io.agora.extension.UtilsAsyncTask;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.internal.RtcEngineImpl;
import io.agora.rtc2.video.VideoCanvas;


public class MainActivity extends AppCompatActivity implements UtilsAsyncTask.OnUtilsAsyncTaskEvents, AgoraByteDanceDataReceiver {

    private static final String[] REQUESTED_PERMISSIONS = {
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };

    private static final String appId = "5db0d12c40354100abd7a8a0adaa1fb8";
    private final static String TAG = "Agora_zt java :";
    private static final int PERMISSION_REQ_ID = 22;
    private FrameLayout localVideoContainer;
    private FrameLayout remoteVideoContainer;
    private RtcEngine mRtcEngine;
    private SurfaceView mRemoteView;
    private TextView infoTextView;
    private Button beautyButton;
    private String handInfoText;
    private String faceInfoText;
    private String lightInfoText;

    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initUI();
        checkPermission();
        File dstFile = getExternalFilesDir("assets");

        if (!ResourceHelper.isResourceReady(this, 1)) {
            Log.d("Agora_zt", "Resource is not ready, need to copy resource");
            infoTextView.setText("Resource is not ready, need to copy resource");
            new UtilsAsyncTask(this, this).execute();
        } else {
            Log.d("Agora_zt", "Resource is ready");
            infoTextView.setText("Resource is ready");
            initEffectEngine();
        }
        AgoraByteDanceNative.dataReceiver = this;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        AgoraByteDanceNative.dataReceiver = null;
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    private void checkPermission() {
        if (checkSelfPermission(REQUESTED_PERMISSIONS[0], PERMISSION_REQ_ID) &&
                checkSelfPermission(REQUESTED_PERMISSIONS[1], PERMISSION_REQ_ID) &&
                checkSelfPermission(REQUESTED_PERMISSIONS[2], PERMISSION_REQ_ID)) {
            initAgoraEngine();
        }
    }

    private void initUI() {
        localVideoContainer = findViewById(R.id.view_container);
        remoteVideoContainer = findViewById(R.id.remote_video_view_container);
        infoTextView = findViewById(R.id.infoTextView);
        beautyButton = findViewById(R.id.enable_beauty_button);
        beautyButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                enableEffect();
            }
        });
    }

    private boolean checkSelfPermission(String permission, int requestCode) {
        if (ContextCompat.checkSelfPermission(this, permission) !=
                PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, REQUESTED_PERMISSIONS, requestCode);
            return false;
        }
        return true;
    }

    private void initAgoraEngine() {
        try {

            mRtcEngine = RtcEngine.create(this, appId, new IRtcEngineEventHandler() {
                @Override
                public void onJoinChannelSuccess(String s, int i, int i1) {
                    Log.d(TAG, "onJoinChannelSuccess");
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mRtcEngine.startPreview();
                        }
                    });
                }

                @Override
                public void onFirstRemoteVideoDecoded(final int i, int i1, int i2, int i3) {
                    super.onFirstRemoteVideoDecoded(i, i1, i2, i3);
                    Log.d(TAG, "onFirstRemoteVideoDecoded  uid = " + i);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            setupRemoteVideo(i);
                        }
                    });
                }

                @Override
                public void onUserJoined(int i, int i1) {
                    super.onUserJoined(i, i1);
                    Log.d(TAG, "onUserJoined  uid = " + i);
                }

                @Override
                public void onUserOffline(final int i, int i1) {
                    super.onUserOffline(i, i1);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            onRemoteUserLeft();
                        }
                    });
                }
            });

            setupLocalVideo();

            mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
            mRtcEngine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
            mRtcEngine.setParameters("{\"rtc.log_filter\": 65535}");
            mRtcEngine.enableLocalVideo(true);
            mRtcEngine.enableVideo();
            mRtcEngine.enableAudio();
            boolean s = RtcEngineImpl.loadExtension("native-lib");
            mRtcEngine.enableLocalVideoFilter("agora", "bytedance", true);
            Log.d(TAG, "api call join channel");
            mRtcEngine.joinChannel("", "agora_test", "", 0);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void setupLocalVideo() {
        SurfaceView view = RtcEngine.CreateRendererView(this);
        view.setZOrderMediaOverlay(true);
        localVideoContainer.addView(view);
        mRtcEngine.setupLocalVideo(new VideoCanvas(view, VideoCanvas.RENDER_MODE_HIDDEN, 0));
        mRtcEngine.setLocalRenderMode(Constants.RENDER_MODE_HIDDEN);
    }

    private void setupRemoteVideo(int uid) {
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        int count = remoteVideoContainer.getChildCount();
        View view = null;
        for (int i = 0; i < count; i++) {
            View v = remoteVideoContainer.getChildAt(i);
            if (v.getTag() instanceof Integer && ((int) v.getTag()) == uid) {
                view = v;
            }
        }

        if (view != null) {
            return;
        }

        Log.d(TAG, " setupRemoteVideo uid = " + uid);
        mRemoteView = RtcEngine.CreateRendererView(getBaseContext());
        remoteVideoContainer.addView(mRemoteView);
        mRtcEngine.setupRemoteVideo(new VideoCanvas(mRemoteView, VideoCanvas.RENDER_MODE_HIDDEN, uid));
        mRtcEngine.setRemoteRenderMode(uid, Constants.RENDER_MODE_HIDDEN);
        mRemoteView.setTag(uid);
    }

    private void onRemoteUserLeft() {
        removeRemoteVideo();
    }

    private void removeRemoteVideo() {
        if (mRemoteView != null) {
            remoteVideoContainer.removeView(mRemoteView);
        }
        mRemoteView = null;
    }

    private void enableEffect() {
        JSONObject o = new JSONObject();
        try {
            o.put("plugin.bytedance.licensePath", ResourceHelper.getLicensePath(this));
            o.put("plugin.bytedance.modelDir", ResourceHelper.getModelDir(this));
            o.put("plugin.bytedance.aiEffectEnabled", true);

            o.put("plugin.bytedance.faceAttributeEnabled", true);
            o.put("plugin.bytedance.faceDetectModelPath", ResourceHelper.getFaceModelPath(this));
            o.put("plugin.bytedance.faceAttributeModelPath", ResourceHelper.getFaceAttriModelPath(this));

            o.put("plugin.bytedance.faceStickerEnabled", true);
            o.put("plugin.bytedance.faceStickerItemResourcePath", ResourceHelper.getStickerPath(this, "leisituer"));

            o.put("plugin.bytedance.handDetectEnabled", true);
            o.put("plugin.bytedance.handDetectModelPath", ResourceHelper.getHandModelPath(this, ResourceHelper.DetectParamFile));
            o.put("plugin.bytedance.handBoxModelPath", ResourceHelper.getHandModelPath(this, ResourceHelper.BoxRegParamFile));
            o.put("plugin.bytedance.handGestureModelPath", ResourceHelper.getHandModelPath(this, ResourceHelper.GestureParamFile));
            o.put("plugin.bytedance.handKPModelPath", ResourceHelper.getHandModelPath(this, ResourceHelper.KeyPointParamFile));

            o.put("plugin.bytedance.lightDetectEnabled", true);
            o.put("plugin.bytedance.lightDetectModelPath", ResourceHelper.getLightClsModelPath(this));

            JSONObject node1 = new JSONObject();
            node1.put("path", ResourceHelper.getComposePath(this) + "lip/fuguhong");
            node1.put("key", "Internal_Makeup_Lips");
            node1.put("intensity", 1.0);

            JSONObject node2 = new JSONObject();
            node2.put("path", ResourceHelper.getComposePath(this) + "blush/weixun");
            node2.put("key", "Internal_Makeup_Blusher");
            node2.put("intensity", 1.0);

            JSONObject node3 = new JSONObject();
            node3.put("path", ResourceHelper.getComposePath(this) + "reshape_camera");
            node3.put("key", "Internal_Deform_Face");
            node3.put("intensity", 1.0);


            JSONArray arr = new JSONArray();
            arr.put(node1);
            arr.put(node2);
            arr.put(node3);
//            arr.put(node4);
            o.put("plugin.bytedance.ai.composer.nodes", arr);

            AgoraByteDanceNative.setParameters(o.toString());
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void initEffectEngine() {
        AgoraByteDanceNative.setContext(this);

    }

    @Override
    public void onPreExecute() {

    }

    @Override
    public void onPostExecute() {
        ResourceHelper.setResourceReady(this, true, 1);
        Toast.makeText(this, "copy resource Ready", Toast.LENGTH_LONG).show();
        infoTextView.setText("Resource is ready");
        initEffectEngine();
    }

    @Override
    public void onDataReceive(String data) {
        try {

            JSONObject o = new JSONObject(data);
            if (o.has("plugin.bytedance.light.info")) {
                StringBuilder sb = new StringBuilder();
                JSONObject lightInfo = o.getJSONObject("plugin.bytedance.light.info");
                int selectedIndex = lightInfo.getInt("selected_index");
                double prob = lightInfo.getDouble("prob");
                sb.append("Light: ");
                String lightName[] = {"Indoor Yellow", "Indoor White", "Indoor weak", "Sunny", "Cloudy", "Night", "Backlight"};
                if (selectedIndex >= 0 && selectedIndex <= 7) {
                    sb.append(lightName[selectedIndex]);
                    sb.append(" prob: ");
                    sb.append(prob);
                } else {
                    sb.append(("Unknown"));
                }
                sb.append("\n");
                lightInfoText = sb.toString();
            }
            if (o.has("plugin.bytedance.hand.info")) {
                StringBuilder sb = new StringBuilder();
                JSONArray handsInfo = o.getJSONArray("plugin.bytedance.hand.info");
                sb.append("Hand: \n");
                String handActionName[] = {"HEART_A", "HEART_B", "HEART_C", "HEART_D", "OK", "HAND_OPEN", "THUMB_UP", "THUMB_DOWN", "ROCK", "NAMASTE", "PLAM_UP", "FIST", "INDEX_FINGER_UP", "DOUBLE_FINGER_UP", "VICTORY", "BIG_V", "PHONECALL", "BEG", "THANKS", "UNKNOWN", "CABBAGE", "THREE", "FOUR", "PISTOL", "ROCK2", "SWEAR", "HOLDFACE", "SALUTE", "SPREAD", "PRAY", "QIGONG", "SLIDE", "PALM_DOWN", "PISTOL2", "NARUTO1", "NARUTO2", "NARUTO3", "NARUTO4", "NARUTO5", "NARUTO7", "NARUTO8", "NARUTO9", "NARUTO10", "NARUTO11", "NARUTO12"};
                for (int i = 0; i < handsInfo.length(); i++) {
                    JSONObject handInfo = handsInfo.getJSONObject(i);
                    int action = handInfo.getInt("action");
                    if (action >=0 && action <= 44) {
                        sb.append("action: ");
                        sb.append(handActionName[action]);
                    }
                    if (action == 47) {
                        sb.append("action: ");
                        sb.append("RAISE");
                    }
                    sb.append("\n");
                    handInfoText = sb.toString();
                }
            }
            if (o.has("plugin.bytedance.face.info")) {
                StringBuilder sb = new StringBuilder();
                JSONArray facesInfo = o.getJSONArray("plugin.bytedance.face.info");
                sb.append("Face: \n");
                for (int i = 0; i < facesInfo.length(); i++) {
                    JSONObject faceInfo = facesInfo.getJSONObject(i);
                    double yaw = faceInfo.getDouble("yaw");
                    double roll = faceInfo.getDouble("roll");
                    double pitch = faceInfo.getDouble("pitch");
                    sb.append("yaw: ");
                    sb.append(yaw);
                    sb.append("roll: ");
                    sb.append(roll);
                    sb.append("pitch: ");
                    sb.append(pitch);
                    sb.append("\n");
                    int action = faceInfo.getInt("action");
                    sb.append("action: ");
                    sb.append(action);
                    if ((action & 0x00000002) != 0) {
                        sb.append(" ");
                        sb.append("眨眼");
                    }
                    if ((action & 0x00000004) != 0) {
                        sb.append(" ");
                        sb.append("张嘴");
                    }
                    if ((action & 0x00000008) != 0) {
                        sb.append(" ");
                        sb.append("摇头");
                    }
                    if ((action & 0x00000010) != 0) {
                        sb.append(" ");
                        sb.append("点头");
                    }
                    if ((action & 0x00000020) != 0) {
                        sb.append(" ");
                        sb.append("挑眉");
                    }
                    if ((action & 0x00000040) != 0) {
                        sb.append(" ");
                        sb.append("嘟嘴");
                    }
                    sb.append("\n");
                    sb.append("expression: ");
                    int expression = faceInfo.getInt("expression");
                    String expressionName[] = {"Angry", "Disgust", "Fear", "Happy", "Sad", "Surprise", "Neutral"};
                    if (expression >=0 && expression <=6) {
                        sb.append(expressionName[expression]);
                    } else {
                        sb.append("Unknown");
                    }
                    sb.append("\n");
                    double confuseProb = faceInfo.getDouble("confused_prob");
                    sb.append("confused prob: ");
                    sb.append(confuseProb);
                    sb.append("\n");
                    faceInfoText = sb.toString();
                }
            }
            this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    StringBuilder sb = new StringBuilder();
                    if (lightInfoText != null) {
                        sb.append(lightInfoText);
                    }
                    if (handInfoText != null) {
                        sb.append(handInfoText);
                    }
                    if (faceInfoText != null) {
                        sb.append(faceInfoText);
                    }
                    infoTextView.setText(sb.toString());
                }
            });

        } catch (JSONException e) {
            e.printStackTrace();
        }

    }
}
