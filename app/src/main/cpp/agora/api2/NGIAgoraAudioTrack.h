
// Copyright (c) 2018 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "../AgoraRefPtr.h"
#include "IAgoraService.h"
#include "NGIAgoraMediaNodeFactory.h"

// FIXME(Ender): use this class instead of AudioSendStream as local track
namespace agora {
namespace rtc {
class IAudioTrackStateObserver;

// AudioSinkWants is used for notifying the source of properties a audio frame
// should have when it is delivered to a certain sink.
struct AudioSinkWants {
  AudioSinkWants() = default;
  AudioSinkWants(const AudioSinkWants&) = default;
  ~AudioSinkWants() = default;

  // Tells the source the sample rate the sink wants.
  int samplesPerSec;

  // Tells the source the number of audio channels the sink wants.
  size_t channels;
};

/**
 * The IAudioTrack class.
 */
class IAudioTrack : public RefCountInterface {
public:
  /**
   * The position of the audio filter.
   */
  enum AudioFilterPosition {
    Default
  };

 public:
  /**
   * Adjusts the playout volume.
   * @param volume The playout volume. The value ranges between 0 and 100 (default).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPlayoutVolume(int volume) = 0;

  /**
   * Gets the current playout volume.
   * @param volume A pointer to the playout volume.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlayoutVolume(int* volume) = 0;

  /**
   * Adds an audio filter.
   *
   * By adding an audio filter, you can apply various audio effects to the audio, for example, voice change.
   * @param filter A pointer to the audio filter: IAudioFilter.
   * @param position The position of the audio filter: AudioFilterPosition.
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual bool addAudioFilter(agora_refptr<IAudioFilter> filter, AudioFilterPosition position) = 0;
  /**
   * Removes the audio filter added by using addAudioFilter().
   *
   * @param filter The pointer to the audio filter that you want to remove: IAudioFilter.
   * @param position The position of the audio filter: AudioFilterPosition.
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual bool removeAudioFilter(agora_refptr<IAudioFilter> filter, AudioFilterPosition position) = 0;

  /**
   * Gets the audio filter by its name.
   *
   * @param name The name of the audio filter.
   * @return
   * - The pointer to the audio filter, if the method call succeeds.
   * - nullptr, if the method call fails.
   */
  virtual agora_refptr<IAudioFilter> getAudioFilter(const char *name) const = 0;

  /** Add audio sink to get PCM data from audio track.
   *
   * @param sink A pointer to the audio sink: IAudioSinkBase
   * @param wants The properties a audio frame should have when it is delivered to a the sink
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual bool addAudioSink(agora_refptr<IAudioSinkBase> sink, const AudioSinkWants& wants) = 0;

  /** Remove audio sink */
  virtual bool removeAudioSink(agora_refptr<IAudioSinkBase> sink) = 0;
};

/**
 * Statistics of the local audio track.
 */
struct LocalAudioTrackStats {
  /** The number of channels.
  */
  int num_channels;
  /** The sample rate (Hz).
   */
  int sent_sample_rate;
  /** The average sending bitrate (Kbps).
   */
  int sent_bitrate;
};

/**
 * The ILocalAudioTrack class.
 *
 * A local audio track can be created by \ref agora::base::IAgoraService::createLocalAudioTrack
 * "IAgoraService::createLocalAudioTrack". It originates from the default audio recording
 * device, that is, the built-in microphone. You can also use the APIs we provide in the
 * \ref agora::rtc::INGAudioDeviceManager "IAudioDeviceManager" class if multiple recording
 * devices are available in the system.
 *
 * After the local audio track is created, you can publish one or multiple local audio
 * tracks using \ref agora::rtc::ILocalUser::publishAudio "ILocalUser::publishAudio".
 */
class ILocalAudioTrack : public IAudioTrack {
 public:
  /**
   * The struct of LocalAudioTrackStats.
   */
  struct LocalAudioTrackStats {
    /**
     * The source ID of the local audio track.
     */
    uint32_t source_id;
    uint32_t buffered_pcm_data_list_size;
    uint32_t free_pcm_data_list_size;
    uint32_t missed_audio_frames;
    uint32_t sent_audio_frames;
    uint32_t pushed_audio_frames;
    uint32_t dropped_audio_frames;
    bool enabled;
  };

 public:
  /**
   * Enables or disables the local audio track.
   *
   * Once enabled, the SDK allows for local audio capturing, processing, and encoding.
   *
   * @param enable Determines whether to enable the audio track:
   * - true: Enable the local audio track.
   * - false: Disable the local audio track.
   */
  virtual void setEnabled(bool enable) = 0;

