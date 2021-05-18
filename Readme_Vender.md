# SDK

### 1. Implement agora::rtc::IExtensionProvider

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

1.1 PROVIDER_TYPE defines the position of the video filter plugin in the pipeline:

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

The SDK only supports the following three provider types:
- `LOCAL_VIDEO_FILTER`
- `REMOTE_VIDEO_FILTER`
- `LOCAL_AUDIO_FILTER`

If you set the return value of `getProviderType()` as `LOCAL_VIDEO_FILTER` or `REMOTE_VIDEO_FILTER`, after the customer creates the `IExtensionProvider` object when initializing `RtcEngine`, the SDK calls
the `createVideoFilter()` method, and you need to return the `IVideoFilter` instance as described in Step 2.

If you set the return value of `getProviderType()` as `LOCAL_AUDIO_FILTER`, after the customer creates the `IExtensionProvider` object when initializing `RtcEngine`, the SDK calls
the `createAudioFilter()` method, and you need to return the `IAudioFilter` instance as described in Step 3.

1.2 Use IExtensionControl to trigger callback events and send logs

```
void ByteDanceProcessor::dataCallback(const char* data){
  if (control_ != nullptr) {
	control_->fireEvent(id_, "beauty", data);	
  }
}
```

When the customer registers the `IMediaExtensionObserver` object when initializing RtcEngine, the app receives this event. 

### 2. Implement agora::rtc::IVideoFilter

```
class IVideoFilter {
public:
  virtual bool adaptVideoFrame(const media::base::VideoFrame& capturedFrame,
                               media::base::VideoFrame& adaptedFrame);
  virtual void setEnabled(bool enable);
  virtual bool isEnabled();
  virtual size_t setProperty(const char* key, const void* buf, size_t buf_size);
  virtual size_t getProperty(const char* key, void* buf, size_t buf_size);
};
```

2.1 The `adaptVideoFrame` method is the core method of `IVideoFilter`. It processes video frames in `capturedFrame` and returns the processed frames in `adaptedFrame`.

### 3. Implement agora::rtc::IAudioFilter

```
class IAudioFilter {
public:
  virtual bool adaptAudioFrame(const media::base::AudioPcmFrame& inAudioFrame,
                               media::base::AudioPcmFrame& adaptedFrame);
  virtual void setEnabled(bool enable);
  virtual bool isEnabled();
  virtual size_t setProperty(const char* key, const void* buf, size_t buf_size);
  virtual size_t getProperty(const char* key, void* buf, size_t buf_size);
  virtual const char * getName();
};
```

3.1 The `adaptAudioFrame` method is the core method of `IAudioFilter`. It processes audio frames in `inAudioFrame` and returns the processed frames in `adaptedFrame`.

3.2 You need to specify the VENDOR_NAME in the return value of `getName`.

# Java native

### 4. Specify a native library (.so file) as the class loader and use the `nativeGetExtensionProvider` method in the class to get the native provider.

```
public class ExtensionManager {
  public static final String VENDOR_NAME = "ByteDance";
  static {
	System.loadLibrary("native-lib");
  }
  public static native long nativeGetExtensionProvider(Context context, String vendor, int type);
}
```
