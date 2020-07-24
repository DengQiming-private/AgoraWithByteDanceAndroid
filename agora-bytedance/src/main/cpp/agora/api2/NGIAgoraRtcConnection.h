//
//  Agora SDK
//
//  Created by Sting Feng in 2018-01.
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "../AgoraBase.h"
#include "../AgoraRefPtr.h"
#include "NGIAgoraLocalUser.h"

namespace agora {
namespace rtc {
class IAudioEncodedFrameSender;
class IRtcConnectionObserver;
class INetworkObserver;
class IRtcConnection;
class IVideoEncodedImageSender;

/**
 * The information on the RTC Connection.
 */
struct TConnectionInfo {
  /**
   * ID of the RTC Connection.
   */
  conn_id_t id;
  /**
   * ID of the target channel. NULL if you did not call the connect
   * method.
   */
  util::AString channelId;
  /**
   * The state of the current connection: #CONNECTION_STATE_TYPE.
   */
  CONNECTION_STATE_TYPE state;
  /**
   * ID of the local user.
   */
  util::AString localUserId;

  TConnectionInfo() : id(-1), channelId(nullptr), state(CONNECTION_STATE_DISCONNECTED), localUserId(nullptr) {}
};

/**
 * The audio subscription options.
 */
struct AudioSubscriptionOptions {
  AudioSubscriptionOptions() :
    packetOnly(false),
    pcmDataOnly(false),
    bytesPerSample(0),
    numberOfChannels(0),
    sampleRateHz(0) {
  }

  AudioSubscriptionOptions(const AudioSubscriptionOptions &rhs) :
      packetOnly(rhs.packetOnly),
      pcmDataOnly(rhs.pcmDataOnly),
      bytesPerSample(rhs.bytesPerSample),
      numberOfChannels(rhs.numberOfChannels),
      sampleRateHz(rhs.sampleRateHz) {
  }
  /**
   * Determines whether to subscribe to audio packet only, i.e., RTP packet.
   * - true: Subscribe to audio packet only, which means that the remote audio stream
   * is not be decoded at all. You can use this mode to receive audio packet and handle it
   * in applicatin.
   * Note: if set to true, other fileds in AudioSubscriptionOptions will be ignored.
   * - false: Do not subscribe to audio packet only, which means that the remote audio stream
   * is decoded automatically.
  */
  bool packetOnly;
  /**
   * Determines whether to subscribe to PCM audio data only.
   * - true: Subscribe to PCM audio data only, which means that the remote audio stream
   * is not be played by the built-in playback device automatically. You can use this
   * mode to pull PCM data and handle playback.
   * - false: Do not subscribe to PCM audio only, which means that the remote audio stream
   * is played automatically.
   */
  bool pcmDataOnly;
  /**
   * The number of bytes that you expect for each audio sample.
   */
  size_t bytesPerSample;
  /**
   * The number of audio channels that you expect.
   */
  size_t numberOfChannels;
  /**
   * The audio sample rate (Hz) that you expect.
   */
  uint32_t sampleRateHz;
};

enum RECV_TYPE : uint8_t {
  /**
   * Receive the packet as a media packet that just processed by Agora media pipeline.
   */
  RECV_MEDIA_ONLY = 0,
  /**
   * The received packet will just be delivered to user by a receiver that registered by user.
   */
  RECV_PACKET_ONLY,
  /**
   * The received packet will be delivered to user by a receiver while be processed by Agora media pipeline.
   */
  RECV_PACKET_AND_MEDIA,
};

/**
 * Configurations for the RTC connection.
 */
struct RtcConnectionConfiguration {
  /**
   * Determines whether to subscribe to all audio streams automatically.
   * - true: (Default) Subscribe to all audio streams automatically.
   * - false: Do not subscribe to any audio stream automatically.
   */
  bool autoSubscribeAudio;
  /**
   * Determines whether to subscribe to all video streams automatically.
   * - true: (Default) Subscribe to all video streams automatically.
   * - false: Do not subscribe to any video stream automatically.
   */
  bool autoSubscribeVideo;
  /**
   * The maximum sending bitrate.
   */
  int maxSendBitrate;
  /**
   * The minimum port.
   */
  int minPort;
  /**
   * The maximum port.
   */
  int maxPort;
  /**
   * The options for audio subscription: AudioSubscriptionOptions.
   */
  AudioSubscriptionOptions audioSubscriptionOptions;
  /**
   * The role of the user: #CLIENT_ROLE_TYPE. The default user role is CLIENT_ROLE_AUDIENCE.
   */
  CLIENT_ROLE_TYPE clientRoleType;