  /**
   * Gets whether the local audio track is enabled.
   * @return The enable state of the local audio track:
   * - true: The local track is enabled.
   * - false: The local track is not enabled.
   */
  virtual bool isEnabled() const = 0;

  /**
   * Gets the state of the local audio.
   * @return The state of the local audio: #LOCAL_AUDIO_STREAM_STATE, if the method call succeeds.
   */
  virtual LOCAL_AUDIO_STREAM_STATE getState() = 0;

  /**
   * Gets the statistics of the local audio track: LocalAudioTrackStats.
   * @return The statistics of the local audio: LocalAudioTrackStats, if the method call succeeds.
   */
  virtual LocalAudioTrackStats GetStats() = 0;

  /**
   * Adjusts the audio volume for publishing.
   *
   * @param volume The volume for publishing.  The value ranges between 0 and 100 (default).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPublishVolume(int volume) = 0;
  
  /**
   * Gets the current volume for publishing.
   * @param volume A pointer to the publishing volume.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPublishVolume(int* volume) = 0;

  /**
   * Enables or disables local playback.
   * @param enable Determines whether to enable local playback:
   * - true: Enable local playback.
   * - false: Disable local playback.
   */
  virtual int enableLocalPlayback(bool enable) = 0;
  /**
   * Enables or disables ear monitor.
   * @param enable Determines whether to enable ear monitor:
   * - true: Enable ear monitor.
   * - false: Do not enable ear monitor.
   * @param includeAudioFilter Determines whether to add audio filter to the ear monitor.
   * - true: Add audio filter.
   * - false: Do not add audio filter.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableEarMonitor(bool enable, bool includeAudioFilter) = 0;

 protected:
  ~ILocalAudioTrack() = default;
};

/**
 * The struct of RemoteAudioTrackStats.
 */
struct RemoteAudioTrackStats {
  /**
   * User ID of the remote user sending the audio streams.
   */
  uid_t uid;
  /**
   * Audio quality received by the user: #QUALITY_TYPE.
   */
  int quality;
  /**
   * @return Network delay (ms) from the sender to the receiver.
   */
  int network_transport_delay;
  /**
   * @return Network delay (ms) from the receiver to the jitter buffer.
   */
  int jitter_buffer_delay;
  /**
   * The audio frame loss rate in the reported interval.
   */
  int audio_loss_rate;
  /**
   * The number of channels.
   */
  int num_channels;
  /**
   * The sample rate (Hz) of the received audio stream in the reported interval.
   */
  int received_sample_rate;
  /**
   * The average bitrate (Kbps) of the received audio stream in the reported interval.
   * */
  int received_bitrate;
  /**
   * The total freeze time (ms) of the remote audio stream after the remote user joins the channel.
   * In a session, audio freeze occurs when the audio frame loss rate reaches 4%.
   * Agora uses 2 seconds as an audio piece unit to calculate the audio freeze time.
   * The total audio freeze time = The audio freeze number &times; 2 seconds
   */
  int total_frozen_time;
  /**
   * The total audio freeze time as a percentage (%) of the total time when the audio is available.
   * */
  int frozen_rate;
  /**
   * The number of audio bytes received.
   */
  int64_t received_bytes = 0;
};

/**
 * The IRemoteAudioTrack class.
 */
class IRemoteAudioTrack : public IAudioTrack {
 public:
  /**
   * Gets the statistics of the remote audio track.
   * @param stats A reference to the statistics of the remote audio track: RemoteAudioTrackStats.
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual bool getStatistics(RemoteAudioTrackStats& stats) = 0;

  /**
   * Gets the state of the remote audio.
   * @return The state of the remote audio: #REMOTE_AUDIO_STATE, if the method call succeeds.
   */
  virtual REMOTE_AUDIO_STATE getState() = 0;
  
  /**
   * Registers an IMediaPacketReceiver object.
   *
   * You need to implement the IMediaPacketReceiver class in this method. Once you successfully register
   * the media packet receiver, the SDK triggers the onMediaPacketReceived callback when it receives the
   * audio packet.
   *
   * @param packetReceiver The pointer to the IMediaPacketReceiver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerMediaPacketReceiver(IMediaPacketReceiver* packetReceiver) = 0;
  
  /**
   * Releases the IMediaPacketReceiver object.
   * @param packetReceiver The pointer to the IMediaPacketReceiver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterMediaPacketReceiver(IMediaPacketReceiver* packetReceiver) = 0;
};

}  // namespace rtc
}  // namespace agora
