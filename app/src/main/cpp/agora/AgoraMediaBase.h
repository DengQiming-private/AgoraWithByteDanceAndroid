//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)
#include <stdint.h>

namespace agora {
namespace media {

static const uint8_t kMaxCodecNameLength = 50;

/** The packet information, i.e., infomation contained in RTP heaader.
*/
struct PacketOptions {
  // Rtp timestamp
  uint32_t time_stamp;
  PacketOptions()
      : time_stamp(0) {}
};


/** The video pixel format.
 */
enum VIDEO_PIXEL_FORMAT {
  /** 0: The video pixel format is unknown.
   */
  VIDEO_PIXEL_UNKNOWN = 0,
  /** 1: The video pixel format is I420.
   */
  VIDEO_PIXEL_I420 = 1,
  /** 2: The video pixel format is BGRA.
   */
  VIDEO_PIXEL_BGRA = 2,
  /** 3: Planar YUV 4:2:2 format.
   */
  VIDEO_PIXEL_I422 = 3,
  /** 2: The video pixel format is RGBA.
   */
  VIDEO_PIXEL_RGBA = 4,
  /** 8: The video pixel format is NV12.
   */
  VIDEO_PIXEL_NV12 = 8,
};

/** The external video frame.
 */
struct ExternalVideoFrame {
  /** The video buffer type.
   */
  enum VIDEO_BUFFER_TYPE {
    /** 1: The video buffer in the format of raw data.
     */
    VIDEO_BUFFER_RAW_DATA = 1,
  };

  /** The buffer type. See #VIDEO_BUFFER_TYPE
   */
  VIDEO_BUFFER_TYPE type;
  /** The pixel format. See #VIDEO_PIXEL_FORMAT
   */
  VIDEO_PIXEL_FORMAT format;
  /** The video buffer.
   */
  void* buffer;
  /** Line spacing of the incoming video frame, which must be in pixels instead of bytes. For
   * textures, it is the width of the texture.
   */
  int stride;
  /** Height of the incoming video frame.
   */
  int height;
  /** [Raw data related parameter] The number of pixels trimmed from the left. The default value is
   * 0.
   */
  int cropLeft;
  /** [Raw data related parameter] The number of pixels trimmed from the top. The default value is
   * 0.
   */
  int cropTop;
  /** [Raw data related parameter] The number of pixels trimmed from the right. The default value is
   * 0.
   */
  int cropRight;
  /** [Raw data related parameter] The number of pixels trimmed from the bottom. The default value
   * is 0.
   */
  int cropBottom;
  /** [Raw data related parameter] The clockwise rotation of the video frame. You can set the
   * rotation angle as 0, 90, 180, or 270. The default value is 0.
   */
  int rotation;
  /** Timestamp of the incoming video frame (ms). An incorrect timestamp results in frame loss or
   * unsynchronized audio and video.
   */
  long long timestamp;
};

/** Definition of VideoFrame.

The video data format is in YUV420. The buffer provides a pointer to a pointer. However, the
interface cannot modify the pointer of the buffer, but can only modify the content of the buffer.

*/
struct VideoFrame {
  VIDEO_PIXEL_FORMAT type;
  /** Video pixel width.
   */
  int width;  // width of video frame
  /** Video pixel height.
   */
  int height;  // height of video frame
  /** Line span of Y buffer in YUV data.
   */
  int yStride;  // stride of Y data buffer
  /** Line span of U buffer in YUV data.
   */
  int uStride;  // stride of U data buffer
  /** Line span of V buffer in YUV data.
   */
  int vStride;  // stride of V data buffer
  /** Pointer to the Y buffer pointer in the YUV data.
   */
  uint8_t* yBuffer;  // Y data buffer
  /** Pointer to the U buffer pointer in the YUV data.
   */
  uint8_t* uBuffer;  // U data buffer
  /** Pointer to the V buffer pointer in the YUV data
   */
  uint8_t* vBuffer;  // V data buffer
  /** Set the rotation of this frame before rendering the video, and it supports 0, 90, 180, 270
   * degrees clockwise.
   */
  int rotation;  // rotation of this frame (0, 90, 180, 270)
  /** Timestamp to render the video stream. It instructs the users to use this timestamp to
  synchronize the video stream render while rendering the video streams.

  Note: This timestamp is for rendering the video stream, not for capturing the video stream.
  */
  int64_t renderTimeMs;
  int avsync_type;
};

/**
 * @brief Player state
 *
 */
enum MEDIA_PLAYER_STATE {
  /** Player idle
   */
  PLAYER_STATE_IDLE = 0,
  /** Opening media file
   */
  PLAYER_STATE_OPENING,
  /** Media file opened successfully
   */
  PLAYER_STATE_OPEN_COMPLETED,
  /** Player playing
   */
  PLAYER_STATE_PLAYING,
  /** Player paused
   */
  PLAYER_STATE_PAUSED,
  /** Player playback completed
   */
  PLAYER_STATE_PLAYBACK_COMPLETED,
  /** Player stopped
   */
  PLAYER_STATE_STOPPED = PLAYER_STATE_IDLE,
  /** Player pausing (internal)
   */
  PLAYER_STATE_PAUSING_INTERNAL = 50,
  /** Player stopping (internal)
   */
  PLAYER_STATE_STOPPING_INTERNAL,
  /** Player seeking state (internal)
   */
  PLAYER_STATE_SEEKING_INTERNAL,
  /** Player getting state (internal)
   */
  PLAYER_STATE_GETTING_INTERNAL,
  /** None state for state machine (internal)
   */
  PLAYER_STATE_NONE_INTERNAL,
  /** Do nothing state for state machine (internal)
   */
  PLAYER_STATE_DO_NOTHING_INTERNAL,
  /** Player failed
   */
  PLAYER_STATE_FAILED = 100,
};

/**
 * @brief Player error code
 *
 */
enum MEDIA_PLAYER_ERROR {
  /** No error
   */
  PLAYER_ERROR_NONE = 0,
  /** The parameter is incorrect
   */
  PLAYER_ERROR_INVALID_ARGUMENTS = -1,
  /** Internel error
   */
  PLAYER_ERROR_INTERNAL = -2,
  /** No resource error
   */
  PLAYER_ERROR_NO_RESOURCE = -3,
  /** Media source is invalid
   */
  PLAYER_ERROR_INVALID_MEDIA_SOURCE = -4,
  /** Unknown stream type
   */
  PLAYER_ERROR_UNKNOWN_STREAM_TYPE = -5,
  /** Object is not initialized
   */
  PLAYER_ERROR_OBJ_NOT_INITIALIZED = -6,
  /** Decoder codec not supported
   */
  PLAYER_ERROR_CODEC_NOT_SUPPORTED = -7,
  /** Video renderer is invalid
   */
  PLAYER_ERROR_VIDEO_RENDER_FAILED = -8,
  /** Internal state error
   */
  PLAYER_ERROR_INVALID_STATE = -9,
  /** Url not found
   */
  PLAYER_ERROR_URL_NOT_FOUND = -10,
  /** Invalid connection state
   */
  PLAYER_ERROR_INVALID_CONNECTION_STATE = -11,
  /** Insufficient buffer data
   */
  PLAYER_ERROR_SRC_BUFFER_UNDERFLOW = -12,
};

/**
 * @brief Media stream type
 *
 */
enum MEDIA_STREAM_TYPE {
  /** Unknown stream type
   */
  STREAM_TYPE_UNKNOWN = 0,
  /** Video stream
   */
  STREAM_TYPE_VIDEO = 1,
  /** Audio stream
   */
  STREAM_TYPE_AUDIO = 2,
  /** Subtitle stream
   */
  STREAM_TYPE_SUBTITLE = 3,
};

/**
 * @brief Player event
 *
 */
enum MEDIA_PLAYER_EVENT {
  /** player seek begin
   */
  PLAYER_EVENT_SEEK_BEGIN = 0,
  /** player seek complete
   */
  PLAYER_EVENT_SEEK_COMPLETE = 1,
  /** player seek error
   */
  PLAYER_EVENT_SEEK_ERROR = 2,
  /** player video published
   */
  PLAYER_EVENT_VIDEO_PUBLISHED = 3,
  /** player audio published
   */
  PLAYER_EVENT_AUDIO_PUBLISHED = 4,
};

/**
 * @brief Media stream object
 *
 */
struct MediaStreamInfo { /* the index of the stream in the media file */
  int streamIndex;

