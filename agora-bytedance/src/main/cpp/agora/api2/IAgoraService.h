//  Agora SDK
//
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "../AgoraBase.h"
#include "optional.h"

namespace agora {
namespace rtc {
class IRtcConnection;
class ILocalUser;
class IMediaDeviceManager;
class INGAudioDeviceManager;
struct TConnectionInfo;
struct RtcConnectionConfiguration;
class ILocalAudioTrack;
class IMediaPlayer;
class IMediaPlayerSource;
class ICameraCapturer;
class IScreenCapturer;
class IAudioPcmDataSender;
class IAudioEncodedFrameSender;
class IVideoFrameSender;
class IVideoEncodedImageSender;
class IVideoSourceNode;
class IVideoMixerSource;
class ILocalVideoTrack;
class IMediaNodeFactory;
class IRtmpStreamingService;
class IMediaPacketSender;
class IExtensionControl;

class IRtcEngine;
/**
 * The audio encoder configuration.
 */
struct AudioEncoderConfiguration {
  /**
   * The audio profile: #AUDIO_PROFILE_TYPE
   */
  AUDIO_PROFILE_TYPE audioProfile = AUDIO_PROFILE_DEFAULT;
  // absl::optional<DtxStatus> dtx;
  // double bitrate_priority = 1.0;
  // absl::optional<int> ptime;
  // FEC parameters
  // Rtx parameters
};

}  // namespace rtc

namespace signaling {
class ISignalingEngine;
}

namespace rtm {
class IRtmService;
}

namespace base {
class IEngineBase;

/**
 * The media engine type.
 */
enum TMediaEngineType { MEDIA_ENGINE_WEBRTC, MEDIA_ENGINE_EMPTY, MEDIA_ENGINE_UNKNOWN };

/**
 * The global configuration for the Agora service.
 */
struct AgoraServiceConfiguration {
  /**
   * The media engine type: #TMediaEngineType. The default value is `MEDIA_ENGINE_WEBRTC`.
   */
  TMediaEngineType engineType = MEDIA_ENGINE_WEBRTC;
  /**
   * Determines whether to enable the audio processor.
   * - `true`: (Default) Enable the audio processor. Once enabled, the underlying
   * audio processor is initialized in advance.
   * - `false`: Disable the audio processor. Set this member
   * as `false` if you do not need audio at all.
   */
  bool enableAudioProcessor = true;
  /**
   * Determines whether to enable the audio device.
   * - `true`: (Default) Enable the audio device. Once enabled, the underlying
   * audio device module is initialized in advance to support audio
   * recording and playback.
   * - `false`: Disable the audio device. Set this member as `false` if
   * you do not need to record or play the audio.
   *
   * @note
   * When this member is set as `false`, and `enableAudioProcessor` is set as `true`,
   * you can still pull the PCM audio data.
   */
  bool enableAudioDevice = true;
  /**
   * Determines whether to enable video.
   * - `true`: Enable video. Once enabled, the underlying video engine is
   * initialized in advance.
   * - `false`: (Default) Disable video. Set this parameter as `false` if you
   * do not need video at all.
   */
  bool enableVideo = false;
  /**
   * The user context, for example, the activity context in Android.
   */
  void* context = nullptr;
  /**
   * The App ID of your project
   */
  const char* appId = nullptr;
};
/**
 * The global audio session configuration.
 */
struct AudioSessionConfiguration {
  /**
   * Determines whether to enable recording (input) and playback (output) of audio:
   * - `true`: Enable audio recording and playback.
   * - `false`: Disable audio recording or playback, which prevents audio input
   * and output.
   *
   * @note
   * - For the recording function to work, the user must grant permission for audio recording.
   * - By default, your app's audio is nonmixable, which means
   * activating audio sessions in your app interrupts other nonmixable audio sessions.
   * To allow mixing, set `allowMixWithOthers` as `true`.
   */
  Optional<bool> playbackAndRecord;
  /**
   * Determines whether to enable the chat mode:
   * - `true`: Enable the chat mode. Specify this mode is your app is engaging in two-way
   * real-time communication, such as a voice or video chat.
   * - `false`: Disable the chat mode.
   *
   * For a video chat, set this member as true and set the audio route to the speaker.
   */
  Optional<bool> chatMode;
  /**
   * Determines whether audio from this session defaults to the built-in speaker instead
   * of the receiver:
   * - `true`: Audio from this session defaults to the built-in speaker instead
   * of the receiver.
   * - `false`: Audio from this session does not default to the built-in speaker instead
   * of the receiver.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> defaultToSpeaker;
  /**
   * Determines whether to temporarily change the current audio route to the built-in speaker:
   * - `true`: Set the current audio route to the built-in speaker.
   * - `false`: Do not set the current audio route to the built-in speaker.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> overrideSpeaker;
  /**
   * Determines whether audio from this session is mixed with audio from active sessions
   * in other audio apps.
   * - `true`: Mix audio from this session with audio from active sessions in
   * other audio apps, that is, your app's audio is mixed with audio playing in background
   * apps.
   * - `false`: Do not mix audio from this session with audio from active sessions in
   * other audio apps.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> allowMixWithOthers;
  /**
   * Determines whether Bluetooth handsfree devices appear as available audio input
   * routes:
   * - `true`: Bluetooth handsfree devices appear as available audio input routes.
   * - `false`: Bluetooth handsfree devices do not appear as available audio input
   * routes.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> allowBluetooth;
  /**
   * Determines whether audio from the current session can be streamed to Bluetooth
   * devices that support the Advanced Audio Distribution Profile (A2DP).
   * - `true`: Audio from the current session can be streamed to Bluetooth devices that
   * support the Advanced Audio Distribution Profile (A2DP).
   * - `false`: Audio from the current session cannot be streamed to Bluetooth devices that
   * support the Advanced Audio Distribution Profile (A2DP).
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  Optional<bool> allowBluetoothA2DP;
  /**
   * Sets the preferred hardware sample rate (kHz) for the session. The value range is
   * [8, 48]. Depending on the hardware in use, the actual sample rate might be different.
   */
  Optional<double> sampleRate;
  /**
   * Sets the preferred hardware input and output buffer duration (ms) for the session.
   * The value range is [0, 93]. Depending on the hardware in use, the actual I/O buffer
   * duration might be lower.
   */
  Optional<double> ioBufferDuration;
  /**
   * Sets the preferred number of audio input channels for the current route.
   */
  Optional<int> inputNumberOfChannels;
  /**
   * Sets the preferred number of audio output channels for the current route.
   */
  Optional<int> outputNumberOfChannels;

