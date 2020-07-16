//
//  Agora SDK
//
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "../AgoraBase.h"
#include "../AgoraRefPtr.h"
#include "../IAgoraMediaEngine.h"
#include "IAgoraService.h"
#include "NGIAgoraCameraCapturer.h"
#include "NGIAgoraScreenCapturer.h"
#include "NGIAgoraVideoMixerSource.h"

namespace agora {
namespace rtc {

/**
 * The IAudioPcmDataSender class.
 *
 * In scenarios involving custom audio source, you can use the IAudioPcmDataSender class
 * to push PCM audio data directly to the audio track. If the audio track is disabled,
 * the pushed audio data will be automatically discarded.
 */
class IAudioPcmDataSender : public RefCountInterface {
 public:
  /**
   * Sends the PCM audio data to the local audio track.
   *
   * @param audio_data The PCM audio data to be sent.
   * @param capture_timestamp The timestamp for capturing the audio data.
   * @param samples_per_channel The number of audio samples in 10 ms for each channel.
   * @param bytes_per_sample The number of bytes for each sample.
   * @param number_of_channels The number of channels.
   * @param sample_rate The sample rate (Hz). The minimum value is 8000.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendAudioPcmData(
      const void* audio_data, uint32_t capture_timestamp,
      const size_t samples_per_channel,  // for 10ms Data, number_of_samples * 100 = sample_rate
      const size_t bytes_per_sample,     // 2 * number_of_channels
      const size_t number_of_channels,
      const uint32_t sample_rate) = 0;  // sample_rate > 8000

 protected:
  ~IAudioPcmDataSender() {}
};

/**
 * The IAudioEncodedFrameSender class.
 *
 * In scenarios involving custom audio source, you can use the IAudioEncodedFrameSender class to
 * push encoded audio data directly to the audio track. If the track is disabled, the pushed audio
 * data will be automatically discarded.
 */
class IAudioEncodedFrameSender : public RefCountInterface {
 public:
  /**
   * Sends the encoded audio frame to the local audio track.
   *
   * @param payload_data The pointer to the playload data.
   * @param payload_size The playload size.
   * @param audioFrameInfo The reference to the information of the audio frame:
   * EncodedAudioFrameInfo.
   *
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual bool sendEncodedAudioFrame(const uint8_t* payload_data, size_t payload_size,
                                     const EncodedAudioFrameInfo& audioFrameInfo) = 0;

 protected:
  ~IAudioEncodedFrameSender() {}
};

/**
 * The IMediaPacketReceiver class. This can be register in remote audio or video tracks and triggered
 * when RTP/UDP packet is received.
 */
class IMediaPacketReceiver {
 public:
  /**
   * Occurs each time the track receives media packet
   * @param packet The pointer to the media packet
   * @param length The length of the packet.
   *
   */
  virtual bool onMediaPacketReceived(const uint8_t *packet, size_t length) = 0;

  virtual ~IMediaPacketReceiver() {}
};

/**
 * The IMediaControlPacketReceiver class.
 *
 * This can be register in audio or video tracks and triggered
 * when RTCP/UDP packet is received.
 */
class IMediaControlPacketReceiver {
 public:
  /**
   * Occurs each time the track receives media control packet
   * @param packet The pointer to the media packet
   * @param length The length of the packet.
   *
   */
  virtual bool onMediaControlPacketReceived(const uint8_t *packet, size_t length) = 0;

