//
//  Agora SDK
//
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "../AgoraBase.h"
#include "../AgoraMediaBase.h"
#include "../AgoraRefPtr.h"
#include "IAgoraService.h"

namespace agora {
namespace rtc {
class IMediaPlayerSourceObserver;

/**
 * The IMediaPlayerSource class provides access to a media player source. To playout multiple media sources simultaneously,
 * create multiple media player source objects.
 */
class IMediaPlayerSource : public RefCountInterface {
public:
  /**
   * Opens a media file with a specified URL.
   * @param url The URL of the media file that you want to play.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int open(const char* url, int64_t startPos) = 0;

  /**
   * Plays the media file.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int play() = 0;

  /**
   * Pauses playing the media file.
   */
  virtual int pause() = 0;

  /**
   * Stops playing the current media file.
   */
  virtual int stop() = 0;

  /**
   * Resumes playing the media file.
   */
  virtual int resume() = 0;

  /**
   * Sets the current playback position of the media file.
   * @param newPos The new playback position (ms).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int seek(int64_t newPos) = 0;

  /**
   * Gets the duration of the media file.
   * @param duration A reference to the duration of the media file.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDuration(int64_t& duration) = 0;

  /**
   * Gets the current playback position of the media file.
   * @param currentPosition A reference to the current playback position (ms).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlayPosition(int64_t& pos) = 0;

  virtual int getStreamCount(int64_t& count) = 0;

  virtual int getStreamInfo(int64_t index, agora::media::MediaStreamInfo* info) = 0;

  /**
   * Sets whether to loop the media file for playback.
   * @param looping Determines whether to loop the media file.
   * - true: Loop the media file.
   * - false: Do not loop the media file.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLooping(bool looping) = 0;

  virtual agora::media::MEDIA_PLAYER_STATE getState() = 0;

  /**
   * Registers a media player source observer.
   *
   * Once the media player source observer is registered, you can use the observer to monitor the state change of the media player.
   * @param observer The pointer to the IMediaPlayerSourceObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerPlayerObserver(IMediaPlayerSourceObserver* observer) = 0;

  /**
   * Releases the media player source observer.
   * @param observer The pointer to the IMediaPlayerSourceObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterPlayerObserver(IMediaPlayerSourceObserver* observer) = 0;

  /**
   * Register the audio frame observer.
   *
   * @param observer The pointer to the IAudioFrameObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(IAudioFrameObserver* observer) = 0;

  /**
   * Releases the audio frame observer.
   * @param observer The pointer to the IAudioFrameObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterAudioFrameObserver(IAudioFrameObserver* observer) = 0;

protected:
  virtual ~IMediaPlayerSource() {}
};

class IMediaPlayerSourceObserver {
 public:
  /**
   * @brief Triggered when the player state changes
   *
   * @param state New player state
   * @param ec Player error message
   */
  virtual void onPlayerSourceStateChanged(const agora::media::MEDIA_PLAYER_STATE state,
                                    const agora::media::MEDIA_PLAYER_ERROR ec) = 0;

  /**
   * @brief Triggered when the player progress changes, once every 1 second
   *
   * @param position Current playback progress, in seconds
   */
  virtual void onPositionChanged(const int64_t position) = 0;

  /**
   * @brief Triggered when the player have some event
   *
   * @param event
   */
  virtual void onPlayerEvent(const agora::media::MEDIA_PLAYER_EVENT event) = 0;

  /**
   * @brief Triggered when meta data is obtained
   *
   * @param data meta data
   * @param length  meta data length
   */
  virtual void onMetaData(const void* data, int length) = 0;

  virtual ~IMediaPlayerSourceObserver() {}
};

} //namespace rtc
} // namespace agora
