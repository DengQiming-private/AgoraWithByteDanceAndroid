# 字节 plugin 的使用方法
### 1. 将所需相关依赖文件放到指定目录
|依赖文件|存放路径|
|----|----|
|agora-rtc-sdk.jar|AgoraWithByteDanceAndroid\app\libs\|
|64位libagora-rtc-sdk-jni.so|AgoraWithByteDanceAndroid\app\src\main\jniLibs\arm64-v8a\|
|32位libagora-rtc-sdk-jni.so|AgoraWithByteDanceAndroid\app\src\main\jniLibs\armeabi-v7a\|
|64位字节美颜libeffect.so|AgoraWithByteDanceAndroid\agora-bytedance\src\main\jniLibs\arm64-v8a\|
|32位字节美颜libeffect.so|AgoraWithByteDanceAndroid\agora-bytedance\src\main\jniLibs\armeabi-v7a\|
|字节美颜资源包|AgoraWithByteDanceAndroid\agora-bytedance\src\main\assets\|

### 2. 使用 RtcEngine create(RtcEngineConfig config) 初始化方法
```
//更换为开发者自己的appID
private static final String appId = "#YOUR APP ID#";
......
RtcEngineConfig config = new RtcEngineConfig();
config.mContext = this;
config.mAppId = appId;
//通过插件提供的接口获取native provider句柄
long provider = ExtensionManager.nativeGetExtensionProvider(this); 
//注册native provider句柄，其中：vender用于区分不同的插件，observer用于监听该插件的消息
config.addExtensionProvider(ExtensionManager.VENDOR_NAME, provider, this); 
......
mRtcEngine = RtcEngine.create(config);
//enable插件
mRtcEngine.enableExtension(ExtensionManager.VENDOR_NAME, true);
```

2.1 addExtensionProvider可多次调用，以注册多个插件（需使用不同的VENDOR_NAME）

2.2 注册插件的消息回调需要实现 io.agora.rtc2.IMediaExtensionObserver 的 onEvent 接口
```
@Override
public void onEvent(String vendor, String key, String value) {
//vendor即为上述注册插件时的VENDOR_NAME，key/value是插件消息的键值对
......
}
```

### 3. 设置字节插件参数

设置模型加载，美颜，贴纸参数，参数用 json 的方式传输

```
// Android
ExtensionManager.nativeSetParameters(jsonString)
```

参数解释如下

```
{
  "plugin.bytedance.licensePath" : "字节 lincense 的路径",
  "plugin.bytedance.modelDir" : "模型所在根目录",
  
  "plugin.bytedance.faceAttributeEnabled" : true, // 是否启用脸部属性检测
  "plugin.bytedance.faceDetectModelPath" : "字节脸部检测模型路径",
  "plugin.bytedance.faceAttributeModelPath" : "字节脸部属性模型路径",
  
  "plugin.bytedance.handDetectEnabled" : true, // 是否启用手部检测
  "plugin.bytedance.handBoxModelPath" : "字节手部 box 模型的路径",
  "plugin.bytedance.handKPModelPath" : "字节手部 kp 模型的路径",
  "plugin.bytedance.handGestureModelPath" : "手部姿势模型路径",
  "plugin.bytedance.handDetectModelPath" : "手部检测模型路径",

  "plugin.bytedance.lightDetectEnabled" : true, // 是否启用灯光检测
  "plugin.bytedance.lightDetectModelPath" : "字节灯光检测模型的路径",
  
  "plugin.bytedance.faceStickerEnabled" : true, // 是否启用贴纸
  "plugin.bytedance.faceStickerItemResourcePath" : "要加载贴纸所在路径",
 
  "plugin.bytedance.aiEffectEnabled" : true, // 是否启用字节特效
  "plugin.bytedance.ai.composer.nodes" : [ // 美颜，化妆和修容的 composer node
    {
      "path" : "美颜路径1",
      "key" : "美颜路径1对应的 Key",
      "intensity" : 1 // 美颜强度
    },
    {
      "path" : "美颜路径2",
      "key" : "美颜路径2对应的 Key",
      "intensity" : 1 // 美颜强度
    }
  ]
}
```

### 4. 不同的识别结果将以 json 的方式返回
4.1 脸部识别的结果

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

4.2 手部识别的结果

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

4.3 灯光识别结果

```
"plugin.bytedance.light.info": {
        "selected_index": 1,
        "prob": 0.984
    }
```