  virtual ~IMediaControlPacketReceiver() {}
};

/**
 * The IMediaPacketSender class.
 *
 * You can use the IMediaPacketSender class to create LocalVideoTrack or LocalAudioTrack,
 * and then send media packet directly to the audio track or video track, i.e., RTP/UDP packet contains
 * media payload. If the track is disabled, the packet will be automatically discarded.
 */
class IMediaPacketSender : public RefCountInterface {
 public:
  /**
   * Sends the frame packet to the sepcific track.
   *
   * @param packet The pointer to the packet.
   * @param length The packet size.
   * @param PacketOptions The packet information.
   *
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual int sendMediaPacket(const uint8_t *packet, size_t length,
                              const agora::media::PacketOptions &options) = 0;
 protected:
  ~IMediaPacketSender() {}
};

/**
 * The IMediaControlPacketSender class.
 *
 * You can get IMediaControlPacketSender class object from VideoTrack or AudioTrack,
 * and then send media control packet directly, i.e., RTCP/UDP packet contains
 * media control payload. If the track is disabled, the packet will be automatically discarded.
 */
class IMediaControlPacketSender {
 public:
  /**
   * Sends the media transport control packet to the user
   * Currently, we only support send packet through video track.
   *
   * @param userId Send the packet to the specific user
   * @param packet The pointer to the packet.
   * @param length The packet size.
   *
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual int sendPeerMediaControlPacket(user_id_t userId,
                                         const uint8_t *packet,
                                         size_t length) = 0;

  /**
   * Sends the media trasport control packet to all users
   * Currently, we only support send packet through video track.
   * @param packet The pointer to the packet.
   * @param length The packet size.
   *
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual int sendBroadcastMediaControlPacket(const uint8_t *packet, size_t length) = 0;

  virtual ~IMediaControlPacketSender() {}
};

/**
 * The IAudioSinkBase class.
 *
 * This is the base class for the audio sink. You can use this calss to implement your own sink
 * and add the sink to the audio track.
 */
class IAudioSinkBase : public RefCountInterface {
 public:
  /** Gets the audio frame.
   *
   * @param audioframe AudioFrame
   */
  virtual bool onAudioFrame(const AudioPcmFrame& audioFrame) = 0;

 protected:
  ~IAudioSinkBase() {}
};

/**
 * The IAudioFilterBase class.
 *
 * This is the base class for the audio filter. You can use this class to implement your own filter
 * and add the filter to the audio track.
 */
class IAudioFilterBase : public RefCountInterface {
 public:
  // Return false if filter decide to drop the frame.
  // Implementation should not change samples_per_channel_/sample_rate_hz_/num_channels_
  // Implementation should just adapt data of audio frame.
  /**
   * Adapts the audio frame.
   * @param inAudioFrame The reference to the audio frame that you want to adapt: AudioPcmFrame.
   * @param adaptedFrame The reference to the adapted audio frame: AudioPcmFrame.
   * @return
   * - true: Success.
   * - false: Failure. For example, the IAudiofilter object drops the audio frame.
   */
  virtual bool adaptAudioFrame(const AudioPcmFrame& inAudioFrame,
                               AudioPcmFrame& adaptedFrame) = 0;
 protected:
  ~IAudioFilterBase() {}
};

/**
 * The IAudioFilter class.
 *
 * This class is the intermediate node for audio, which reads audio frames from the underlying
 * pipeline and writes audio frames back after adaptation.
 */
class IAudioFilter : public IAudioFilterBase {
 public:
  /**
   * Enables or disables the audio filter.
   * @param enable Determines whether to enable the audio filter:
   * - true: Enable the audio filter.
   * - false: Do not enable the audio filter.
   */
  virtual void setEnabled(bool enable) = 0;
  /**
   * Checks whether the audio filter is enabled.
   * @return
   * - true: The audio filter is enabled.
   * - false: The audio filter is not enabled.
   */
  virtual bool isEnabled() const = 0;
  /**
   * Sets a private property in the IAudioFilter class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int setProperty(const char* key, const void* buf, int buf_size) = 0;
  /**
   * Gets a private property in the IAudioFilter class.
   *
   * @param name The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int getProperty(const char* key, void* buf, int buf_size) const = 0;
  /**
   * Gets a name in the IAudioFilter class.
   *
   * @return
   * - The name of the audio filter, if the method call succeeds.
   * - "", if the method call fails.
   */
  virtual const char * getName() const = 0;

