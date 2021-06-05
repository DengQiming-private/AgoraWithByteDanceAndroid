
// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"

#ifndef OPTIONAL_ENUM_CLASS
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_ENUM_CLASS OPTIONAL_ENUM_CLASS
#else
#define OPTIONAL_ENUM_CLASS enum
#endif
#endif

namespace agora {
namespace rtc {

struct TextureInfo {
  OPTIONAL_ENUM_CLASS TextureType {
    kGlTexture2D,
    kGlTextureOes,
  };
  OPTIONAL_ENUM_CLASS EglContextType {
    kEglContext10,
    kEglContext14,
  };

  TextureType texture_type;
  EglContextType context_type;
  void* shared_context;
  int texture_id;
  float transform_matrix[16];
};

struct MemPixelBuffer {
  OPTIONAL_ENUM_CLASS Format {
    kUnknown,
    kI420,
    kI422,
    kNV21,
    kNV12,
    kRGBA,
    kARGB,
    kBGRA
  };
  Format format;
  uint8_t* data;
  int size;
};

struct VideoFrameInfo {
  OPTIONAL_ENUM_CLASS Type {
    kMemPixels, // Pixels in memory
    kTexture, // Android: GL_TEXTURE_2D/GL_TEXTURE_OES
    kCVPixelBuffer, // iOS: CVPixelBufferRef
  };
  Type type;
  union {
    TextureInfo texture; // Android
    MemPixelBuffer memBuffer; // All platform
    void* cvpixelbuffer; // iOS
  };
  int width;
  int height;
  int rotation;
  int64_t timestamp_us; // Capture time in micro seconds
};

OPTIONAL_ENUM_CLASS VideoFrameMetaDataType {
  kAlphaChannel,
  // Add other types afterwards
};

struct AlphaChannel {
  uint8_t* data;
  int size;
};

/**
 * The IVideoFrame class defines the interface to
 * send video frame data to the SDK or get video frame data from the SDK.
 */
class IVideoFrame : public RefCountInterface {
 public:
  /**
   * Get the concrete video frame of the underlying buffer.
   * @return
   * - Type: The video frame info.
   */
  virtual int getVideoFrameInfo(VideoFrameInfo& info) const = 0;

  /**
   * Fill the underlying buffer with source buffer info contained in VideoFrameInfo
   * For frames of type "Type::kMemPixels", This function first tries to fill in-place with no copy and reallocation.
   * When it fails, a copy or copy-plus-reallocation may happen
   * @return 
   * - 0: if succeeds
   * - <0: failure
   */
  virtual int fill(const VideoFrameInfo& info) = 0;

  /**
   * Get the underlying meta data of the given type.
   * e.g. If type is VideoFrameMetaDataType::kAlphaChannel, then data value can be casted to AlphaChannel;
   */
  virtual int getVideoFrameMetaData(VideoFrameMetaDataType type, void* data) = 0;

  /**
   * Fill the underlying meta data of the given type.
   * e.g. If type is VideoFrameMetaDataType::kAlphaChannel, then data should point to AlphaChannel object;
   */
  virtual int fillVideoFrameMetaData(VideoFrameMetaDataType type, const void* data) = 0;

 protected:
  ~IVideoFrame() {}
};

class IVideoFrameMemoryPool : public RefCountInterface {
 public:
  /**
   * Create IVideoFrame
   * @param info video frame info of the new video frame
   * @param metatypes pointer to the array of meta types
   * @param count number of the meta types in the array
   */
  virtual agora::agora_refptr<IVideoFrame> createVideoFrame(
      const VideoFrameInfo& info, const VideoFrameMetaDataType* metatypes = NULL, int count = 0) = 0;

 protected:
  virtual ~IVideoFrameMemoryPool() {}
};

}  // namespace rtc
}  // namespace agora