  void SetAll(AudioSessionConfiguration& change) {
    SetFrom(&playbackAndRecord, change.playbackAndRecord);
    SetFrom(&chatMode, change.chatMode);
    SetFrom(&defaultToSpeaker, change.defaultToSpeaker);
    SetFrom(&overrideSpeaker, change.overrideSpeaker);
    SetFrom(&allowMixWithOthers, change.allowMixWithOthers);
    SetFrom(&allowBluetooth, change.allowBluetooth);
    SetFrom(&allowBluetoothA2DP, change.allowBluetoothA2DP);
    SetFrom(&sampleRate, change.sampleRate);
    SetFrom(&ioBufferDuration, change.ioBufferDuration);
    SetFrom(&inputNumberOfChannels, change.inputNumberOfChannels);
    SetFrom(&outputNumberOfChannels, change.outputNumberOfChannels);
  }

  bool operator==(const AudioSessionConfiguration& o) const {
    return playbackAndRecord == o.playbackAndRecord && chatMode == o.chatMode &&
           defaultToSpeaker == o.defaultToSpeaker && overrideSpeaker == o.overrideSpeaker &&
           allowMixWithOthers == o.allowMixWithOthers && allowBluetooth == o.allowBluetooth &&
           allowBluetoothA2DP == o.allowBluetoothA2DP && sampleRate == o.sampleRate &&
           ioBufferDuration == o.ioBufferDuration &&
           inputNumberOfChannels == o.inputNumberOfChannels &&
           outputNumberOfChannels == o.outputNumberOfChannels;
  }
  bool operator!=(const AudioSessionConfiguration& o) const { return !(*this == o); }