  CHANNEL_PROFILE_TYPE  channelProfile;

  /**
   * Determine the receiving type.
   */
  RECV_TYPE recvType;

  RtcConnectionConfiguration()
      : autoSubscribeAudio(true),
        autoSubscribeVideo(true),
        maxSendBitrate(-1),
        minPort(0),
        maxPort(0),
        clientRoleType(CLIENT_ROLE_AUDIENCE),
        channelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING),
        recvType(RECV_MEDIA_ONLY) {}
};

/**
 * The IRtcConnection class.
 *
 * With this class, an app can establish a connection to an Agora Channel.
 * Once connected, the app gets an AgoraLocalUser object, which
 * can publish and subscribe to media streams in the Agora Channel.
 *
 * Connecting to a channel is done asynchronous, and apps can listen for the
 * connection states or events with IRtcConnectionObserver.
 *
 * IRtcConnection also monitors remote users in the channel. Once a remote user
 * joins or leaves a channel, the app will also be notified.
 */
class IRtcConnection : public RefCountInterface {
 public:
  /**
   * Connects to an Agora channel.
   *
   * When the method call succeeds, the RTC connection state changes to
   * CONNECTION_STATE_CONNECTING(2).
   *
   * Depending on the success or failure of the connection establishment, the RTC
   * connection will make a second state transition to either
   * CONNECTION_STATE_CONNECTED(3) or CONNECTION_STATE_FAILED(5). You will also be notified with the either
   * onConnected() or onDisconnected().
   *
   * @param token The token for authentication. If you set this parameter to NULL, the SDK
   * uses the app ID you passed in \ref agora::base::IAgoraService::initialize "initialize" for
   * authentication. Note that the token you set should correspond to the role of the local
   * user, or the connection fails with the \ref IRtcConnectionObserver::onConnectionFailure
   * "onConnectionFailure" callback.
   * @param channelId The unique channel name for an Agora RTC connection. It must be in the
   * string format and not exceed 64 bytes in length. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+",
   * "-", ":", ";", "<", "=",
   * ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
   * @param userId ID of the local user. If a userId is not assigned, or set to null,
   * the SDK returns a userId in the \ref IRtcConnectionObserver::onConnected "onConnected"
   * callback. Your app must record and maintain the returned userId since the SDK does not do so.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT: The passed in argument is invalid.
   *   - ERR_INVALID_STATE: The current connection state is not CONNECTION_STATE_DISCONNECTED(1).
   */
  virtual int connect(const char* token, const char* channelId, user_id_t userId) = 0;

  /**
   * Disconnects from the channel.
   *
   * A successful call of this method changes the connection state to
   * CONNECTION_STATE_DISCONNECTED(1). You will be notified with the callback
   * \ref onDisconnected "onDisconnected".
   */
  virtual int disconnect() = 0;

  /** Starts the last-mile network probe test.
   *
   * This method starts the last-mile network probe test before joining a channel to get the uplink and
   * downlink last-mile network statistics, including the bandwidth, packet loss, jitter, and
   * round-trip time (RTT).
   *
   * Call this method to check the uplink network quality before users join a channel or before an
   * audience switches to a host. Once this method is enabled, the SDK returns the following callbacks:
   * - \ref IRtcEngineEventHandler::onLastmileQuality "onLastmileQuality": the SDK triggers this
   * callback within two seconds depending on the network conditions. This callback rates the network
   * conditions and is more closely linked to the user experience.
   * - \ref IRtcEngineEventHandler::onLastmileProbeResult "onLastmileProbeResult": the SDK triggers
   * this callback within 30 seconds depending on the network conditions. This callback returns the
   * real-time statistics of the network conditions and is more objective.
   *
   * @note
   * - Do not call other methods before receiving the \ref IRtcEngineEventHandler::onLastmileQuality
   * "onLastmileQuality" and \ref IRtcEngineEventHandler::onLastmileProbeResult "onLastmileProbeResult"
   * callbacks. Otherwise, the callbacks may be interrupted.
   * - In the Live-broadcast profile, a host should not call this method after joining a channel.
   *
   * @param config Sets the configurations of the last-mile network probe test. See
   * LastmileProbeConfig.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startLastmileProbeTest(const LastmileProbeConfig& config) = 0;

  /** 
   * Stops the last-mile network probe test. 
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopLastmileProbeTest() = 0;

  /**
   * Renews the Token.
   *
   * The token expires after a certain period of time once the token schema is
   * enabled. When the `onError` callback reports the error ERR_TOKEN_EXPIRED (109)
   * your app must generate a new token and then call this method to
   * renew it. Otherwise, the SDK disconnects from the Agora channel.
   */
  virtual int renewToken(const char* token) = 0;