 protected:
  ~IAudioFilter() {}
};

/**
 * The IVideoFrameSender class.
 *
 * In scenarios involving custom video source, you can use this class to push the video
 * data directly to the video track. If the video track is disabled, the pushed data will
 * be automatically discarded.
 */
class IVideoFrameSender : public RefCountInterface {
 public:
  /**
   * Sends the video frame to the video track.
   *
   * @param videoFrame The reference to the video frame to be sent: \ref
   * agora::media::ExternalVideoFrame "ExternalVideoFrame".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendVideoFrame(const agora::media::ExternalVideoFrame& videoFrame) = 0;

 protected:
  ~IVideoFrameSender() {}
};

/**
 * The IVideoEncodedImageSender class.
 *
 * In scenarios involving custom video source, you can use this class to push the encoded video data
 * directly to the video track. If the video track is disabled, the pushed video image will be
 * automatically discarded.
 */
class IVideoEncodedImageSender : public RefCountInterface {
 public:
  /**
   * Sends the encoded video image to the video track.
   * @param imageBuffer The video buffer.
   * @param length The data length of the video data.
   * @param videoEncodedFrameInfo The reference to the information of the encoded video frame:
   * EncodedVideoFrameInfo.
   *
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual bool sendEncodedVideoImage(const uint8_t* imageBuffer, size_t length,
                                     const EncodedVideoFrameInfo& videoEncodedFrameInfo) = 0;

 protected:
  ~IVideoEncodedImageSender() {}
};

/**
 * The IVideoFilterBase class.
 *
 * This is the base class for the video filter. You can use this class to implement your own filter
 * and add the filter to the video track.
 */
class IVideoFilterBase : public RefCountInterface {
 public:
  /**
   * Adapts the video frame.
   *
   * @param capturedFrame The reference to the captured video frame that you want to adapt: \ref
   * agora::media::VideoFrame "VideoFrame".
   * @param adaptedFrame The reference to the adapted video frame: \ref agora::media::VideoFrame
   * "VideoFrame".
   *
   * @return
   * - true: Success.
   * - false: Failure, if, for example, the IVideofilter object drops the video frame.
   */
  virtual bool adaptVideoFrame(const agora::media::VideoFrame& capturedFrame,
                               agora::media::VideoFrame& adaptedFrame) = 0;
};
/**
 * The IVideoSinkBase class.
 *
 * This is the base class for the custom video sink.
 */
class IVideoSinkBase : public RefCountInterface {
 public:
  /**
   * Sets a private property in the IVideoFilter class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int setProperty(const char* key, const void* buf, int buf_size) { return -1; }
  /**
   * Gets a private property in the IVideoFilter class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int getProperty(const char* key, void* buf, int buf_size) { return -1; }
  /**
   * Occurs when the IVideoSinkBase object receives the video frame.
   */
  virtual int onFrame(const agora::media::VideoFrame& videoFrame) = 0;
  /**
   *  Used internally to distinguish external or internal sink.
   * Extenal application should not override this interface.
   */
  virtual bool isExternalSink() { return true; }
  /**
   * This function is invoked right before data stream starts.
   * Custom sink can override this function for initialization.
   * @return
   * - true, if initialization succeeds
   * - false, if initialization fails
   */
  virtual bool onDataStreamWillStart() { return true; }
   /**
   * This function is invoked right before data stream stops.
   * Custom sink can override this function for deinitialization.
   */
  virtual void onDataStreamWillStop() { }
};

// TODO(Bob): private inherit?
/**
 * The IVideoFilter class.
 *
 * This class is the intermediate node for video, which contains both the video source and the video
 * sink. It reads video frames from the underlying video pipeline and writes video frames back after
 * adaptation.
 */
class IVideoFilter : public IVideoFilterBase {
 public:
  /**
   * Enables or disables the video filter.
   * @param enable Determines whether to enable the video filter:
   * - true: (Default) Enable the video filter.
   * - false: Do not enable the video filter. If filter is disabled, frame will be passed without
   * adaption.
   */
  virtual void setEnabled(bool enable) = 0;
  /**
   * Checks whether the video filter is enabled.
   * @return
   * - true: The video filter is enabled.
   * - false: The video filter is not enabled.
   */
  virtual bool isEnabled() = 0;
  /**
   * Sets a private property in the IVideoFilter class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual size_t setProperty(const char* key, const void* buf, size_t buf_size) { return -1; }
  /**
   * Gets a private property in the IVideoFilter class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual size_t getProperty(const char* key, void* buf, size_t buf_size) { return -1; }
  /**
   * This function is invoked right before data stream starts.
   * Custom filter can override this function for initialization.
   * @return
   * - true, if initialization succeeds
   * - false, if initialization fails
   */
  virtual bool onDataStreamWillStart() { return true; }
   /**
   * This function is invoked right before data stream stops.
   * Custom filter can override this function for deinitialization.
   */
  virtual void onDataStreamWillStop() { }
};

/**
 * The IVideoBeautyFilter class.
 */
class IVideoBeautyFilter : public IVideoFilter {
 public:
  /**
   * The video image enhancement options.
   */
  struct BeautyOptions {
    /**
     * The lightening contrast level.
     */
    enum LIGHTENING_CONTRAST_LEVEL {
      /**
       * 0: Low contrast level.
       */
      LIGHTENING_CONTRAST_LOW = 0,
      /**
       * (Default) Normal contrast level.
       */
      LIGHTENING_CONTRAST_NORMAL,
      /**
       * High contrast level.
       */
      LIGHTENING_CONTRAST_HIGH
    };