 private:
  template <typename T>
  static void SetFrom(Optional<T>* s, const Optional<T>& o) {
    if (o) {
      *s = o;
    }
  }
};
/**
 * The mixing mode options.
 */
enum TMixMode {
  /**
   * Mix all the audio streams in the channel.
   */
  MIX_ENABLED,
  /**
   * Do not mix the audio streams in the channel.
   */
  MIX_DISABLED,
};
/**
 * The CC mode options.
 */
enum TCcMode {
  /**
   * Enable the CC mode.
   */
  CC_ENABLED,
  /**
   * Do not enable the CC mode.
   */
  CC_DISABLED,
};

/**
 * The IAgoraService class.
 *
 * IAgoraService is the entry point of Agora Low Level APIs. Use this interface to
 * create access points to Agora Modules, including RTC Connection, Media Tracks,
 * Audio Device Manager and etc. In addition, in order to customize different user
 * scenarios, the interface allows application to configure service and media sessions
 * on the global level, for example, AgoraServiceConfiguration or AudioSessionConfiguration.
 */
class IAgoraService {
 public:
  /**
   * Initializes the Agora service object.
   * @param config The reference to Agora service configuration: AgoraServiceConfiguration.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - `ERR_INVALID_ARGUMENT`, if `context` in AgoraServiceConfiguration is not provided for
   * Android.
   *   - `ERR_INIT_NET_ENGINE`, if the event engine cannot be initialized. On Windows it is most
   * likely because connection to the local port is disabled by the firewall. In this case, turn off
   * and on the firewall again.
   */
  virtual int initialize(const AgoraServiceConfiguration& config) = 0;

  virtual int release() = 0;

  /**
   * Configures the preset audio session.
   * @param scenario The audio scenario option: \ref agora::rtc::AUDIO_SCENARIO_TYPE
   * "AUDIO_SCENARIO_TYPE".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioSessionPreset(agora::rtc::AUDIO_SCENARIO_TYPE scenario) = 0;

  /**
   * Sets the audio session configuration.
   * @param config The reference to the audio session configuration: AudioSessionConfiguration.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioSessionConfiguration(const AudioSessionConfiguration& config) = 0;

  /**
   * Gets the audio session configuration.
   * @param config The pointer to AudioSessionConfiguration.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getAudioSessionConfiguration(AudioSessionConfiguration* config) = 0;
  /**
   * Sets the SDK output log file path and size.
   *
   * The log file records all the log data during the SDK runtime. Ensure that the directory
   * for the log file exists and is writable.
   *
   * The Agora SDK has two log files, each with a default size of 512 KB. If you set
   * `fileSize` as 1024 KB, the SDK outputs log files with a total maximum size of 2 MB.
   * If the total size of the log files exceed the set value, the new output log files
   * overwrite the old output log files.
   *
   * @note
   * To ensure that the output log is complete, call this method immediately after calling
   * the \ref agora::base::IAgoraService::initialize "initialize" method.
   *
   * @param filePath The reference to the log file.
   * @param fileSize The SDK log file size (KB).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFile(const char* filePath, unsigned int fileSize) = 0;
  /**
   * Sets the SDK output log filter.
   *
   * You can use one or a combination of the log filter levels. The log level follows the
   * sequence of OFF, CRITICAL, ERROR, WARNING, INFO, and DEBUG. Choose a level to see the
   * logs preceding that level. For example, if you set the log level to WARNING, you can
   * see the logs within levels CRITICAL, ERROR, and WARNING.
   *
   * @param filters The log filter level.
   */
  virtual int setLogFilter(unsigned int filters) = 0;

