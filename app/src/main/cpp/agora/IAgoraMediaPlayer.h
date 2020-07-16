//
//  Agora Rtc Engine SDK
//
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#ifndef AGORA_MEDIA_PLAYER_H_
#define AGORA_MEDIA_PLAYER_H_

#include "AgoraBase.h"
#include "AgoraMediaBase.h"

namespace agora {
namespace rtc {

/** Definition of MediaPlayerContext.
 */
struct MediaPlayerContext {
  /** User Context, i.e., activity context in Android.
   */
  void *context = nullptr;
};

class IMediaPlayerObserver;

/**
 * @brief Player interface
 *
 */
class IMediaPlayer {
 public:

  virtual int initialize(const MediaPlayerContext& context) = 0;

  /**
   * @brief Open media file
   *
   * @param src Media path, local path or network path
   * @param startPos Set the starting position for playback, in seconds
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int open(const char* src, int64_t startPos) = 0;

  /**
   * @brief Play
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int play() = 0;

  /**
   * @brief pause
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int pause() = 0;

  /**
   * @brief stop
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int stop() = 0;

  /**
   * @brief Play to a specified position
   *
   * @param pos The position to play, in seconds
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int seek(int64_t pos) = 0;

  /**
   * @brief Turn mute on or off
   *
   * @param mute Whether to mute on
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int mute(bool mute) = 0;

  /**
   * @brief Get mute state
   *
   * @param[out] mute Whether is mute on
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getMute(bool& mute) = 0;

  /**
   * @brief Adjust playback volume
   *
   * @param volume The volume value to be adjusted
   * The volume can be adjusted from 0 to 400:
   * 0: mute;
   * 100: original volume;
   * 400: Up to 4 times the original volume (with built-in overflow protection).
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int adjustPlayoutVolume(int volume) = 0;

  /**
   * @brief Get the current playback volume
   *
   * @param[out] volume
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getPlayoutVolume(int& volume) = 0;

  /**
   * @brief Get the current playback progress
   *
   * @param[out] pos Progress in seconds
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getPlayPosition(int64_t& pos) = 0;

  /**
   * @brief Get media duration
   *
   * @param[out] duration Duration in seconds
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getDuration(int64_t& duration) = 0;

  /**
   * @brief Get player state
   *
   * @return PLAYER_STATE
   */
  virtual agora::media::MEDIA_PLAYER_STATE getState() = 0;

  /**
   * @brief Get the streams info count in the media
   *
   * @param[out] count
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int getStreamCount(int64_t& count) = 0;

  /**
   * @brief Get the streams info by index
   *
   * @param[in] index, index
   * @param[out] info, stream info for return
   */
  virtual int getStreamInfo(int64_t index, agora::media::MediaStreamInfo* info) = 0;

  /**
   * @brief Set video rendering view
   *
   * @param view view object, windows platform is HWND
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int setView(agora::view_t view) = 0;

  /**
   * @brief Set video display mode
   *
   * @param renderMode Video display mode
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int setRenderMode(agora::rtc::RENDER_MODE_TYPE renderMode) = 0;

  /**
   * @brief Register the player observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int registerPlayerObserver(IMediaPlayerObserver* observer) = 0;

  /**
   * @brief Unregister the player observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int unregisterPlayerObserver(IMediaPlayerObserver* observer) = 0;

  /**
   * @brief Register the player observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int registerVideoFrameObserver(IVideoFrameObserver* observer) = 0;

  /**
   * @brief Unregister the player observer
   *
   * @param observer observer object
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int registerAudioFrameObserver(IAudioFrameObserver* observer) = 0;

  /**
   * @brief This method starts connecting to an Agora Channel
   * @param token The token to join the channel, if this is set to NULL, appID
   * passed in IAgoraService::initialize() will be used as static key.
   * @param channelId The identifier of channel specifies which channel you wish
   * to join.
   * @param userId Your identifier as local user. If a *uid* is not assigned (or set to null),
   * the SDK will return a *uid* in the IRtcConnection::onConnected "onConnected" callback.
   * The application must record and maintain the returned *uid* since the SDK does not do so.
   */
  virtual int connect(const char* token, const char* channelId, user_id_t userId) = 0;

  /** This method disconnects the current connection to the channel.
   * If this method returns successfully, the Connection state will change to
   * STATE_DISCONNECTED. You will be notified with the event onDisconnected().
   */
  virtual int disconnect() = 0;

  /**
   * @brief publish video stream
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int publishVideo() = 0;

  /**
   * @brief unpublish video stream
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int unpublishVideo() = 0;

  /**
   * @brief publish audio stream
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int publishAudio() = 0;

  /**
   * @brief unpublish audio stream
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int unpublishAudio() = 0;

  /**
   * @brief adjust publish signal volume
   *
   * @return int <= 0 On behalf of an error, the value corresponds to one of PLAYER_ERROR
   */
  virtual int adjustPublishSignalVolume(int volume) = 0;

  /**
   * @brief release IMediaPlayer object.
   *
   */
  virtual void release(bool sync = true) = 0;

  virtual ~IMediaPlayer() {}
};

class IMediaPlayerObserver {
 public:
  /**
   * @brief Triggered when the player state changes
   *
   * @param state New player state
   * @param ec Player error message
   */
  virtual void onPlayerStateChanged(agora::media::MEDIA_PLAYER_STATE state,
                                    agora::media::MEDIA_PLAYER_ERROR ec) = 0;

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
  virtual void onPlayerEvent(agora::media::MEDIA_PLAYER_EVENT event) = 0;

  /**
   * @brief Triggered when metadata is obtained
   *
   * @param type Metadata type
   * @param data data
   * @param length  data length
   */
  virtual void onMetadata(agora::media::MEDIA_PLAYER_METADATA_TYPE type, const uint8_t* data,
                                  uint32_t length) = 0;

  virtual ~IMediaPlayerObserver() {}
};

}  // namespace rtc
}  // namespace agora

/**
 * Creates an Agora media player object and returns the pointer.
 * @return
 * - The pointer to \ref agora::rtc::IMediaPlayer "IMediaPlayer", if the method call succeeds.
 * - The empty pointer NULL, if the method call fails.
 */
AGORA_API agora::rtc::IMediaPlayer* AGORA_CALL createAgoraMediaPlayer();

#endif  // AGORA_MEDIA_PLAYER_H_
