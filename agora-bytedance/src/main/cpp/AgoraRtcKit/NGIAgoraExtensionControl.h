//
// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)
#include "AgoraBase.h"
#include "AgoraRefPtr.h"
#include "IAgoraLog.h"
#include "NGIAgoraVideoFrame.h"
#include "NGIAgoraMediaNode.h"

namespace agora {
namespace rtc {

class IExtensionControl;

/**
 * Interfaces for Extension Provider
 * User can implement these interfaces for providing their own media node implementations to SDK.
 * Please refer to \ref agora::RefCountedObject to wrap your implementation so that it can be
 * held by agora::agora_refptr.
 * For example:
 * class YourExtensionProvide: public IExtensionProvider {
 *  // Your Concrete implementaion
 * };
 * 
 * // Use agroa::RefCountedObject to provide RefCountInterface implementation for you implementation,
 * // intantiate and wrap it with agora_refptr.
 * 
 * agora_refptr<IExtensionProvider> provider = new RefCountedObject<YourExtensionProvide>(Arg1, Arg2, ...);
 * 
 * You can instantiate your AudioFilter/VideoFilter/VideoSink in the same way.
 */
class IExtensionProvider : public RefCountInterface {
 public:
  enum PROVIDER_TYPE {
    LOCAL_AUDIO_FILTER,
    REMOTE_AUDIO_FILTER,
    LOCAL_VIDEO_FILTER,
    REMOTE_VIDEO_FILTER,
    LOCAL_VIDEO_SINK,
    REMOTE_VIDEO_SINK,
    UNKNOWN,
  };

  virtual PROVIDER_TYPE getProviderType() {
    return UNKNOWN;
  }

  virtual void setExtensionControl(IExtensionControl* control) {}

  virtual agora_refptr<IAudioFilter> createAudioFilter(const char* id) {
    return NULL;
  }

  virtual agora_refptr<IVideoFilter> createVideoFilter(const char* id) {
    return NULL;
  }

  virtual agora_refptr<IVideoSinkBase> createVideoSink(const char* id) {
    return NULL;
  }

 protected:
  ~IExtensionProvider() {}
};

/**
 * Interface for handling agora extensions
 */
class IExtensionControl {
 public:
  /**
   * Agora Extension Capabilities
   */
  struct Capabilities {
    bool audio;
    bool video;
  };

  /**
   * Get the capabilities of agora extensions
   * @param capabilities current supported agora extension features
   */
  virtual void getCapabilities(Capabilities& capabilities) = 0;

  /**
   * This method creates an IVideoFrame object with specified type, format, width and height
   * @return
   * - The pointer to \ref agora::rtc::IVideoFrame, if the method call succeeds
   * - The emply pointer nullptr, if the method call fails
   */
  virtual agora_refptr<IVideoFrame> createVideoFrame(
      IVideoFrame::Type type, IVideoFrame::Format format, int width, int height) = 0;
  
  /**
   * This method creates a new IVideoFrame obj by copying from the source video frame
   * @return
   * - The pointer to \ref agora::rtc::IVideoFrame, if the method call succeeds
   * - The empty pointer nullptr, if the method call fails
   */
  virtual agora_refptr<IVideoFrame> copyVideoFrame(agora_refptr<IVideoFrame> src) = 0;

  /**
   * This method recycle internal frame memory with specified type.
   * Deprecated video frames will be recycled automatically inside sdk. However,
   * user can invoke the following method to perform an immediate memory recycle.
   * @param type type of the frame memory to be recycled.
   */
  virtual void recycleVideoCache(IVideoFrame::Type type) = 0;

  /**
   * This method dumps the content of the video frame to the specified file.
   * @return
   * - 0: if succeeds
   * - <0: failure
   */
  virtual int dumpVideoFrame(agora_refptr<IVideoFrame> frame, const char* file) = 0;

  /**
   * write log into sdk.
   * @param level logging level
   * @param message logging message string
   * @return
   * - 0, if succeeds
   * - <0, if error happens
   */
  virtual int log(commons::LOG_LEVEL level, const char* message) = 0;

  virtual int fireEvent(const char* id, const char* event_key, const char* event_json_str) = 0;

 protected:
  virtual ~IExtensionControl() {}
};

}  // namespace rtc
}  // namespace agora