  /**
   * Creates an RTC connection object and returns the pointer.
   * @param cfg The reference to the RTC connection configuration: RtcConnectionConfiguration.
   * @return
   * - The pointer to \ref rtc::IRtcConnection "IRtcConnection", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::IRtcConnection> createRtcConnection(
      const rtc::RtcConnectionConfiguration& cfg) = 0;

  /**
   * Creates a local audio track object and returns the pointer.
   *
   * By default, the audio track is created from the selected audio input device, for example,
   * the built-in microphone of the mobile device. For details, see IAudioDeviceManager for
   * ADM usage.
   *
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   * - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in AgoraServiceConfiguration.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createLocalAudioTrack() = 0;

  /**
   * Creates a local audio track object with the PCM data sender and returns the pointer.
   *
   * Once created, this track can be used to send PCM audio data.
   * @param audioSource The pointer to the PCM audio data sender: IAudioPcmDataSender.
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   * - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in AgoraServiceConfiguration.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IAudioPcmDataSender> audioSource) = 0;

  /**
   * Creates a local audio track object with the encoded audio data sender and returns the pointer.
   *
   * Once created, this track can be used to send encoded audio data, for example, the Opus frame.
   * @param audioSource The pointer to the encoded audio frame sender: IAudioEncoderFrameSender.
   * @param mixMode The mixing mode of the encoded audio in the channel: #TMixMode.
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   * - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in AgoraServiceConfiguration.
   */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IAudioEncodedFrameSender> audioSource, TMixMode mixMode) = 0;
  
  /**
   * Creates a local audio track object with the media packet sender and returns the pointer.
   *
   * Once created, this track can be used to send audio packet, for example, customized UDP/RTP packet.
   * @param source The pointer to the media packet sender: IMediaPacketSender.
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   * - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in AgoraServiceConfiguration.
  */
  virtual agora_refptr<rtc::ILocalAudioTrack> createCustomAudioTrack(
      agora_refptr<rtc::IMediaPacketSender> source) = 0;
  
  /**
   * Creates a local audio track object with the player source and returns the pointer.
   *
   * Once created, this track can be used to send PCM audio data decoded from a player.
   * @param audioSource The pointer to the player source: IMediaPlayerSource.
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   * - `INVALID_STATE`, if `enableAudioProcessor` is set as `false` in AgoraServiceConfiguration.
  */
  virtual agora_refptr<rtc::ILocalAudioTrack> createMediaPlayerAudioTrack(
      agora_refptr<rtc::IMediaPlayerSource> audioSource) = 0;

