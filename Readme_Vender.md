# native部分
### 1. 实现agora::rtc::IExtensionProvider接口
```
class IExtensionProvider {
public:
  virtual PROVIDER_TYPE getProviderType();
  virtual void setExtensionControl(IExtensionControl* control);
  virtual agora_refptr<IVideoFilter> createVideoFilter(const char* id);
  virtual agora_refptr<IAudioFilter> createAudioFilter(const char* id);
  virtual agora_refptr<IVideoSinkBase> createVideoSink(const char* id);
};
```
其中:
1.1 PROVIDER_TYPE 是指插件在 video pipeline 中的位置，定义如下：
```
enum PROVIDER_TYPE {
  LOCAL_AUDIO_FILTER,
  REMOTE_AUDIO_FILTER,
  LOCAL_VIDEO_FILTER,
  REMOTE_VIDEO_FILTER,
  LOCAL_VIDEO_SINK,
  REMOTE_VIDEO_SINK,
  UNKNOWN,
};
```
2.1 IExtensionControl 用于触发回调事件 & log能力
```
void ByteDanceProcessor::dataCallback(const char* data){
  if (control_ != nullptr) {
	control_->fireEvent(id_, "beauty", data);	
  }
}
```

### 2. 实现agora::rtc::IVideoFilter接口

```
class IVideoFilter {
public:
  virtual bool adaptVideoFrame(const media::base::VideoFrame& capturedFrame,
                               media::base::VideoFrame& adaptedFrame);
  virtual void setEnabled(bool enable);
  virtual bool isEnabled();
  virtual size_t setProperty(const char* key, const void* buf, size_t buf_size);
  virtual size_t getProperty(const char* key, void* buf, size_t buf_size) = 0;
};
```
其中:
2.1 adaptVideoFrame函数通过处理 capturedFrame，返回 adaptedFrame，提供了插件的核心功能

# Java部分
### 3. 指定一个类加载 native 库（.so文件），并在该类中提供 nativeGetExtensionProvider 方法以获取 native provider
```
public class ExtensionManager {
  public static final String VENDOR_NAME = "ByteDance";
  static {
	System.loadLibrary("native-lib");
  }
  public static native long nativeGetExtensionProvider(Context context);
}
```
### 4. 如有需要，提供 nativeSetParameters 方法，以将所需参数设置到native层
```
public static native int nativeSetParameters(String parameters);
```