  /**
   * Gets the current connection information.
   *
   * @return
   * - A pointer to the TConnectionInfo object, if the method call succeeds.
   * - An empty pointer NULL, if the method call fails.
   */
  virtual TConnectionInfo getConnectionInfo() = 0;

  /**
   * Gets the ILocalUser object.
   *
   * @return
   * - A pointer to the ILocalUser object, if the method call succeeds.
   * - An empty pointer NULL, if the method call fails.
   */
  virtual ILocalUser* getLocalUser() = 0;

  /**
   * Gets the information of remote users.
   *
   * Once the user successfully join the channel, you can also get the information of remote users
   * with the onUserJoined callback of each remote user.
   *
   * @param users The reference to the UserList object, which contains the information of all users
   * in the channel.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRemoteUsers(UserList& users) = 0;

  /**
   * Gets the information of a specified user in the channel.
   *
   * @param userId ID of the user whose information you want to get.
   * @param userInfo The reference to the UserInfo object, which contains the information of a
   * specified user.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getUserInfo(user_id_t userId, UserInfo& userInfo) = 0;

  /**
   * Registers an IRtcConnectionObserver object.
   *
   * @param observer A pointer to the IRtcConnectionObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerObserver(IRtcConnectionObserver* observer) = 0;

  /**
   * Releases the registered IRtcConnectionObserver object.
   *
   * @param observer The pointer to the IRtcConnectionObserver object created by the \ref registerObserver
   * "registerObserver" method.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterObserver(IRtcConnectionObserver* observer) = 0;

  /**
   * Registers an INetworkObserver object.
   *
   * @param observer A pointer to the INetworkObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerNetworkObserver(INetworkObserver* observer) = 0;

  /**
   * Releases the registered INetworkObserver object.
   *
   * @param observer The pointer to the INetworkObserver object created by the \ref registerNetworkObserver
   * "registerNetworkObserver" method.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterNetworkObserver(INetworkObserver* observer) = 0;

  /**
   * Gets the ID of the current connection.
   *
   * @return
   * - The connection ID, if the method call succeeds.
   * - An empty pointer null, if the method call fails.
   */
  virtual conn_id_t getConnId() = 0;

  /**
   * Gets the transportation statistics of this connection.
   *
   * @return
   * - The pointer to RtcStats, which reports statistics of transportation of the current call, if the method call succeeds.
   * - An empty pointer null, if the method call fails.
   */
  virtual RtcStats getTransportStats() = 0;

  /**
   * Gets the IAgoraParameter object.
   *
   * @return
   * - The pointer to the \ref agora::base::IAgoraParameter "IAgoraParameter" object.
   * - An empty pointer null, if the method call fails.
   */
  virtual agora::base::IAgoraParameter* getAgoraParameter() = 0;

  /** Creates a data stream.
   *
   * Each user can create up to five data streams during the lifecycle of the RtcConnection.
   *
   * @note Set both the @p reliable and @p ordered parameters to true or false. Do not set one as true and the other as false.
   *
   * @param streamId Pointer to the ID of the created data stream.
   * @param reliable Sets whether or not the recipients are guaranteed to receive the data stream from the sender within five seconds:
   * - true: The recipients receive the data stream from the sender within five seconds. If the recipient does not receive the data stream within five seconds, an error is reported to the application.
   * - false: There is no guarantee that the recipients receive the data stream within five seconds and no error message is reported for any delay or missing data stream.
   * @param ordered Sets whether or not the recipients receive the data stream in the sent order:
   * - true: The recipients receive the data stream in the sent order.
   * - false: The recipients do not receive the data stream in the sent order.
   *
   * @return
   * - Returns 0: Success.
   * - < 0: Failure.
   */
  virtual int createDataStream(int* streamId, bool reliable, bool ordered) = 0;

