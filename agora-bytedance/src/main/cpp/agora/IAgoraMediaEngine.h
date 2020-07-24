//
//  Agora Media SDK
//
//  Copyright (c) 2015 Agora IO. All rights reserved.
//

#ifndef AGORA_MEDIA_ENGINE_H
#define AGORA_MEDIA_ENGINE_H

#include <stdint.h>
#include "IAgoraRtcEngine.h"

namespace agora {
namespace media {
/** 
 * The type of media device.
 */
enum MEDIA_SOURCE_TYPE {
  /** 
   * 0: The audio playback device.
   */
  AUDIO_PLAYOUT_SOURCE = 0,
  /** 
   * 1: Microphone.
   */
  AUDIO_RECORDING_SOURCE = 1,
};
/**
 * The IAudioFrameObserver class.
 */
class IAudioFrameObserver {
 public:
  /**
   * The audio frame type.
   */
  enum AUDIO_FRAME_TYPE {
    /**
     * 0: The frame type is PCM.
     */
    FRAME_TYPE_PCM16 = 0,  // PCM 16bit little endian
  };
  /** 
   * The definition of AudioFrame.
   */
  struct AudioFrame {
    AUDIO_FRAME_TYPE type;
    /** 
     * The number of samples per channel in this frame.
     */
    int samplesPerChannel;
    /** 
     * The number of bytes per sample: Two for PCM 16.
     */
    int bytesPerSample;  
    /** 
     * The number of channels (data is interleaved, if stereo).
     */
    int channels; 
    /** 
     * The Sample rate.
     */
    int samplesPerSec;
    /** 
     * The pointer to the data buffer.
     */
    void* buffer;  
    /** 
     * The timestamp to render the audio data. Use this member to synchronize the audio renderer while 
     * rendering the audio streams.
     *
     * @note
     * This timestamp is for audio stream rendering. Set it as 0.
    */
    int64_t renderTimeMs;
    int avsync_type;
  };

 public:
  virtual ~IAudioFrameObserver() = default;

  /** 
   * Occurs when the recorded audio frame is received.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The recorded audio frame is valid and is encoded and sent.
   * - false: The recorded audio frame is invalid and is not encoded or sent.
   */
  virtual bool onRecordAudioFrame(AudioFrame& audioFrame) = 0;
  /** 
   * Occurs when the playback audio frame is received.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The playback audio frame is valid and is encoded and sent.
   * - false: The playback audio frame is invalid and is not encoded or sent.
   */
  virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame) = 0;
  /** 
   * Occurs when the mixed audio data is received.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The mixed audio data is valid and is encoded and sent.
   * - false: The mixed audio data is invalid and is not encoded or sent.
   */
  virtual bool onMixedAudioFrame(AudioFrame& audioFrame) = 0;
  /** 
   * Occurs when the playback audio frame before mixing is received.
   * @param uid ID of the remote user.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The playback audio frame before mixing is valid and is encoded and sent.
   * - false: The playback audio frame before mixing is invalid and is not encoded or sent.
   */
  virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame) = 0;
};
/**
 * The IVideoFrameObserver class.
 */
class IVideoFrameObserver {
 public:
  using VideoFrame = agora::media::VideoFrame;
  
 public:
  virtual ~IVideoFrameObserver() {}

  /** 
   * Occurs each time the SDK receives a video frame captured by the local camera.
   *
   * After you successfully register the video frame observer, the SDK triggers this callback each time 
   * a video frame is received. In this callback, you can get the video data captured by the local 
   * camera. You can then pre-process the data according to your scenarios.
   * 
   * After pre-processing, you can send the processed video data back to the SDK by setting the 
   * `videoFrame` parameter in this callback.
   * 
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @return Determines whether to ignore the current video frame if the pre-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
  */
  virtual bool onCaptureVideoFrame(VideoFrame& videoFrame) = 0;
  /** 
   * Occurs each time the SDK receives a video frame sent by the remote user.
   * 
   * After you successfully register the video frame observer, the SDK triggers this callback each time a 
   * video frame is received. In this callback, you can get the video data sent by the remote user. You 
   * can then post-process the data according to your scenarios.
   * 
   * After post-processing, you can send the processed data back to the SDK by setting the `videoFrame`
   * parameter in this callback.
   *
   * @param uid ID of the remote user who sends the current video frame.
   * @param connectionId ID of the connection.
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @return Determines whether to ignore the current video frame if the post-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
   */
  virtual bool onRenderVideoFrame(rtc::uid_t uid, rtc::conn_id_t connectionId,
                                  VideoFrame& videoFrame) = 0;

  virtual VIDEO_PIXEL_FORMAT getVideoPixelFormatPreference() {
      return VIDEO_PIXEL_I420; }
  virtual bool getRotationApplied() { return false; }
  virtual bool getMirrorApplied() { return false; }
};

/**
 * The IMediaEngine class.
 */