    /**
     * The contrast level, usually used with `lighteningLevel` to brighten the video:
     * #LIGHTENING_CONTRAST_LEVEL.
     */
    LIGHTENING_CONTRAST_LEVEL lighteningContrastLevel;
  
    /**
     * The brightness level. The value ranges from 0.0 (original) to 1.0.
     */
    float lighteningLevel;

    /**
     * The sharpness level. The value ranges from 0.0 (original) to 1.0. This parameter is usually
     * used to remove blemishes.
     */
    float smoothnessLevel;

    /**
     * The redness level. The value ranges from 0.0 (original) to 1.0. This parameter adjusts the
     * red saturation level.
     */
    float rednessLevel;

    BeautyOptions(LIGHTENING_CONTRAST_LEVEL contrastLevel, float lightening, float smoothness,
                  float redness)
        : lighteningContrastLevel(contrastLevel),
          lighteningLevel(lightening),
          smoothnessLevel(smoothness),
          rednessLevel(redness) {}

    BeautyOptions()
        : lighteningContrastLevel(LIGHTENING_CONTRAST_NORMAL),
          lighteningLevel(0),
          smoothnessLevel(0),
          rednessLevel(0) {}
  };
  /**
   * Sets the beauty effect options.
   * @param enabled Determines whether to enable the beauty effect.
   * - true: Enable the beauty effect.
   * - false: Do not enable the beauty effect.
   * @param options The beauty effect options: BeautyOptions.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setBeautyEffectOptions(bool enabled, BeautyOptions options) = 0;
};
/**
 * The IVideoRenderer class.
 */
class IVideoRenderer : public IVideoSinkBase {
 public:
  /**
   * Sets the render mode.
   * @param renderMode The video render mode: #RENDER_MODE_TYPE.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRenderMode(RENDER_MODE_TYPE renderMode) = 0;
  /**
   * Sets whether to mirror the video.
   * @param mirror Determines whether to mirror the video:
   * - true: Mirror the video.
   * - false: Do not mirror the video.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMirror(bool mirror) = 0;
  /**
   * Sets the video display window.
   * @param view The pointer to the video display window.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setView(void* view) = 0;
  /**
   * Stops rendering the video view on the window.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unsetView() = 0;
};
/**
 * The IMediaNodeFactory class.
 */
class IMediaNodeFactory : public RefCountInterface {
 public:
  /**
   * Creates a PCM audio data sender.
   *
   * This method creates an IAudioPcmDataSender object, which can be used by createCustomAudioTrack
   * to send PCM audio data.
   *
   * @return
   * - The pointer to IAudioPcmDataSender, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IAudioPcmDataSender> createAudioPcmDataSender() = 0;

  /**
   * Creates an encoded audio data sender.
   *
   * This method creates an IAudioEncodedFrameSender object, which can be used by
   * createCustomAudioTrack() to send encoded audio data.
   *
   * @return
   * - The pointer to IAudioEncodedFrameSender, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IAudioEncodedFrameSender> createAudioEncodedFrameSender() = 0;

  /**
   * Creates a camera capturer.
   *
   * Once a camera capturer object is created, you can use the video data captured by the camera as
   * the custom video source.
   *
   * @return
   * - The pointer to ICameraCapturer, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<ICameraCapturer> createCameraCapturer() = 0;

  /**
   * Creates a screen capturer.
   *
   * Once a screen capturer object is created, you can use the screen video data as the custom video
   * source.
   *
   * @return
   * - The pointer to IScreenCapturer, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IScreenCapturer> createScreenCapturer() = 0;
  
   /**
   * Creates a video mixer.
   *
   * Once a video mixer object is created, you can use the video mixer data as the custom video
   * source.
   *
   * @return
   * - The pointer to IScreenCapturer, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IVideoMixerSource> createVideoMixer() = 0;

  /**
   * Creates a video frame sender.
   *
   * This method creates an IVideoFrameSender object, which can be used by createCustomVideoTrack to
   * send the custom video data.
   *
   * @return
   * - The pointer to IVideoFrameSender, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IVideoFrameSender> createVideoFrameSender() = 0;

  /**
   * Creates an encoded video image sender.
   *
   * This method creates an IVideoEncodedImageSender object, which can be used by
   * createCustomVideoTrack to send the encoded video data.
   *
   * @return
   * - The pointer to IVideoEncodedImageSender, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IVideoEncodedImageSender> createVideoEncodedImageSender() = 0;

  /**
   * Creates a built-in video renderer.
   *
   * @param view The video window view.
   * @return
   * - The pointer to IVideoRenderer, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   * - Note that IVideoRenderer is also an IVideoSinkBase except that it's not an extension sink on
   * purpos
   */
  virtual agora_refptr<IVideoRenderer> createVideoRenderer(agora::view_t view) = 0;