  /** Sends data stream messages to all users in a channel.
   *
   * The SDK has the following restrictions on this method:
   * - Up to 30 packets can be sent per second in a channel with each packet having a maximum size of 1 kB.
   * - Each client can send up to 6 kB of data per second.
   * - Each user can have up to five data streams simultaneously.
   *
   * @param  streamId  ID of the sent data stream, returned in the \ref IRtcEngine::createDataStream "createDataStream" method.
   * @param data Pointer to the sent data.
   * @param length Length of the sent data.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendStreamMessage(int streamId, const char* data, size_t length) = 0;

 protected:
  ~IRtcConnection() = default;
};

/**
 * The IRtcConnectionObserver class, which observes the connection state of the SDK.
 */
class IRtcConnectionObserver {
 public:
  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to CONNECTION_STATE_CONNECTED(3).
   *
   * @param connectionInfo The information of the current connection: TConnectionInfo.
   * @param reason The reason for the change of connection state to connected: CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onConnected(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to CONNECTION_STATE_DISCONNECTED(1).
   *
   * @param connectionInfo The information of the current connection: TConnectionInfo.
   * @param reason The reason for the change of connection state to disconnected: CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onDisconnected(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to CONNECTION_STATE_RECONNECTING(4).
   *
   * @param connectionInfo The information of the current connection: TConnectionInfo.
   * @param reason The reason for the change of connection state to reconnecting: CONNECTION_CHANGED_REASON_TYPE.
   */
  virtual void onReconnecting(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  // This should be deleted. onConnected is enough.
  virtual void onReconnected(const TConnectionInfo& connectionInfo, CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when the SDK loses connection with the Agora channel.
   *
   * @param connectionInfo The information of the current connection: IConnectionInfo.
   */
  virtual void onConnectionLost(const TConnectionInfo& connectionInfo) = 0;

  /**
   * Reports the quality of the lastmile network.
   *
   * After you successfully call the \ref IRtcConnection::startLastmileProbeTest "startLastmileProbeTest" method,
   * the SDK triggers this callback to report the result of the last-mile network probe test.
   *
   * @param quality Quality of the lastmile network: #QUALITY_TYPE.
   */
  virtual void onLastmileQuality(const QUALITY_TYPE quality) = 0;

  /** 
   * Reports the last-mile network probe result.
   *
   * The SDK triggers this callback within 30 seconds after the app calls the \ref agora::rtc::IRtcEngine::startLastmileProbeTest "startLastmileProbeTest" method.
   *
   * @param result The uplink and downlink last-mile network probe test result. See LastmileProbeResult.
   */
  virtual void onLastmileProbeResult(const LastmileProbeResult& result) = 0;

  /**
   * Occurs when the token privilege expires in 30 seconds.
   *
   * The SDK triggers this callback to remind the app to get a new token before the token privilege expires.
   *
   * Upon receiving this callback, you must generate a new token on your server and call the \ref IRtcConnection::renewToken
   * "renewToken" method to pass the new token to the SDK.
   *
   * @param token The token that expires in 30 seconds.
   */
  virtual void onTokenPrivilegeWillExpire(const char* token) = 0;

  /**
   * Occurs when the token privilege has expired.
   *
   * Upon receiving this callback, you must generate a new token on your server and call the \ref IRtcConnection::renewToken
   * "renewToken" method to pass the new token to the SDK.
   */
  virtual void onTokenPrivilegeDidExpire() = 0;

  /**
   * Occurs when the connection state between the SDK and the Agora channel changes to CONNECTION_STATE_FAILED(5).
   *
   * @param connectionInfo The connection information: TConnectionInfo.
   * @param reason The reason for the connection failure.
   */
  virtual void onConnectionFailure(const TConnectionInfo& connectionInfo,
                                   CONNECTION_CHANGED_REASON_TYPE reason) = 0;

  /**
   * Occurs when a remote user has successfully joined the channel.
   *
   * You can get the ID of the remote user in this callback.
   * @param userId ID of the remote user who has joined the channel.
   */
  virtual void onUserJoined(user_id_t userId) = 0;

  /**
   * Occurs when a remote user has left the channel.
   *
   * You can know why the user leaves the channel with the `reason` parameter.
   * @param userId ID of the user who leaves the channel.
   * @param reason The reason why the remote user leaves the channel: #USER_OFFLINE_REASON_TYPE.
   */
  virtual void onUserLeft(user_id_t userId, USER_OFFLINE_REASON_TYPE reason) = 0;

  /**
   * Reports the transport statistics of the current connection.
   *
   * In CONNECTION_STATE_CONNECTED, this callback is triggered once every two seconds.
   *
   * @param stats A pointer to \ref rtc::RtcStats "RtcStats".
   */
  virtual void onTransportStats(const RtcStats& stats) = 0;

  /**
   * Occurs when the role of the local user changes.
   *
   * @param oldRole The previous role of the local user: \ref rtc::CLIENT_ROLE_TYPE "CLIENT_ROLE_TYPE".
   * @param newRole The current role of the local user: \ref rtc::CLIENT_ROLE_TYPE "CLIENT_ROLE_TYPE".
   */
  virtual void onChangeRoleSuccess(CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole) {
    (void)oldRole;
    (void)newRole;
  }

  /**
   * Occurs when the local user fails to change the user role.
   */
  virtual void onChangeRoleFailure() {}

  /**
   * Reports the network quality of each user.
   *
   * This callback is triggered once every two seconds to report the uplink and downlink network conditions
   * of each user, including the local user.
   *
   * @param userId ID of the user, whose network quality is reported in this callback. If `userId` is empty,
   * this callback reports the network quality of the local user.
   * @param txQuality The uplink network quality: #QUALITY_TYPE.
   * @param rxQuality The downlink network quality: #QUALITY_TYPE.
   */
  virtual void onUserNetworkQuality(user_id_t userId, QUALITY_TYPE txQuality,
                                    QUALITY_TYPE rxQuality) {
    (void)userId;
    (void)txQuality;
    (void)rxQuality;
  }

  /**
   * Occurs when an API method is executed.
   *
   * @param err The error code that the SDK reports when the method call fails. If the SDK reports 0,
   * the method call succeeds.
   * @param api The API method that is executed.
   * @param result The result of the method call.
   */
  virtual void onApiCallExecuted(int err, const char* api, const char* result) {
    (void)err;
    (void)api;
    (void)result;
  }

  /**
   * Reports the error code and error message.
   * @param error The error code: #ERROR_CODE_TYPE.
   * @param msg The error message.
   */
  virtual void onError(ERROR_CODE_TYPE error, const char* msg) {
    (void)error;
    (void)msg;
  }

  /**
   * Reports the warning code and warning message.
   * @param warning The warning code: #WARN_CODE_TYPE.
   * @param msg The warning message.
   */
  virtual void onWarning(WARN_CODE_TYPE warning, const char* msg) {
    (void)warning;
    (void)msg;
  }

  /**
   * Occurs when the state of the channel media relay changes.
   *
   * After you successfully call the `startChannelMediaRelay` method, the SDK triggers this callback to report
   * the state of the media relay.
   *
   * @param state The state code of the channel media relay.
   * @param code The error code of the channel media relay.
   */
  virtual void onChannelMediaRelayStateChanged(int state, int code) = 0;
//  virtual void onChannelMediaRelayEvent(int code) = 0;

  virtual void onStreamMessage(user_id_t userId, int streamId, const char* data,
                               size_t length){};
  virtual void onStreamMessageError(user_id_t userId, int streamId, int code, int missed,
                                    int cached){};

  virtual ~IRtcConnectionObserver() {}
};

class INetworkObserver {
 public:
  virtual ~INetworkObserver() {}

 public:
  virtual void onBandwidthEstimationUpdated(int target_bitrate_bps) {};
};

}  // namespace rtc
}  // namespace agora
