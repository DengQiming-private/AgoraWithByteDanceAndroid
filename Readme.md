# 字节 plugin 的使用方法

### 1. 使用 RtcEngine create(RtcEngineConfig config) 初始化方法

```
private static final String EXTENSION_TAG = "ByteDance";
RtcEngineConfig config = new RtcEngineConfig();
config.mContext = this;
config.mAppId = appId;
long provider = AgoraPluginManager.nativeGetFilterProvider(this); //通过插件提供的接口获取native provider句柄
config.addExtensionProvider(EXTENSION_TAG, provider); //注册native provider句柄，其中的EXTENSION_TAG用于区分不同的插件
config.mEventHandler = new IRtcEngineEventHandler() {
  ......
  @Override
  public void onExtensionEvent(String tag, String key, final String value) { //此回调函数的第一个参数即为extension tag
	......
  }
}
mRtcEngine = RtcEngine.create(config);
```
其中，
1.1 addExtensionProvider可多次调用，以注册多个插件（需使用不同的EXTENSION_TAG）
1.2 注册插件的event回调需要实现 IRtcEngineEventHandler 的 onExtensionEvent 接口

### 2. 设置参数

设置模型加载，美颜，贴纸参数，参数用 json 的方式传输

```
// Android
AgoraPluginManager.nativeSetParameters(jsonString)
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

### 3. 不同的识别结果将以 json 的方式返回
3.1 脸部识别的结果

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

3.2 手部识别的结果

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

3.3 灯光识别结果

```
"plugin.bytedance.light.info": {
        "selected_index": 1,
        "prob": 0.984
    }
```