  /**
   * Creates an audio device manager object and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtc::INGAudioDeviceManager "INGAudioDeviceManager", if the method call
   * succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::INGAudioDeviceManager> createAudioDeviceManager() = 0;

  /**
   * Creates a media node factory object and returns the pointer.
   *
   * @return
   * - The pointer to \ref rtc::IMediaNodeFactory "IMediaNodeFactory", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::IMediaNodeFactory> createMediaNodeFactory() = 0;

  /**
   * Creates a local video track object with the camera capturer and returns the pointer.
   *
   * Once created, this track can be used to send video data captured by the camera.
   *
   * @param videoSource A pointer to the camera capturer: ICameraCapturer.
   * @param syncWithAudioTrack Determines whether to synchronize with the audio track:
   * - `true`: (Default) Synchronize with the audio track.
   * - `false`: Do not synchronize with the audio track.
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createCameraVideoTrack(
      agora_refptr<rtc::ICameraCapturer> videoSource, bool syncWithAudioTrack = true) = 0;

  /**
   * Creates a local video track object with the screen capturer and returns the pointer.
   *
   * Once created, this track can be used to send video data captured by the screen.
   *
   * @param videoSource The pointer to the screen capturer: IScreenCapturer.
   * @param syncWithAudioTrack Determines whether to synchronize with the audio track:
   * - true: Synchronize with the audio track.
   * - false: (Default) Do not synchronize with the audio track.
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createScreenVideoTrack(
      agora_refptr<rtc::IScreenCapturer> videoSource, bool syncWithAudioTrack = false) = 0;
  
   /**
   * Creates a local video track object with the video mixer and returns the pointer.
   *
   * Once created, this track can be used to send video data processed by the video mixer.
   *
   * @param videoSource The pointer to the video mixer: IVideoMixerSource.
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createMixedVideoTrack(agora_refptr<rtc::IVideoMixerSource> videoSource) = 0;

  /**
   * Creates a local video track object with the custom video source and returns the pointer.
   *
   * Once created, this track can be used to send video data captured from a customized source, such
   * as WebRTC.
   *
   * @param videoSource The pointer to the custom video frame sender: IVideoFrameSender.
   * @param syncWithAudioTrack Determines whether to synchronize with the audio track:
   * - `true`: Synchronize with the audio track.
   * - `false`: (Default) Do not synchronize with the audio track.
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(
      agora_refptr<rtc::IVideoFrameSender> videoSource, bool syncWithAudioTrack = false) = 0;

  /**
   * Creates a local video track object with the receiver for encoded video data and returns the
   * pointer.
   *
   * Once created, this track can be used to send encoded video data, for example, H.264 or VP8
   * frames.
   *
   * @param videoSource The pointer to the encoded video frame sender: IVideoEncodedImageSender.
   * @param syncWithAudioTrack Determines whether to synchronize with the audio track:
   * - `true`: Synchronize with the audio track.
   * - `false`: (Default) Do not synchronize with the audio track.
   * @param ccMode Determins whether to enable the CC mode: #TCcMode.
   *
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(
      agora_refptr<rtc::IVideoEncodedImageSender> videoSource, bool syncWithAudioTrack = false,
      TCcMode ccMode = CC_ENABLED) = 0;
  
  /**
   * Creates a local video track object with the media packet sender and returns the pointer.
   *
   * Once created, this track can be used to send video packet, for example, custom UDP/RTP packet.
   * @param source The pointer to the media packet sender: IMediaPacketSender.
   * @return
   * - The pointer to \ref rtc::ILocalVideoTrack "ILocalVideoTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
  */
  virtual agora_refptr<rtc::ILocalVideoTrack> createCustomVideoTrack(
      agora_refptr<rtc::IMediaPacketSender> source) = 0;
  
  /**
   * Creates a local video track object with the player source and returns the pointer.
   *
   * Once created, this track can be used to send YUV frame decoded from a player.
   * @param videoSource The pointer to the player source: IMediaPlayerSource.
   * @param syncWithAudioTrack Determines whether to synchronize with the audio track:
   * @return
   * - The pointer to \ref rtc::ILocalAudioTrack "ILocalAudioTrack", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
  */
  virtual agora_refptr<rtc::ILocalVideoTrack> createMediaPlayerVideoTrack(
      agora_refptr<rtc::IMediaPlayerSource> videoSource, bool syncWithAudioTrack = false) = 0;

  /**
   * Creates a live stream service object.
   * @param rtcConnection The pointer to \ref rtc::IRtcConnection "IRtcConnection".
   * @param appId The app ID of the live stream service.
   * @return
   * - The pointer to \ref rtc::IRtmpStreamingService "IRtmpStreamingService", if the method call
   * succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual agora_refptr<rtc::IRtmpStreamingService> createRtmpStreamingService(
      agora_refptr<rtc::IRtcConnection> rtcConnection, const char* appId) = 0;

  /**
   * Creates an RTM servive object.
   * @return
   * - The pointer to \ref rtm::IRtmService "IRtmService", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual rtm::IRtmService* createRtmService() = 0;

  /**
   * @return
   * - The pointer to \ref agora::rtc::IExtensionControl "IExtensionControl", if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual rtc::IExtensionControl* getExtensionControl() = 0;

 protected:
  virtual ~IAgoraService() {}
};

}  // namespace base
}  // namespace agora

/** \addtogroup createAgoraService
 @{
 */
/**
 * Creates an Agora service object and returns the pointer.
 * @return
 * - The pointer to \ref agora::base::IAgoraService "IAgoraService", if the method call succeeds.
 * - The empty pointer NULL, if the method call fails.
 */
AGORA_API agora::base::IAgoraService* AGORA_CALL createAgoraService();
/** @} */