  /**
   * Creates a observable video sink
   *
   * This method creates an IVideoSinkBase object, which can be used to observer video
   *
   * @param observer The pointer to the observer
   * @param trackInfo The info of the track that needs observer
   * @return
   * - The pointer to IVideoSinkBase, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::IVideoSinkBase> createObservableVideoSink(
      agora::media::IVideoFrameObserver* observer, VideoTrackInfo trackInfo) = 0;

  /**
   * Creates a audio renderer for the extension.
   *
   * This method creates an IAudioFilter object, which can be used to filter the audio data from
   * inside extension.
   *
   * @param name The pointer to the name of the extension.
   * @param vendor The pointer to the name of the extension vendor
   * @return
   * - The pointer to IAudioFilter, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IAudioFilter> createAudioFilter(const char* name,
                                                       const char* vendor = nullptr) = 0;

  /**
   * Creates a video filter for the extension.
   *
   * This method creates an IVideoFilter object, which can be used to filter the video from inside
   * extendion.
   *
   * @param name The pointer to the name of the extension.
   * @param vendor The pointer to the name of the extension vendor
   * @return
   * - The pointer to IVideoFilter, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IVideoFilter> createVideoFilter(const char* name,
                                                       const char* vendor = nullptr) = 0;

  /**
   * Creates a video sink for the extension.
   *
   * This method creates an IVideoSinkBase object, which can be used to receive the video from
   * inside extendion.
   *
   * @param name The pointer to the name of the extension.
   * @param vendor The pointer to the name of the extension vendor
   * @return
   * - The pointer to IVideoSinkBase, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IVideoSinkBase> createVideoSink(const char* name,
                                                       const char* vendor = nullptr) = 0;

  /**
   * Creates a media player source object and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtc::IMediaPlayerSource "IMediaPlayerSource", if the method call
   * succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IMediaPlayerSource> createMediaPlayerSource() = 0;
  
  /**
   * Creates a media packet sender object and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtc::IMediaPacketSender "IMediaPacketSender", if the method call
   * succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<IMediaPacketSender> createMediaPacketSender() = 0;

 protected:
  ~IMediaNodeFactory() {}
};
}  // namespace rtc
}  // namespace agora
