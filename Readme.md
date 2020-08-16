# 初始化字节 plugin

在 RtcEngine 初始化完成之后调用一下代码初始化字节的 plugin

```
// iOS
[[AgoraPluginManager sharedManager] loadPlugin];
[self.agoraKit enableLocalVideoFilter:@"agora" vendor:@"bytedance" enable:YES];

// Android
RtcEngineImpl.loadExtension("native-lib");
AgoraPluginManager.setContext(this);
```

# 移除字节 plugin

```
// iOS
[[AgoraPluginManager sharedManager] unloadPlugin];
```

# 设置参数

设置模型加载，美颜，贴纸参数，参数用 json 的方式传输

```
// iOS
[[AgoraPluginManager sharedManager] setParameter:jsonString];

// Android
AgoraPluginManager.setParameters(jsonString)
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



# 注册人脸识别，光线识别，表情识别，手部识别的回调

## iOS

```
[[AgoraPluginManager sharedManager] setDataReceiver:self];
```

其中 self 需要实现以下 protocol

```
@protocol AgoraByteDanceDataReceiver <NSObject>

- (void) onDataReceive: (NSString *) data;

@end
```

## Android

```
AgoraPluginManager.dataReceiver = this;
```

其中 this 需要继承以下接口

```
public interface AgoraByteDanceDataReceiver {
    public void onDataReceive(String data);
}
```

不同的识别结果会以 json 的方式返回

## 脸部识别的结果

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

## 手部识别的结果

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

## 灯光识别结果

```
"plugin.bytedance.light.info": {
        "selected_index": 1,
        "prob": 0.984
    }
```

