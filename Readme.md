# Introduction
This demo app is used to show how to load `IVideoFilter/IAudioFilter` type plug-ins in the Agora SDK. The main functions include

1. IVideoFilter -- Encapsulate `ByteDance VolcanoEngine` as Agora SDK's `IVideoFilter` type plug-in, and provide functions such as face detection, beautification, and addition of props. The button at the bottom of the main interface of the demo app can enable/disable this function.

2. IAudioFilter -- The Agora SDK team encapsulates a simple `IAudioFilter` type plug-in, which provides the function of adjusting the volume of the microphone collected locally. The slide bar under the main interface of the demo app can be used to adjust the volume.

# How to use ByteDance plugin
### 1. Place the required dependent files in the specified directory
|Dependent file|Storage path|
|----|----|
|agora-rtc-sdk.jar|AgoraWithByteDanceAndroid\app\libs|
|64bit libagora-rtc-sdk-jni.so|AgoraWithByteDanceAndroid\app\src\main\jniLibs\arm64-v8a|
|32bit libagora-rtc-sdk-jni.so|AgoraWithByteDanceAndroid\app\src\main\jniLibs\armeabi-v7a|
|64bit libeffect.so|AgoraWithByteDanceAndroid\agora-bytedance\src\main\jniLibs\arm64-v8a|
|32bit libeffect.so|AgoraWithByteDanceAndroid\agora-bytedance\src\main\jniLibs\armeabi-v7a|
|ByteDance resource package|AgoraWithByteDanceAndroid\agora-bytedance\src\main\assets|

### 2. Use RtcEngine create(RtcEngineConfig config) to initialize
```
//Replace with the developer’s own appID
private static final String appId = "#YOUR APP ID#";
......
RtcEngineConfig config = new RtcEngineConfig();
config.mContext = this;
config.mAppId = appId;
//Name of dynamic link library is provided by plug-in vendor,
//e.g. libagora-bytedance.so whose EXTENSION_NAME should be "agora-bytedance"
config.addExtension(ExtensionManager.EXTENSION_NAME);
//Observer is used to monitor the messages reported by the plug-in
config.mExtensionObserver = this;
......
//Create RtcEngine
mRtcEngine = RtcEngine.create(config);
......
//Join channel
mRtcEngine.joinChannel("", channelName, "", 0);
```

2.1 RtcEngineConfig.addExtension can be called for multiple times to register multiple plug-ins (different EXTENSION_NAME is required)

2.2 The `io.agora.rtc2.IMediaExtensionObserver#onEvent` interface needs to be implemented to receive the message callback of the registered plug-in
```
@Override
public void onEvent(String vendor, String extension, String key, String value) {
//The vendor is the name of registered plug-in when RtcEngine is created, 
//the extension is the name of video/audio extension filter created in the plug-in,
//the key/value is the key-value pair of message from plug-in.
......
}
```

### 3. Set ByteDance plugin parameters

Set model loading, beautification, sticker parameters, and parameters are set as json mode

```
//The vendor is the name of registered plug-in when RtcEngine is created, 
//the extension is the name of video/audio extension filter created in the plug-in (a plug-in can contain multiple extension filters),
//the key/value is the key-value pair of parameter to be set into the plug-in.
mRtcEngine.setExtensionProperty(ExtensionManager.EXTENSION_VENDOR_NAME, ExtensionManager.EXTENSION_VIDEO_FILTER_NAME, "key", "value");
```

3.1 The parameters of the ByteDance plug-in are explained as follows

```
{
  "plugin.bytedance.licensePath" : "Path of ByteDance lincense",
  "plugin.bytedance.modelDir" : "The root directory where the model is located",
  
  "plugin.bytedance.faceAttributeEnabled" : true, // Whether to enable face attribute detection
  "plugin.bytedance.faceDetectModelPath" : "Path of face detection model",
  "plugin.bytedance.faceAttributeModelPath" : "Path of face attribute model",
  
  "plugin.bytedance.handDetectEnabled" : true, // Whether to enable hand detection
  "plugin.bytedance.handBoxModelPath" : "Path of hand box model",
  "plugin.bytedance.handKPModelPath" : "Path of the hand kp model",
  "plugin.bytedance.handGestureModelPath" : "Path of hand pose model",
  "plugin.bytedance.handDetectModelPath" : "Path of hand detection model",

  "plugin.bytedance.lightDetectEnabled" : true, // Whether to enable light detection
  "plugin.bytedance.lightDetectModelPath" : "Path of the light detection model",
  
  "plugin.bytedance.faceStickerEnabled" : true, // Whether to enable stickers
  "plugin.bytedance.faceStickerItemResourcePath" : "Path of the sticker",
 
  "plugin.bytedance.aiEffectEnabled" : true, // Whether to enable byte effects
  "plugin.bytedance.ai.composer.nodes" : [ // Composer node for beauty, makeup and repair
    {
      "path" : "Beauty Path1",
      "key" : "Key for Beauty Path1",
      "intensity" : 1 // Beauty strength
    },
    {
      "path" : "Beauty Path2",
      "key" : "Key for Beauty Path2",
      "intensity" : 1 // Beauty strength
    }
  ]
}
```

### 4. Different recognition results will be returned as json
4.1 Result of facial recognition

```
"plugin.bytedance.face.info": [
        {
            "yaw": 11.429,
            "roll": -1.536,
            "pitch": -9.299,
            "action": 0,
            "expression": 4,
            "confused_prob": 0.0
        },
        {
            "yaw": 1.429,
            "roll": -1.536,
            "pitch": -9.299,
            "action": 2,
            "expression": 4,
            "confused_prob": 0.0
        }
    ]
```

4.2 Result of hand recognition

```
"plugin.bytedance.hand.info": [
        {
            "action": 19,
            "seq_action": 0.0
        },
        {
            "action": 9,
            "seq_action": 0.0
        }
    ]
```

4.3 Result of light recognition

```
"plugin.bytedance.light.info": {
        "selected_index": 1,
        "prob": 0.984
    }
```

