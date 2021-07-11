### 1. Implement agora::rtc::IExtensionProvider

```
class IExtensionProvider {
public:
  virtual void enumerateExtensions(ExtensionMetaInfo* extension_list, int& extension_count);
  virtual void setExtensionControl(IExtensionControl* control);
  virtual agora_refptr<IExtensionVideoFilter> createVideoFilter(const char* id);
  virtual agora_refptr<IAudioFilter> createAudioFilter(const char* id);
  virtual agora_refptr<IVideoSinkBase> createVideoSink(const char* id);
};
```

1.1 Return a list of all supported extensions in `enumerateExtensions`
- Agora SDK will call the interface of `enumerateExtensions` to enumerate all the supported extension informations when loading
- The information should include the extension type and the corresponding name. 

1.2 EXTENSION_TYPE defines the position of the video/audio extension filter in the pipeline:

```
enum EXTENSION_TYPE {
  AUDIO_FILTER,
  VIDEO_PRE_PROCESSING_FILTER,
  VIDEO_POST_PROCESSING_FILTER,
  AUDIO_SINK,
  VIDEO_SINK,
  UNKNOWN,
};
```

Agora SDK only supports the following three types of extension filter:
- `AUDIO_FILTER`
- `VIDEO_PRE_PROCESSING_FILTER`
- `VIDEO_POST_PROCESSING_FILTER`

If you set the extension type as `VIDEO_PRE_PROCESSING_FILTER` or `VIDEO_POST_PROCESSING_FILTER`, Agora SDK will call the `createVideoFilter()` method 
when loading the instance of `IExtensionProvider`, and you need to return the `IExtensionVideoFilter` instance as described in Step 2.

If you set the extension type as `AUDIO_FILTER`, Agora SDK will call the `createAudioFilter()` method when loading the instance of `IExtensionProvider`, 
and you need to return the `IAudioFilter` instance as described in Step 3.


### 2. Implement agora::rtc::IExtensionVideoFilter

```
class IExtensionVideoFilter {
public:
  virtual void getProcessMode(ProcessMode& mode, bool& independent_thread);
  virtual void getVideoFormatWanted(VideoFrameData::Type& type, RawPixelBuffer::Format& format);
  virtual bool adaptVideoFrame(const media::base::VideoFrame& capturedFrame,
                               media::base::VideoFrame& adaptedFrame);
  virtual ProcessResult pendVideoFrame(agora::agora_refptr<IVideoFrame> frame);
  virtual int start(agora::agora_refptr<Control> control);
  virtual int stop();
  virtual void setEnabled(bool enable);
  virtual bool isEnabled();
  virtual size_t setProperty(const char* key, const void* buf, size_t buf_size);
  virtual size_t getProperty(const char* key, void* buf, size_t buf_size);
};
```

2.1 `getProcessMode`
- If set `ProcessMode` to Sync mode, Agora SDK and video extension will pass data through `adaptVideoFrame`
- If set `ProcessMode` to Async mode, Agora SDK and video extension will pass data through `pendVideoFrame` and `deliverVideoFrame`
- If set `independent_thread` to false, all callbacks sent by Agora SDK are performed on the internal video processing thread
- If set `independent_thread` to true, all callbacks sent by Agora SDK are performed on a separate thread

2.2 `start`
- After the video data processing pipeline of Agora SDK is started, the `start` callback will be triggered. You can initialize OpenGL in this callback
- You need to keep the received `IExtensionVideoFilter##Control` object to facilitate the subsequent interaction between the video extension and Agora SDK

2.3 `stop`
- The `stop` callback will be triggered before the video data processing pipeline of Agora SDK stops. You can release OpenGL in this callback

2.4 `getVideoFormatWanted`
- Each time Agora SDK provides a frame of video data to be processed to the extension, it will first call the `getVideoFormatWanted` method
- You need to specify the data type and format of this frame in the return value
- You can specify different data types and formats for different video frames

2.5 `adaptVideoFrame`
- In synchronization mode (mode is set to Sync), Agora SDK uses this method to transfer data to and from the video extension
- Agora SDK passes the video data to be processed to the extension, and the extension returns the processed data to Agora SDK

2.6 `pendVideoFrame`
- In asynchronous mode (mode is set to Async), Agora SDK uses this method to submit pending video frames to the video extension
- The extension must return the processed video frame through the `deliverVideoFrame` method of `IExtensionVideoFilter##Control`.

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


### 4. Package
4.1 Use `REGISTER_AGORA_EXTENSION_PROVIDER` located in the `AgoraExtensionProviderEntry.h` to register a extension provider
- You need to use this macro at the entrance of the extension provider, which will automatically register your extension provider into Agora SDK
- No need to use quotation marks for input parameter of PROVIDER_NAME

4.2 Link to `libagora-rtc-sdk-jni.so` in `CMakeLists.txt` and place the required dependent files in the specified directory
|Dependent file|Storage path|
|----|----|
|64bit libagora-rtc-sdk-jni.so|AgoraWithByteDanceAndroid\agora-bytedance\src\main\agoraLibs\arm64-v8a|
|32bit libagora-rtc-sdk-jni.so|AgoraWithByteDanceAndroid\agora-bytedance\src\main\agoraLibs\armeabi-v7a|

4.3 Please list the following name in a Java or md file
- EXTENSION_NAME: the name of target link library used in `CMakeLists.txt`, e.g. libagora-bytedance.so whose EXTENSION_NAME should be "agora-bytedance"
- EXTENSION_VENDOR_NAME: the extension provider name used for registering in `agora-bytedance.cpp`
- EXTENSION_FILTER_NAME: the video/audio filter name defined in `ExtensionProvider.h`