  /* stream type */
  MEDIA_STREAM_TYPE streamType;

  /* stream encoding name */
  char codecName[kMaxCodecNameLength];

  /* streaming language */
  char language[kMaxCodecNameLength];

  /* If it is a video stream, video frames rate */
  int videoFrameRate;

  /* If it is a video stream, video bit rate */
  int videoBitRate;

  /* If it is a video stream, video width */
  int videoWidth;

  /* If it is a video stream, video height */
  int videoHeight;

  /* If it is an audio stream, audio bit rate */
  int audioSampleRate;

  /* If it is an audio stream, the number of audio channels */
  int audioChannels;

  /* If it is an audio stream, bits per sample */
  int audioBitsPerSample;

  /* stream duration in second */
  int64_t duration;};

/**
 * @brief Player Metadata type
 *
 */
enum MEDIA_PLAYER_METADATA_TYPE {
  /** data type unknown
   */
  PLAYER_METADATA_TYPE_UNKNOWN = 0,
  /** sei data
   */
  PLAYER_METADATA_TYPE_SEI = 1,
};

}  // namespace media

namespace rtc {
/**
 * The audio route.
 */
enum AudioRoute
{
  /**
   * -1: The default audio route.
   */
  ROUTE_DEFAULT = -1,
  /**
   * The headset.
   */
  ROUTE_HEADSET,
  /**
   * The earpiece.
   */
  ROUTE_EARPIECE,
  /**
   * The headset with no microphone.
   */
  ROUTE_HEADSETNOMIC,
  /**
   * The speakerphone.
   */
  ROUTE_SPEAKERPHONE,
  /**
   * The loudspeaker.
   */
  ROUTE_LOUDSPEAKER,
  /**
   * The Bluetooth headset.
   */
  ROUTE_HEADSETBLUETOOTH
};
} // namespace rtc


}  // namespace agora