class IMediaEngine {
 public:
  /**
   * Registers an audio frame observer object.
   *
   * @note
   * Ensure that you call this method before \ref IRtcEngine::joinChannel "joinChannel".
   *
   * @param observer A pointer to the audio frame observer object: IAudioFrameObserver.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(IAudioFrameObserver* observer) = 0;
  /** 
   * Registers a video frame observer object.
   *
   * @note
   * - Ensure that you call this method before joining the channel.
   * - If you register an observer for video raw video data, you cannot register an IVideoEncodedImageReceiver
   * object.
   *
   * @param observer A pointer to the video frame observer: IVideoFrameObserver.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoFrameObserver(IVideoFrameObserver* observer) = 0;

  /** 
   * Registers a receiver object for the encoded video image.
   *
   * @note
   * - Ensure that you call this method before joining the channel.
   * - If you register a receiver for encoded video data, you cannot register an IVideoFrameObserver
   * object.
   *
   * @param receiver A pointer to the receiver of the encoded video image: \ref rtc::IVideoEncodedImageReceiver 
   * "IVideoEncodedImageReceiver".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoEncodedImageReceiver(rtc::IVideoEncodedImageReceiver* receiver) = 0;

  /** 
   * Pushes the external audio data to the app.
   *
   * @param type deprecated.
   * @param frame The audio buffer data.
   * @param wrap deprecated.
   * @param sourceId The audio track ID.
   * @param connectionId The channel connection ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushAudioFrame(MEDIA_SOURCE_TYPE type, IAudioFrameObserver::AudioFrame* frame,
                             bool wrap = false, int sourceId = 0,
                             int connectionId = agora::rtc::DEFAULT_CONNECTION_ID) {
    return -1;
  }
  /**
   * Pulls the remote audio data.
   *
   * After a successful method call, the app pulls the decoded and mixed audio data for playback.
   *
   * The difference between this method and the \ref onPlaybackAudioFrame "onPlaybackAudioFrame" is as follows:
   * - `onPlaybackAudioFrame`: The SDK sends the audio data to the app once every 10 ms. Any delay in processing 
   * the audio frames may result in audio jitter.
   * - `pullAudioFrame`: The app pulls the remote audio data. After setting the audio data parameters, the 
   * SDK adjusts the frame buffer and avoids problems caused by jitter in the external audio playback.
   *
   * @param frame The pointer to the audio frame: AudioFrame.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pullAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;

  /** 
   * Sets the external video source.
   *
   * Once the external video source is enabled, the SDK prepares to accept the external video frame.
   *
   * @param enabled Determines whether to enable the external video source.
   * - true: Enable the external video source. Once set, the SDK creates the external source and prepares 
   * video data from `pushVideoFrame` or `pushEncodedVideoImage`.
   * - false: Disable the external video source.
   * @param useTexture Determines whether to use textured video data.
   * - true: Use texture, which is not supported now.
   * - False: Do not use texture.
   * @param encodedFrame Determines whether the external video source is encoded.
   * - true: The external video source is encoded.
   * - false: The external video source is not encoded.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalVideoSource(bool enabled, bool useTexture, bool encodedFrame = false) = 0;
  /** 
   * Sets the external audio source.
   *
   * @note
   * Ensure that you call this method before joining the channel.
   *
   * @param enabled Determines whether to enable the external audio source:
   * - true: Enable the external audio source.
   * - false: (default) Disable the external audio source.
   * @param sampleRate The Sample rate (Hz) of the external audio source, which can set be as 
   * 8000, 16000, 32000, 44100, or 48000.
   * @param channels The number of channels of the external audio source, which can be set as 1 or 2:
   * - 1: Mono.
   * - 2: Stereo.
   * @param sourceNumber The number of audio source per channel.
   * - 1: Mono
   * - 2: Dual-track
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setExternalAudioSource(bool enabled, int sampleRate, int channels, int sourceNumber) = 0;

  /** Deprecated by IRtcEngine::setVideoEncoderConfiguration. 
   */
  virtual int setExternalVideoConfigEx(
      const rtc::VideoEncoderConfiguration& config,
      rtc::conn_id_t connectionId = rtc::DEFAULT_CONNECTION_ID) = 0;

  /** 
   * Pushes the external video frame to the app.
   *
   * @param frame The external video frame: ExternalVideoFrame.
   * @param connectionId The connection ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushVideoFrame(media::ExternalVideoFrame* frame,
                             rtc::conn_id_t connectionId = rtc::DEFAULT_CONNECTION_ID) = 0;

  /** 
   * Pushes the encoded video image to the app.
   * @param imageBuffer A pointer to the video image.
   * @param length The data length.
   * @param videoEncodedFrameInfo The reference to the information of the encoded video frame: 
   * \ref agora::rtc::EncodedVideoFrameInfo "EncodedVideoFrameInfo".
   * @param connectionId The connection ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int pushEncodedVideoImage(const uint8_t* imageBuffer, size_t length,
                                    const agora::rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo,
                                    rtc::conn_id_t connectionId = rtc::DEFAULT_CONNECTION_ID) = 0;

  virtual void release() { delete this; }

 protected:
  virtual ~IMediaEngine(){};
};

}  // namespace media

}  // namespace agora

#endif  // AGORA_MEDIA_ENGINE_H
