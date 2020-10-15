#pragma once
#include "AgoraMediaBase.h"
#include "NGIAgoraExtensionFacility.h"

namespace agora {
namespace rtc {
static const char* extension_video_framework_version = "1.0.0";
static const unsigned int max_vendor_length = 64;
static const unsigned int max_name_length = 64;
static const unsigned int max_version_length = 16;

class IExtensionVideoFilter {
public:
  virtual ~IExtensionVideoFilter() = default;
  virtual bool setProperty(const char* key, const char* json_value) = 0;
  virtual unsigned int property(const char* key,
                                char* json_value_buffer, unsigned int json_value_buffer_size) const = 0;
  virtual bool setExtensionFacility(IExtensionFacility* facility) = 0;
  virtual bool filter(const agora::media::base::VideoFrame& original_frame,
                          agora::media::base::VideoFrame& processed_frame) = 0;
};

enum class Video_Filter_Position {
  Video_Filter_Invalid_Position = 0,
  Video_Filter_Pre_Encode = 1,
  Video_Filter_Post_Decode
};

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
using ExtensionVideoFilterProviderDeleter = void (*)(IExtensionVideoFilterProvider*);
using ExtensionFacilityDeleter = void(*)(IExtensionFacility*);
}
}
