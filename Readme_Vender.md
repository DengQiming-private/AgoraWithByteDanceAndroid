# native部分
### 1. 实现agora::rtc::IExtensionVideoFilterProvider接口
```
class IExtensionVideoFilterProvider {
public:
  virtual ~IExtensionVideoFilterProvider() = default;
  virtual unsigned int extensionFrameworkVersion(char* buffer, unsigned int buffer_size) const = 0;
  virtual unsigned int vendor(char* buffer, unsigned int buffer_size) const = 0;
  virtual unsigned int name(char* buffer, unsigned int buffer_size) const = 0;
  virtual unsigned int version(char* buffer, unsigned int buffer_size) const = 0;
  virtual Video_Filter_Position position() const = 0;
  virtual IExtensionVideoFilter* create() = 0;
  virtual void destroy(IExtensionVideoFilter* filter) = 0;
};
```
其中:
###### 1.1 extensionFrameworkVersion 是指 Agora sdk 中 extension framework 的版本号，现在为“1.0.0”
###### 1.2 Video_Filter_Position 是指插件在 video pipeline 中的位置，定义如下：
```
enum class Video_Filter_Position {
  Video_Filter_Invalid_Position = 0,
  Video_Filter_Pre_Encode = 1,
  Video_Filter_Post_Decode
};
```
### 2. 实现agora::rtc::IExtensionVideoFilter接口

```
class IExtensionVideoFilter {
public:
  virtual ~IExtensionVideoFilter() = default;
  virtual bool setProperty(const char* key, const char* json_value) = 0;
  virtual unsigned int property(const char* key,
                                char* json_value_buffer,
								unsigned int json_value_buffer_size) const = 0;
  virtual bool setExtensionFacility(IExtensionFacility* facility) = 0;
  virtual bool filter(const agora::media::base::VideoFrame& original_frame,
                          agora::media::base::VideoFrame& processed_frame) = 0;
};
```
其中:
###### 2.1 filter函数通过处理 original_frame，返回 processed_frame，提供了插件的核心功能
###### 2.2 IExtensionFacility 提供了触发回调事件 & log能力
```
class IExtensionFacility {
public:
  virtual ~IExtensionFacility() = default;
  virtual void fireEvent(const char* key, const char* json_value) = 0;
  virtual void log(commons::LOG_LEVEL level, const char* message) = 0;
};
```

# Java部分
### 3. 指定一个类加载 native 库（.so文件），并在该类中提供 nativeGetFilterProvider 方法以获取 native provider
```
public class AgoraPluginManager {
  static {
	System.loadLibrary("native-lib");
  }
  public static native long nativeGetFilterProvider(Context context);
}
```
### 4. 如有需要，提供 nativeSetParameters 方法，以将所需参数设置到native层
```
public static native int nativeSetParameters(String parameters);
```
