//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.
//

// This header file is included by both high level and low level APIs,
#pragma once  // NOLINT(build/header_guard)

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <cassert>
#include <limits>

#include "IAgoraParameter.h"
#include "AgoraMediaBase.h"
#include "AgoraRefPtr.h"

#define MAX_PATH_260 (260)

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#if defined(FEATURE_ENABLE_UT_SUPPORT)
#define AGORA_CALL
#define AGORA_API extern "C"
#else
#define AGORA_CALL __cdecl
#if defined(AGORARTC_EXPORT)
#define AGORA_API extern "C" __declspec(dllexport)
#else
#define AGORA_API extern "C" __declspec(dllimport)
#endif
#endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>

#define AGORA_API __attribute__((visibility("default"))) extern "C"
#define AGORA_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#define AGORA_API extern "C" __attribute__((visibility("default")))
#define AGORA_CALL
#else
#define AGORA_API extern "C"
#define AGORA_CALL
#endif

namespace agora {
namespace commons {
namespace cjson {
class JsonWrapper;
}  // namespace cjson
}  // namespace commons

using any_document_t = commons::cjson::JsonWrapper;

namespace base {
class IEngineBase;

class IParameterEngine {
 public:
  virtual int setParameters(const char* parameters) = 0;
  virtual int getParameters(const char* key, any_document_t& result) = 0;
  virtual ~IParameterEngine() {}
};
}  // namespace base

namespace util {

template <class T>
class AutoPtr {
 protected:
  typedef T value_type;
  typedef T* pointer_type;

 public:
  explicit AutoPtr(pointer_type p = 0) : ptr_(p) {}
  ~AutoPtr() {
    if (ptr_) ptr_->release();
  }
  operator bool() const { return ptr_ != (pointer_type)0; }
  value_type& operator*() const { return *get(); }

  pointer_type operator->() const { return get(); }

  pointer_type get() const { return ptr_; }

  pointer_type release() {
    pointer_type tmp = ptr_;
    ptr_ = 0;
    return tmp;
  }

  void reset(pointer_type ptr = 0) {
    if (ptr != ptr_ && ptr_) ptr_->release();
    ptr_ = ptr;
  }
  template <class C1, class C2>
  bool queryInterface(C1* c, C2 iid) {
    pointer_type p = NULL;
    if (c && !c->queryInterface(iid, reinterpret_cast<void**>(&p))) {
      reset(p);
    }
    return p != NULL;
  }

 private:
  AutoPtr(const AutoPtr&);
  AutoPtr& operator=(const AutoPtr&);

 private:
  pointer_type ptr_;
};

template <class T>
class CopyableAutoPtr : public AutoPtr<T> {
  typedef typename AutoPtr<T>::pointer_type pointer_type;

 public:
  explicit CopyableAutoPtr(pointer_type p = 0) : AutoPtr<T>(p) {}
  explicit CopyableAutoPtr(const CopyableAutoPtr& rhs) { this->reset(rhs.clone()); }
  CopyableAutoPtr& operator=(const CopyableAutoPtr& rhs) {
    if (this != &rhs) this->reset(rhs.clone());
    return *this;
  }
  pointer_type clone() const {
    if (!this->get()) return nullptr;
    return this->get()->clone();
  }
};

class IString {
 public:
  virtual bool empty() const = 0;
  virtual const char* c_str() = 0;
  virtual const char* data() = 0;
  virtual size_t length() = 0;
  virtual IString* clone() = 0;
  virtual void release() = 0;
  virtual ~IString() {}
};
typedef CopyableAutoPtr<IString> AString;

class IIterator {
 public:
  virtual void* current() = 0;
  virtual const void* const_current() const = 0;
  virtual bool next() = 0;
  virtual void release() = 0;
  virtual ~IIterator() {}
};

class IContainer {
 public:
  virtual IIterator* begin() = 0;
  virtual size_t size() const = 0;
  virtual void release() = 0;
  virtual ~IContainer() {}
};

template <class T>
class AOutputIterator {
  IIterator* p;

 public:
  typedef T value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  explicit AOutputIterator(IIterator* it = NULL) : p(it) {}
  ~AOutputIterator() {
    if (p) p->release();
  }
  AOutputIterator(const AOutputIterator& rhs) : p(rhs.p) {}
  AOutputIterator& operator++() {
    p->next();
    return *this;
  }
  bool operator==(const AOutputIterator& rhs) const {
    if (p && rhs.p)
      return p->current() == rhs.p->current();
    else
      return valid() == rhs.valid();
  }
  bool operator!=(const AOutputIterator& rhs) const { return !this->operator==(rhs); }
  reference operator*() { return *reinterpret_cast<pointer>(p->current()); }
  const_reference operator*() const { return *reinterpret_cast<const_pointer>(p->const_current()); }
  bool valid() const { return p && p->current() != NULL; }
};

template <class T>
class AList {
  IContainer* container;
  bool owner;

 public:
  typedef T value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef size_t size_type;
  typedef AOutputIterator<value_type> iterator;
  typedef const AOutputIterator<value_type> const_iterator;

 public:
  AList() : container(NULL), owner(false) {}
  AList(IContainer* c, bool take_ownership) : container(c), owner(take_ownership) {}
  ~AList() { reset(); }
  void reset(IContainer* c = NULL, bool take_ownership = false) {
    if (owner && container) container->release();
    container = c;
    owner = take_ownership;
  }
  iterator begin() { return container ? iterator(container->begin()) : iterator(NULL); }
  iterator end() { return iterator(NULL); }
  size_type size() const { return container ? container->size() : 0; }
  bool empty() const { return size() == 0; }
};

}  // namespace util

/**
 * The channel profile.
 */
enum CHANNEL_PROFILE_TYPE {
  /**
   * 0: (Default) Communication.
   * 
   * Use this profile when there are two users in the channel.
   */
  CHANNEL_PROFILE_COMMUNICATION = 0,
  /**
   * 1: Live Broadcast.
   * 
   * Use this profile when there are more than two users in the channel.
   */
  CHANNEL_PROFILE_LIVE_BROADCASTING = 1,
  /**
   * 2: Gaming.
   * @deprecated This profile is deprecated.
   */
  CHANNEL_PROFILE_GAME = 2,
  /**
   * 3: Cloud Gaming.
   * 
   * This profile is for the interactive streaming scenario, which is highly sensitive to end-to-end 
   * latency, and any delay in render impacts the end-user experience. 
   * These use cases prioritize reducing delay over any smoothing done at the receiver.
   * 
   * We recommend using this profile in scenarios there users need to interact with each other.
   */
  CHANNEL_PROFILE_CLOUD_GAMING = 3,
};

/**
 * The warning codes.
 */
enum WARN_CODE_TYPE {
  /**
   * 8: The specified view is invalid. Specify a view when using the video call
   * function.
   */
  WARN_INVALID_VIEW = 8,
  /**
   * 16: Fails to initialize the video function, probably due to a lack of
   * resources. Users cannot see the video though the voice communication is not
   * affected.
   */
  WARN_INIT_VIDEO = 16,
  /**
   * 20: The request is pending, usually because some module is not ready,
   * and the SDK postpones processing the request.
   */
  WARN_PENDING = 20,
  /**
   * 103: No channel resources are available, probably because the server cannot
   * allocate any channel resource.
   */
  WARN_NO_AVAILABLE_CHANNEL = 103,
  /**
   * 104: A timeout occurs when looking up the channel. When joining a channel,
   * the SDK looks up the specified channel. This warning usually occurs when the
   * network condition is too poor to connect to the server.
   */
  WARN_LOOKUP_CHANNEL_TIMEOUT = 104,
  /**
   * 105: The server rejects the request to look up the channel. The server
   * cannot process this request or the request is illegal.
   */
  WARN_LOOKUP_CHANNEL_REJECTED = 105,
  /**
   * 106: A timeout occurs when opening the channel. Once the specific channel
   * is found, the SDK opens the channel. This warning usually occurs when the
   * network condition is too poor to connect to the server.
   */
  WARN_OPEN_CHANNEL_TIMEOUT = 106,
  /**
   * 107: The server rejects the request to open the channel. The server
   * cannot process this request or the request is illegal.
   */
  WARN_OPEN_CHANNEL_REJECTED = 107,

  // sdk: 100~1000
  /**
   * 111: A timeout occurs when switching the live video.
   */
  WARN_SWITCH_LIVE_VIDEO_TIMEOUT = 111,
  /**
   * 118: A timeout occurs when setting the user role in the Live Broadcast
   * profile.
   */
  WARN_SET_CLIENT_ROLE_TIMEOUT = 118,
  /**
   * 121: The ticket to open the channel is invalid.
   */
  WARN_OPEN_CHANNEL_INVALID_TICKET = 121,
  /**
   * 122: Try connecting to another server.
   */
  WARN_OPEN_CHANNEL_TRY_NEXT_VOS = 122,
  WARN_CHANNEL_CONNECTION_UNRECOVERABLE = 131,  
  /**
   * 701: An error occurs in opening the audio mixing file.
   */
  WARN_AUDIO_MIXING_OPEN_ERROR = 701,
  /**
   * 1014: Audio Device Module: a warning occurs in the playback device.
   */
  WARN_ADM_RUNTIME_PLAYOUT_WARNING = 1014,
  /**
   * 1016: Audio Device Module: a warning occurs in the recording device.
   */
  WARN_ADM_RUNTIME_RECORDING_WARNING = 1016,
  /**
   * 1019: Audio Device Module: no valid audio data is collected.
   */
  WARN_ADM_RECORD_AUDIO_SILENCE = 1019,
  /**
   * 1020: Audio Device Module: the playback device fails to start.
   */
  WARN_ADM_PLAYOUT_MALFUNCTION = 1020,
  /**
   * 1021: Audio Device Module: the recording device fails to start.
   */
  WARN_ADM_RECORD_MALFUNCTION = 1021,
  WARN_ADM_IOS_CATEGORY_NOT_PLAYANDRECORD = 1029,
  WARN_ADM_IOS_SAMPLERATE_CHANGE = 1030,
  /**
   * 1031: Audio Device Module: the recorded audio volume is too low.
   */
  WARN_ADM_RECORD_AUDIO_LOWLEVEL = 1031,
  /**
   * 1032: Audio Device Module: the playback audio volume is too low.
   */
  WARN_ADM_PLAYOUT_AUDIO_LOWLEVEL = 1032,
  WARN_ADM_WINDOWS_NO_DATA_READY_EVENT = 1040,
  /**
   * 1051: Audio Device Module: howling is detected.
   */
  WARN_APM_HOWLING = 1051,
  /**
   * 1052: Audio Device Module: the device is in the glitch state.
   */
  WARN_ADM_GLITCH_STATE = 1052,
  /**
   * 1053: Audio Device Module: the settings are improper.
   */
  WARN_ADM_IMPROPER_SETTINGS = 1053,
  WARN_ADM_WIN_CORE_NO_RECORDING_DEVICE = 1322,
  WARN_ADM_WIN_CORE_NO_PLAYOUT_DEVICE = 1323,
  WARN_ADM_WIN_CORE_IMPROPER_CAPTURE_RELEASE = 1324,
};

/**
 * The error codes.
 */
enum ERROR_CODE_TYPE {
  /**
   * 0: No error occurs.
   */
  ERR_OK = 0,
  // 1~1000
  /**
   * 1: A general error occurs (no specified reason).
   */
  ERR_FAILED = 1,
  /**
   * 2: An invalid parameter is used. For example, the specific channel name
   * includes illegal characters.
   */
  ERR_INVALID_ARGUMENT = 2,
  /**
   * 3: The SDK module is not ready. Possible solutions:
   * - Check the audio device.
   * - Check the completeness of the application.
   * - Re-initialize the RTC engine.
   */
  ERR_NOT_READY = 3,
  /**
   * 4: The SDK does not support this function.
   */
  ERR_NOT_SUPPORTED = 4,
  /**
   * 5: The request is rejected. This is for internal SDK internal use only,
   * and it does not return to the application through any method or callback.
   */
  ERR_REFUSED = 5,
  /**
   * 6: The buffer size is not big enough to store the returned data.
   */
  ERR_BUFFER_TOO_SMALL = 6,
  /**
   * 7: The SDK is not initialized before calling this method.
   */
  ERR_NOT_INITIALIZED = 7,

  ERR_INVALID_STATE = 8,

  /**
   * 9: No permission. This is for internal SDK internal use only, and it does
   * not return to the application through any method or callback.
   */
  ERR_NO_PERMISSION = 9,
  /**
   * 10: An API timeout occurs. Some API methods require the SDK to return the
   * execution result, and this error occurs if the request takes too long
   * (more than 10 seconds) for the SDK to process.
   */
  ERR_TIMEDOUT = 10,
  /**
   * 11: The request is cancelled. This is for internal SDK internal use only,
   * and it does not return to the application through any method or callback.
   */
  ERR_CANCELED = 11,
  /**
   * 12: The method is called too often. This is for internal SDK internal use
   * only, and it does not return to the application through any method or
   * callback.
   */
  ERR_TOO_OFTEN = 12,
  /**
   * 13: The SDK fails to bind to the network socket. This is for internal SDK
   * use only, and it does not return to the application through any method or
   * callback.
   */
  ERR_BIND_SOCKET = 13,
  /**
   * 14: The network is unavailable. This is for internal SDK use only, and it
   * does not return to the application through any method or callback.
   */
  ERR_NET_DOWN = 14,
  /**
   * 15: No network buffers are available. This is for internal SDK internal
   * use only, and it does not return to the application through any method or
   * callback.
   */
  ERR_NET_NOBUFS = 15,
  /**
   * 17: The request to join the channel is rejected. This error usually occurs
   * when the user is already in the channel, and still calls the method to join
   * the channel, for example, \ref agora::rtc::IRtcEngine::joinChannel "joinChannel()".
   */
  ERR_JOIN_CHANNEL_REJECTED = 17,
  /**
   * 18: The request to leave the channel is rejected. This error usually
   * occurs when the user has already left the channel, and still calls the
   * method to leave the channel, for example, \ref agora::rtc::IRtcEngine::leaveChannel
   * "leaveChannel()".
   */
  ERR_LEAVE_CHANNEL_REJECTED = 18,
  /**
   * 19: Resources have been occupied and cannot be reused.
   */
  ERR_ALREADY_IN_USE = 19,
  /**
   * 20: The SDK gives up the request due to too many requests. This is for
   * internal SDK internal use only, and it will not return to the application
   * through any method or callback.
   */
  ERR_ABORTED = 20,
  /**
   * 21: In Windows, specific firewall settings can cause the SDK to fail to
   * initialize and crash.
   */
  ERR_INIT_NET_ENGINE = 21,
  /**
   * 22: The application uses too much of the system resources and the SDK
   * fails to allocate the resources.
   */
  ERR_RESOURCE_LIMITED = 22,
  /**
   * 101: The specified App ID is invalid. Please try to rejoin the channel
   * with a valid App ID.
   */
  ERR_INVALID_APP_ID = 101,
  /**
   * 102: The specified channel name is invalid. Please try to rejoin the
   * channel with a valid channel name.
   */
  ERR_INVALID_CHANNEL_NAME = 102,
  /**
   * 109: The Token expired due to one of the following reasons:
   * - Authorized timestamp expired: The timestamp is represented by the number
   * of seconds elapsed since 1/1/1970. The user can use the Token to access the
   * Agora service within five minutes after the token is generated. If the user
   * does not access the Agora service after five minutes, this token will no
   * longer be valid.
   * - Call expiration timestamp expired: The timestamp indicates the exact time
   * when a user can no longer use the Agora service (for example, when a user
   * is forced to leave an ongoing call). When the value is set for the call
   * expiration timestamp, it does not mean that the token will be expired, but
   * that the user will be kicked out of the channel.
   */
  ERR_TOKEN_EXPIRED = 109,
  /**
   * 110: The Token is invalid due to one of the following reasons:
   * - The App Certificate for the project is enabled on the Dashboard, but the
   * user is still using the App ID. Once the App Certificate is enabled, the
   * user must use a token.
   * - The uid field is mandatory, and users must set the same uid when setting
   * the uid parameter when calling \ref agora::rtc::IRtcEngine::joinChannel "joinChannel()".
   */
  ERR_INVALID_TOKEN = 110,
  /**
   * 111: The internet connection is interrupted. This applies to the Agora Web
   * SDK only.
   */
  ERR_CONNECTION_INTERRUPTED = 111,  // only used in web sdk
  /**
   * 112: The internet connection is lost. This applies to the Agora Web SDK
   * only.
   */
  ERR_CONNECTION_LOST = 112,  // only used in web sdk
  /**
   * 113: The user is not in the channel when calling the
   * \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage()" method.
   */
  ERR_NOT_IN_CHANNEL = 113,
  /**
   * 114: The data size is over 1024 bytes when the user calls the
   * \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage()" method.
   */
  ERR_SIZE_TOO_LARGE = 114,
  /**
   * 115: The bitrate of the sent data exceeds the limit of 6 Kbps when the
   * user calls the \ref agora::rtc::IRtcEngine::sendStreamMessage "sendStreamMessage()".
   */
  ERR_BITRATE_LIMIT = 115,
  /**
   * 116: Too many data streams (over 5 streams) are created when the user
   * calls the \ref agora::rtc::IRtcEngine::createDataStream "createDataStream()" method.
   */
  ERR_TOO_MANY_DATA_STREAMS = 116,
  /**
   * 117: The data stream transmission timed out.
   */
  ERR_STREAM_MESSAGE_TIMEOUT = 117,
  /**
   * 119: Switching roles fails. Please try to rejoin the channel.
   */
  ERR_SET_CLIENT_ROLE_NOT_AUTHORIZED = 119,
  /**
   * 120: Decryption fails. The user may have used a different encryption
   * password to join the channel. Check your settings or try rejoining the
   * channel.
   */
  ERR_DECRYPTION_FAILED = 120,
  ERR_INVALID_USER_ID = 121,
  /**
   * 123: The client is banned by the server.
   */
  ERR_CLIENT_IS_BANNED_BY_SERVER = 123,
  /**
   * 124: Incorrect watermark file parameter.
   */
  ERR_WATERMARK_PARAM = 124,
  /**
   * 125: Incorrect watermark file path.
   */
  ERR_WATERMARK_PATH = 125,
  /**
   * 126: Incorrect watermark file format.
   */
  ERR_WATERMARK_PNG = 126,
  /**
   * 127: Incorrect watermark file information.
   */
  ERR_WATERMARKR_INFO = 127,
  /**
   * 128: Incorrect watermark file data format.
   */
  ERR_WATERMARK_ARGB = 128,
  /**
   * 129: An error occurs in reading the watermark file.
   */
  ERR_WATERMARK_READ = 129,
  /**
   * 130: Encryption is enabled when the user calls the
   * \ref agora::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl()" method
   * (CDN live streaming does not support encrypted streams).
   */
  ERR_ENCRYPTED_STREAM_NOT_ALLOWED_PUBLISH = 130,

  // signaling: 400~600
  ERR_LOGOUT_OTHER = 400,          //
  ERR_LOGOUT_USER = 401,           // logout by user
  ERR_LOGOUT_NET = 402,            // network failure
  ERR_LOGOUT_KICKED = 403,         // login in other device
  ERR_LOGOUT_PACKET = 404,         //
  ERR_LOGOUT_TOKEN_EXPIRED = 405,  // token expired
  ERR_LOGOUT_OLDVERSION = 406,     //
  ERR_LOGOUT_TOKEN_WRONG = 407,
  ERR_LOGOUT_ALREADY_LOGOUT = 408,
  ERR_LOGIN_OTHER = 420,
  ERR_LOGIN_NET = 421,
  ERR_LOGIN_FAILED = 422,
  ERR_LOGIN_CANCELED = 423,
  ERR_LOGIN_TOKEN_EXPIRED = 424,
  ERR_LOGIN_OLD_VERSION = 425,
  ERR_LOGIN_TOKEN_WRONG = 426,
  ERR_LOGIN_TOKEN_KICKED = 427,
  ERR_LOGIN_ALREADY_LOGIN = 428,
  ERR_JOIN_CHANNEL_OTHER = 440,
  ERR_SEND_MESSAGE_OTHER = 440,
  ERR_SEND_MESSAGE_TIMEOUT = 441,
  ERR_QUERY_USERNUM_OTHER = 450,
  ERR_QUERY_USERNUM_TIMEOUT = 451,
  ERR_QUERY_USERNUM_BYUSER = 452,
  ERR_LEAVE_CHANNEL_OTHER = 460,
  ERR_LEAVE_CHANNEL_KICKED = 461,
  ERR_LEAVE_CHANNEL_BYUSER = 462,
  ERR_LEAVE_CHANNEL_LOGOUT = 463,
  ERR_LEAVE_CHANNEL_DISCONNECTED = 464,
  ERR_INVITE_OTHER = 470,
  ERR_INVITE_REINVITE = 471,
  ERR_INVITE_NET = 472,
  ERR_INVITE_PEER_OFFLINE = 473,
  ERR_INVITE_TIMEOUT = 474,
  ERR_INVITE_CANT_RECV = 475,

  // 1001~2000
  /**
   * 1001: Fails to load the media engine.
   */
  ERR_LOAD_MEDIA_ENGINE = 1001,
  /**
   * 1002: Fails to start the call after enabling the media engine.
   */
  ERR_START_CALL = 1002,
  /**
   * 1003: Fails to start the camera.
   */
  ERR_START_CAMERA = 1003,
  /**
   * 1004: Fails to start the video rendering module.
   */
  ERR_START_VIDEO_RENDER = 1004,
  /**
   * 1005: A general error occurs in the Audio Device Module (no specified
   * reason). Check if the audio device is used by another application, or try
   * rejoining the channel.
   */
  ERR_ADM_GENERAL_ERROR = 1005,
  /**
   * 1006: Audio Device Module: an error occurs in using the Java resources.
   */
  ERR_ADM_JAVA_RESOURCE = 1006,
  /**
   * 1007: Audio Device Module: an error occurs in setting the sampling
   * frequency.
   */
  ERR_ADM_SAMPLE_RATE = 1007,
  /**
   * 1008: Audio Device Module: an error occurs in initializing the playback
   * device.
   */
  ERR_ADM_INIT_PLAYOUT = 1008,
  /**
   * 1009: Audio Device Module: an error occurs in starting the playback device.
   */
  ERR_ADM_START_PLAYOUT = 1009,
  /**
   * 1010: Audio Device Module: an error occurs in stopping the playback device.
   */
  ERR_ADM_STOP_PLAYOUT = 1010,
  /**
   * 1011: Audio Device Module: an error occurs in initializing the recording
   * device.
   */
  ERR_ADM_INIT_RECORDING = 1011,
  /**
   * 1012: Audio Device Module: an error occurs in starting the recording device.
   */
  ERR_ADM_START_RECORDING = 1012,
  /**
   * 1013: Audio Device Module: an error occurs in stopping the recording device.
   */
  ERR_ADM_STOP_RECORDING = 1013,
  /**
   * 1015: Audio Device Module: a playback error occurs. Check your playback
   * device and try rejoining the channel.
   */
  ERR_ADM_RUNTIME_PLAYOUT_ERROR = 1015,
  /**
   * 1017: Audio Device Module: a recording error occurs.
   */
  ERR_ADM_RUNTIME_RECORDING_ERROR = 1017,
  /**
   * 1018: Audio Device Module: fails to record.
   */
  ERR_ADM_RECORD_AUDIO_FAILED = 1018,
  /**
   * 1022: Audio Device Module: an error occurs in initializing the loopback
   * device.
   */
  ERR_ADM_INIT_LOOPBACK = 1022,
  /**
   * 1023: Audio Device Module: an error occurs in starting the loopback
   * device.
   */
  ERR_ADM_START_LOOPBACK = 1023,
  /**
   * 1027: Audio Device Module: no recording permission. Please check if the
   * recording permission is granted.
   */
  ERR_ADM_NO_PERMISSION = 1027,
  ERR_ADM_RECORD_AUDIO_IS_ACTIVE = 1033,
  ERR_ADM_ANDROID_JNI_JAVA_RESOURCE = 1101,
  ERR_ADM_ANDROID_JNI_NO_RECORD_FREQUENCY = 1108,
  ERR_ADM_ANDROID_JNI_NO_PLAYBACK_FREQUENCY = 1109,
  ERR_ADM_ANDROID_JNI_JAVA_START_RECORD = 1111,
  ERR_ADM_ANDROID_JNI_JAVA_START_PLAYBACK = 1112,
  ERR_ADM_ANDROID_JNI_JAVA_RECORD_ERROR = 1115,
  ERR_ADM_ANDROID_OPENSL_CREATE_ENGINE = 1151,
  ERR_ADM_ANDROID_OPENSL_CREATE_AUDIO_RECORDER = 1153,
  ERR_ADM_ANDROID_OPENSL_START_RECORDER_THREAD = 1156,
  ERR_ADM_ANDROID_OPENSL_CREATE_AUDIO_PLAYER = 1157,
  ERR_ADM_ANDROID_OPENSL_START_PLAYER_THREAD = 1160,
  ERR_ADM_IOS_INPUT_NOT_AVAILABLE = 1201,
  ERR_ADM_IOS_ACTIVATE_SESSION_FAIL = 1206,
  ERR_ADM_IOS_VPIO_INIT_FAIL = 1210,
  ERR_ADM_IOS_VPIO_REINIT_FAIL = 1213,
  ERR_ADM_IOS_VPIO_RESTART_FAIL = 1214,
  ERR_ADM_IOS_SET_RENDER_CALLBACK_FAIL = 1219,
  ERR_ADM_IOS_SESSION_SAMPLERATR_ZERO = 1221,
  ERR_ADM_WIN_CORE_INIT = 1301,
  ERR_ADM_WIN_CORE_INIT_RECORDING = 1303,
  ERR_ADM_WIN_CORE_INIT_PLAYOUT = 1306,
  ERR_ADM_WIN_CORE_INIT_PLAYOUT_NULL = 1307,
  ERR_ADM_WIN_CORE_START_RECORDING = 1309,
  ERR_ADM_WIN_CORE_CREATE_REC_THREAD = 1311,
  ERR_ADM_WIN_CORE_CAPTURE_NOT_STARTUP = 1314,
  ERR_ADM_WIN_CORE_CREATE_RENDER_THREAD = 1319,
  ERR_ADM_WIN_CORE_RENDER_NOT_STARTUP = 1320,
  ERR_ADM_WIN_CORE_NO_RECORDING_DEVICE = 1322,
  ERR_ADM_WIN_CORE_NO_PLAYOUT_DEVICE = 1323,
  ERR_ADM_WIN_WAVE_INIT = 1351,
  ERR_ADM_WIN_WAVE_INIT_RECORDING = 1353,
  ERR_ADM_WIN_WAVE_INIT_MICROPHONE = 1354,
  ERR_ADM_WIN_WAVE_INIT_PLAYOUT = 1355,
  ERR_ADM_WIN_WAVE_INIT_SPEAKER = 1356,
  ERR_ADM_WIN_WAVE_START_RECORDING = 1357,
  ERR_ADM_WIN_WAVE_START_PLAYOUT = 1358,
  /**
   * 1359: Audio Device Module: no recording device.
   */
  ERR_ADM_NO_RECORDING_DEVICE = 1359,
  /**
   * 1360: Audio Device Module: no playback device.
   */
  ERR_ADM_NO_PLAYOUT_DEVICE = 1360,

  // VDM error code starts from 1500
  /**
   * 1501: Video Device Module: the camera is not authorized.
   */
  ERR_VDM_CAMERA_NOT_AUTHORIZED = 1501,

  // VDM error code starts from 1500
  /**
   * 1501: Video Device Module: the camera is in use.
   */
  ERR_VDM_WIN_DEVICE_IN_USE = 1502,

  // VCM error code starts from 1600
  /**
   * 1600: Video Device Module: an unknown error occurs.
   */
  ERR_VCM_UNKNOWN_ERROR = 1600,
  /**
   * 1601: Video Device Module: an error occurs in initializing the video
   * encoder.
   */
  ERR_VCM_ENCODER_INIT_ERROR = 1601,
  /**
   * 1602: Video Device Module: an error occurs in encoding.
   */
  ERR_VCM_ENCODER_ENCODE_ERROR = 1602,
  /**
   * 1603: Video Device Module: an error occurs in setting the video encoder.
   */
  ERR_VCM_ENCODER_SET_ERROR = 1603,
};

/**
 * Supported logging severities of SDK
 */
enum LOG_LEVEL {
  LOG_LEVEL_NONE = 0x0000,
  LOG_LEVEL_INFO = 0x0001,
  LOG_LEVEL_WARN = 0x0002,
  LOG_LEVEL_ERROR = 0x0004,
  LOG_LEVEL_FATAL = 0x0008,
  LOG_LEVEL_ALL = 0xFFFF,
};

// Doesn't contain anything beyond a type and message now, but will in the
// future as more errors are implemented.
class AgoraError {
 public:
  // Constructors.

  // Creates a "no error" error.
  AgoraError() {}
  explicit AgoraError(ERROR_CODE_TYPE type) : type_(type) {}
  // For performance, prefer using the constructor that takes a const char* if
  // the message is a static string.
  AgoraError(ERROR_CODE_TYPE type, const char* message) : type_(type), static_message_(message) {}

  // Delete the copy constructor and assignment operator; there aren't any use
  // cases where you should need to copy an AgoraError, as opposed to moving it.
  // Can revisit this decision if use cases arise in the future.
  AgoraError(const AgoraError& other) = delete;
  AgoraError& operator=(const AgoraError& other) = delete;

  // Move constructor and move-assignment operator.
  AgoraError(AgoraError&& other);
  AgoraError& operator=(AgoraError&& other);

  ~AgoraError();

  // Identical to default constructed error.
  //
  // Preferred over the default constructor for code readability.
  static AgoraError OK();

  // Error type.
  ERROR_CODE_TYPE type() const { return type_; }
  void setType(ERROR_CODE_TYPE type) { type_ = type; }

  // Human-readable message describing the error. Shouldn't be used for
  // anything but logging/diagnostics, since messages are not guaranteed to be
  // stable.
  const char* message() const;
  // For performance, prefer using the method that takes a const char* if the
  // message is a static string.
  void setMessage(const char* message);

  // Convenience method for situations where you only care whether or not an
  // error occurred.
  bool ok() const { return type_ == ERROR_CODE_TYPE::ERR_OK; }

 private:
  ERROR_CODE_TYPE type_ = ERROR_CODE_TYPE::ERR_OK;

  const char* static_message_ = "";
};

typedef const char* user_id_t;
typedef void* view_t;
namespace rtc {
typedef unsigned int uid_t;
typedef unsigned int track_id_t;
typedef unsigned int conn_id_t;

static const unsigned int DEFAULT_CONNECTION_ID = 0;
static const unsigned int DUMMY_CONNECTION_ID = (std::numeric_limits<unsigned int>::max)();
}  // namespace rtc
/**
 * The struct of UserInfo.
 */
struct UserInfo {
  /**
   * ID of the user.
   */
  util::AString userId;
  /**
   * Whether the user has enabled audio:
   * - true: The user has enabled audio.
   * - false: The user has disabled audio.
   */
  bool hasAudio;
  /**
   * Whether the user has enabled video:
   * - true: The user has enabled video.
   * - false: The user has disabled video.
   */
  bool hasVideo;
};

typedef util::AList<UserInfo> UserList;

// Shared between Agora Service and Rtc Engine
namespace rtc {

/**
 * Reasons for a user being offline.
 */
enum USER_OFFLINE_REASON_TYPE {
  /**
   * 0: The user leaves the current channel.
   */
  USER_OFFLINE_QUIT = 0,
  /**
   * 1: The SDK times out and the user drops offline because no data packet was received within a certain
   * period of time. If a user quits the call and the message is not passed to the SDK (due to an
   * unreliable channel), the SDK assumes that the user drops offline.
   */
  USER_OFFLINE_DROPPED = 1,
  /**
   * 2: (Live Broadcast only.) The user role switched from broadcaster to audience.
   */
  USER_OFFLINE_BECOME_AUDIENCE = 2,
};

enum INTERFACE_ID_TYPE {
  AGORA_IID_AUDIO_DEVICE_MANAGER = 1,
  AGORA_IID_VIDEO_DEVICE_MANAGER = 2,
  AGORA_IID_PARAMETER_ENGINE = 3,
  AGORA_IID_MEDIA_ENGINE = 4,
  AGORA_IID_AUDIO_ENGINE = 5,
  AGORA_IID_VIDEO_ENGINE = 6,
  AGORA_IID_RTC_CONNECTION = 7,
  AGORA_IID_SIGNALING_ENGINE = 8,
  AGORA_IID_MEDIA_ENGINE_REGULATOR = 9,
};

/**
 * The quality types.
 */
enum QUALITY_TYPE {
  /**
   * 0: The network quality is unknown.
   * @deprecated This member is deprecated.
   */
  QUALITY_UNKNOWN = 0,
  /**
   * 1: The quality is excellent.
   */
  QUALITY_EXCELLENT = 1,
  /**
   * 2: The quality is quite good, but the bitrate may be slightly
   * lower than excellent.
   */
  QUALITY_GOOD = 2,
  /**
   * 3: Users can feel the communication slightly impaired.
   */
  QUALITY_POOR = 3,
  /**
   * 4: Users cannot communicate smoothly.
   */
  QUALITY_BAD = 4,
  /**
   * 5: Users can barely communicate.
   */
  QUALITY_VBAD = 5,
  /**
   * 6: Users cannot communicate at all.
   */
  QUALITY_DOWN = 6,
  /**
   * 7: Users cannot detect the network quality. (Not in use).
   */
  QUALITY_UNSUPPORTED = 7,
  /**
   * 8: Detecting the network quality.
   */
  QUALITY_DETECTING
};

/**
 * Content fit mode.
 */
enum FIT_MODE_TYPE {
  /**
   * 1: Uniformly scale the video until it fills the visible boundaries (cropped).
   * One dimension of the video may have clipped contents.
   */
  MODE_COVER = 1,

  /**
   * 2: Uniformly scale the video until one of its dimension fits the boundary
   * (zoomed to fit). Areas that are not filled due to disparity in the aspect
   * ratio are filled with black.
   */
  MODE_CONTAIN = 2,
};

// enum for clockwise rotation.
/**
 * The clockwise rotation information.
 */
enum VIDEO_ORIENTATION {
  /**
   * 0: Rotate the video by 0 degree clockwise.
   */
  VIDEO_ORIENTATION_0 = 0,
  /**
   * 90: Rotate the video by 90 degrees clockwise.
   */
  VIDEO_ORIENTATION_90 = 90,
  /**
   * 180: Rotate the video by 180 degrees clockwise.
   */
  VIDEO_ORIENTATION_180 = 180,
  /**
   * 270: Rotate the video by 270 degrees clockwise.
   */
  VIDEO_ORIENTATION_270 = 270
};

/** The video frame rate. */
enum FRAME_RATE {
  /** 1: 1 fps. */
  FRAME_RATE_FPS_1 = 1,
  /** 7: 7 fps. */
  FRAME_RATE_FPS_7 = 7,
  /** 10: 10 fps. */
  FRAME_RATE_FPS_10 = 10,
  /** 15: 15 fps. */
  FRAME_RATE_FPS_15 = 15,
  /** 24: 24 fps. */
  FRAME_RATE_FPS_24 = 24,
  /** 30: 30 fps. */
  FRAME_RATE_FPS_30 = 30,
  /** 60: 60 fps. (Windows and macOS only) */
  FRAME_RATE_FPS_60 = 60,
};

enum FRAME_WIDTH {
  FRAME_WIDTH_640 = 640,
};

enum FRAME_HEIGHT {
  FRAME_HEIGHT_360 = 360,
};

/** Video frame type. */
enum VIDEO_FRAME_TYPE {
  VIDEO_FRAME_TYPE_BLANK_FRAME = 0,
  VIDEO_FRAME_TYPE_KEY_FRAME = 3,
  VIDEO_FRAME_TYPE_DELTA_FRAME = 4,
  VIDEO_FRAME_TYPE_B_FRAME = 5,
  VIDEO_FRAME_TYPE_UNKNOW
};

/** 
 * (For future use) The video output orientation modes. 
 */
enum ORIENTATION_MODE {
  /**
   * 0: (Default) Adaptive mode.
   * The video encoder adapts to the orientation mode of the video input device.
   * - If the width of the captured video from the SDK is greater than the height,
   * the encoder sends the video in landscape mode. The encoder also sends the
   * rotational information of the video, and the receiver uses the rotational
   * information to rotate the received video.
   * - When you use a custom video source, the output video from the encoder
   * inherits the orientation of the original video. If the original video is
   * in portrait mode, the output video from the encoder is also in portrait
   * mode. The encoder also sends the rotational information of the video
   * to the receiver.
   */
  ORIENTATION_MODE_ADAPTIVE = 0,
  /**
   * 1: Landscape mode.
   * The video encoder always sends the video in landscape mode. The video
   * encoder rotates the original video before sending it and the rotational
   * infomation is 0. This mode applies to scenarios involving CDN live
   * streaming.
   */
  ORIENTATION_MODE_FIXED_LANDSCAPE = 1,
  /**
   * 2: Portrait mode.
   * The video encoder always sends the video in portrait mode. The video
   * encoder rotates the original video before sending it and the rotational
   * infomation is 0. This mode applies to scenarios involving CDN live
   * streaming.
   */
  ORIENTATION_MODE_FIXED_PORTRAIT = 2,
};

/** 
 * The video degradation preferences when the bandwidth is a constraint. 
 */
enum DEGRADATION_PREFERENCE {
  /** 
   * 0: (Default) Degrade the frame rate in order to maintain the video quality. 
   */
  MAINTAIN_QUALITY = 0,
  /** 
   * 1: (For future use) Degrade the video quality in order to maintain the frame rate. 
   */
  MAINTAIN_FRAMERATE = 1,
  /** 
   * 2: (For future use) Maintain a balance between the frame rate and video quality. 
   */
  MAINTAIN_BALANCED = 2,
};

/**
 * The video dimensions.
 */
struct VideoDimensions {
  /** 
   * The width of the video in number of pixels.
   */
  int width;
  /** 
   * The height of the video in number of pixels.
   */
  int height;
  VideoDimensions() : width(640), height(480) {}
  VideoDimensions(int w, int h) : width(w), height(h) {}
};

/**
 * (Recommended) The standard bitrate set in \ref
 * agora::rtc::IRtcEngine::setVideoEncoderConfiguration "setVideoEncoderConfiguration()".
 *
 * In this mode, the bitrates differ between the Live Broadcast and Communication
 * profiles:
 *
 * - Communication profile: The video bitrate is the same as the base bitrate.
 * - Live Broadcast profile: The video bitrate is twice the base bitrate.
 */
const int STANDARD_BITRATE = 0;

/**
 * The compatible bitrate set in \ref agora::rtc::IRtcEngine::setVideoEncoderConfiguration
 * "setVideoEncoderConfiguration()".
 *
 * The bitrate remains the same regardless of the channel profile. If you choose
 * this mode in the live broadcast profile, the video frame rate may be lower
 * than the set value.
 */
const int COMPATIBLE_BITRATE = -1;

/** 
 * The default minimum bitrate. 
 * @note
 * Currently, this enumeration is not supported.
 */
const int DEFAULT_MIN_BITRATE = -1;

/** 
 * The video codec type. 
 */
enum VIDEO_CODEC_TYPE {
  /** 
   * 1: VP8. 
   */
  VIDEO_CODEC_VP8 = 1,
  /** 
   * 2: H.264. 
   */
  VIDEO_CODEC_H264 = 2,
  /** 
   * 3: H.265. 
   */
  VIDEO_CODEC_H265 = 3,
  /** 
   * 5: VP9. 
   */
  VIDEO_CODEC_VP9 = 5,
  // kVideoCodecI420,
  // kVideoCodecRED,
  // kVideoCodecULPFEC,
  // kVideoCodecFlexfec,
  // kVideoCodecGeneric,
  // kVideoCodecMultiplex,
  // kVideoCodecUnknown
};

/** 
 * The audio codec type. 
 */
enum AUDIO_CODEC_TYPE {
  /** 
   * 1: OPUS. 
   */
  AUDIO_CODEC_OPUS = 1,
  // kIsac = 2,
  /**
   * 3: PCMA.
   */
  AUDIO_CODEC_PCMA = 3,
  /**
   * 4: PCMU.
   */
  AUDIO_CODEC_PCMU = 4,
  // kG722 = 5,
  // kIlbc = 6,
  /** 7: AAC. */
  // AUDIO_CODEC_AAC = 7,
  /** 
   * 8: AAC LC. 
   */
  AUDIO_CODEC_AACLC = 8,
  /** 
   * 9: HE AAC. 
   */
  AUDIO_CODEC_HEAAC = 9,
  /**
   * 10: JC1.
   */
  AUDIO_CODEC_JC1 = 10,
  AUDIO_CODEC_HEAAC2 = 11,
};
/**
 * The struct of EncodedAudioFrameInfo.
 */
struct EncodedAudioFrameInfo {
  EncodedAudioFrameInfo()
      : speech(true),
        codec(AUDIO_CODEC_AACLC),
        sampleRateHz(0),
        samplesPerChannel(0),
        sendEvenIfEmpty(true),
        numberOfChannels(0) {}

  EncodedAudioFrameInfo(const EncodedAudioFrameInfo& rhs)
      : speech(rhs.speech),
        codec(rhs.codec),
        sampleRateHz(rhs.sampleRateHz),
        samplesPerChannel(rhs.samplesPerChannel),
        sendEvenIfEmpty(rhs.sendEvenIfEmpty),
        numberOfChannels(rhs.numberOfChannels) {}
  /**
   * Determines whether the audio frame source is a speech.
   * - true: (Default) The audio frame source is a speech.
   * - false: The audio frame source is not a speech.
   */
  bool speech;
  /**
   * The audio codec: AUDIO_CODEC_TYPE.
   */
  AUDIO_CODEC_TYPE codec;
  /**
   * The sample rate (Hz) of the audio frame.
   */
  int sampleRateHz;
  /**
   * The sample count of the audio frame.
   * If this value is not set, it is 1024 for AAC, 960 for OPUS default.
   */
  int samplesPerChannel;
  /**
   * Determines whether to sent the audio frame even when it is empty.
   * - true: (Default) Send the audio frame even when it is empty.
   * - false: Do not send the audio frame when it is empty.
   */
  bool sendEvenIfEmpty;
  /**
   * The number of channels of the audio frame.
   */
  int numberOfChannels;
};
/**
 * The struct of AudioPcmDataInfo.
 */
struct AudioPcmDataInfo {
  AudioPcmDataInfo() : sampleCount(0), samplesOut(0), elapsedTimeMs(0), ntpTimeMs(0) {}

  AudioPcmDataInfo(const AudioPcmDataInfo& rhs)
      : sampleCount(rhs.sampleCount),
        samplesOut(rhs.samplesOut),
        elapsedTimeMs(rhs.elapsedTimeMs),
        ntpTimeMs(rhs.ntpTimeMs) {}

  // The samples count you expect.
  /**
   * The sample count of the Pcm data that you expect.
   */
  size_t sampleCount;

  // Output
  /**
   * The number of output samples.
   */
  size_t samplesOut;
  int64_t elapsedTimeMs;
  int64_t ntpTimeMs;
};
/**
 * The struct of AudioPcmFrame.
 */
struct AudioPcmFrame {
  /**
   * The buffer size of the PCM audio frame.
   */
  enum : size_t {
    // Stereo, 32 kHz, 60 ms (2 * 32 * 60)
    kMaxDataSizeSamples = 3840,
    kMaxDataSizeBytes = kMaxDataSizeSamples * sizeof(int16_t),
  };

  AudioPcmFrame() {}

  uint32_t capture_timestamp = 0;
  size_t samples_per_channel_ = 0;
  int sample_rate_hz_ = 0;
  size_t num_channels_ = 0;
  size_t bytes_per_sample = 0;
  int16_t data_[kMaxDataSizeSamples] = {0};

  AudioPcmFrame(const AudioPcmFrame& frame) = delete;
  void operator=(const AudioPcmFrame& frame) = delete;
};
/**
 * The H264PacketizeMode. Applies to H.264 only.
 */
enum H264PacketizeMode {
  NonInterleaved = 0,  // Mode 1 - STAP-A, FU-A is allowed
  SingleNalUnit        // Mode 0 - only single NALU allowed
};
/**
 * The struct of EncodedVideoFrameInfo.
 */
struct EncodedVideoFrameInfo {
  EncodedVideoFrameInfo()
      : codecType(VIDEO_CODEC_H264),
        packetizationMode(H264PacketizeMode::NonInterleaved),
        width(0),
        height(0),
        framesPerSecond(0),
        frameType(VIDEO_FRAME_TYPE_BLANK_FRAME),
        rotation(VIDEO_ORIENTATION_0),
        trackId(0),
        renderTimeMs(0),
        internalSendTs(0),
        uid(0) {}

  EncodedVideoFrameInfo(const EncodedVideoFrameInfo& rhs)
      : codecType(rhs.codecType),
        packetizationMode(rhs.packetizationMode),
        width(rhs.width),
        height(rhs.height),
        framesPerSecond(rhs.framesPerSecond),
        frameType(rhs.frameType),
        rotation(rhs.rotation),
        trackId(rhs.trackId),
        renderTimeMs(rhs.renderTimeMs),
        internalSendTs(rhs.internalSendTs),
        uid(rhs.uid) {}
  /**
   * The video codec: VIDEO_CODEC_TYPE.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * The packetizationMode of the video frame: H264PacketizeMode.
   *
   * This member applies to H.264 only.
   */
  H264PacketizeMode packetizationMode;  // Just for H264.
  /**
   * The width (px) of the video.
   */
  int width;
  /**
   * The height (px) of the video.
   */
  int height;
  /**
   * The number of video frames per second.
   */
  int framesPerSecond;
  /**
   * The frame type of the encoded video frame: VIDEO_FRAME_TYPE.
   */
  VIDEO_FRAME_TYPE frameType;
  /**
   * The rotation information of the encoded video frame: VIDEO_ORIENTATION.
   */
  VIDEO_ORIENTATION rotation;
  int trackId;  // This can be reserved for multiple video tracks, we need to create different ssrc
                // and additional payload for later implementation.
  /**
   * The timestamp for rendering the video.
   */
  int64_t renderTimeMs;
  uint64_t internalSendTs;
  /**
   * ID of the user.
   */
  uid_t uid;
};

/** 
 * The video encoder configuration.
 */
struct VideoEncoderConfiguration {
  /**
   * The video encoder code type: #VIDEO_CODEC_TYPE.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * The video frame dimension used to specify the video quality and measured
   * by the total number of pixels along a frame's width and height: VideoDimensions.
   */
  VideoDimensions dimensions;
  /**
   * Frame rate of the video: int type, but can accept #FRAME_RATE for backward
   * compatibility.
   */
  int frameRate;
  /**
   * Video encoding target bitrate (Kbps).
   *
   * Choose one of the following options:
   *
   * - #STANDARD_BITRATE: (Recommended) Standard bitrate.
   *   - Communication profile: The encoding bitrate equals the base bitrate.
   *   - Live broadcast profile: The encoding bitrate is twice the base bitrate.
   * - #COMPATIBLE_BITRATE: Compatible bitrate: The bitrate stays the same
   * regardless of the profile.
   *
   * The Communication profile prioritizes smoothness, while the Live Broadcast
   * profile prioritizes video quality (requiring a higher bitrate). Agora
   * recommends setting the bitrate mode a #STANDARD_BITRATE or simply to
   * address this difference.
   *
   * The following table lists the recommended video encoder configurations,
   * where the base bitrate applies to the communication profile. Set your
   * bitrate based on this table. If the bitrate you set is beyond the proper
   * range, the SDK automatically sets it to within the range.

   | Resolution             | Frame Rate (fps) | Base Bitrate (Kbps, for Communication) | Live
   Bitrate (Kbps, for Live Broadcast)|
   |------------------------|------------------|----------------------------------------|----------------------------------------|
   | 160 &times; 120        | 15               | 65                                     | 130 | |
   120 &times; 120        | 15               | 50                                     | 100 | | 320
   &times; 180        | 15               | 140                                    | 280 | | 180
   &times; 180        | 15               | 100                                    | 200 | | 240
   &times; 180        | 15               | 120                                    | 240 | | 320
   &times; 240        | 15               | 200                                    | 400 | | 240
   &times; 240        | 15               | 140                                    | 280 | | 424
   &times; 240        | 15               | 220                                    | 440 | | 640
   &times; 360        | 15               | 400                                    | 800 | | 360
   &times; 360        | 15               | 260                                    | 520 | | 640
   &times; 360        | 30               | 600                                    | 1200 | | 360
   &times; 360        | 30               | 400                                    | 800 | | 480
   &times; 360        | 15               | 320                                    | 640 | | 480
   &times; 360        | 30               | 490                                    | 980 | | 640
   &times; 480        | 15               | 500                                    | 1000 | | 480
   &times; 480        | 15               | 400                                    | 800 | | 640
   &times; 480        | 30               | 750                                    | 1500 | | 480
   &times; 480        | 30               | 600                                    | 1200 | | 848
   &times; 480        | 15               | 610                                    | 1220 | | 848
   &times; 480        | 30               | 930                                    | 1860 | | 640
   &times; 480        | 10               | 400                                    | 800 | | 1280
   &times; 720       | 15               | 1130                                   | 2260 | | 1280
   &times; 720       | 30               | 1710                                   | 3420 | | 960
   &times; 720        | 15               | 910                                    | 1820 | | 960
   &times; 720        | 30               | 1380                                   | 2760 | | 1920
   &times; 1080      | 15               | 2080                                   | 4160 | | 1920
   &times; 1080      | 30               | 3150                                   | 6300 | | 1920
   &times; 1080      | 60               | 4780                                   | 6500 | | 2560
   &times; 1440      | 30               | 4850                                   | 6500 | | 2560
   &times; 1440      | 60               | 6500                                   | 6500 | | 3840
   &times; 2160      | 30               | 6500                                   | 6500 | | 3840
   &times; 2160      | 60               | 6500                                   | 6500 |
   */
  int bitrate;

  /** 
   * (For future use) The minimum encoding bitrate (Kbps).
   * 
   * The Agora SDK automatically adjusts the encoding bitrate to adapt to the
   * network conditions.
   *
   * Using a value greater than the default value forces the video encoder to
   * output high-quality images but may cause more packet loss and hence
   * sacrifice the smoothness of the video transmission. That said, unless you
   * have special requirements for image quality, Agora does not recommend
   * changing this value.
   *
   * @note
   * This parameter applies to the Live Broadcast profile only.
   */
  int minBitrate;
  /**
   * (For future use) The video orientation mode of the video: #ORIENTATION_MODE.
   */
  ORIENTATION_MODE orientationMode;
  /** 
   * 
   * The video degradation preference when the bandwidth is a constraint:
   * #DEGRADATION_PREFERENCE. Currently, this member supports `MAINTAIN_QUALITY`(0)
   * only.
   */
  DEGRADATION_PREFERENCE degradationPreference;

  VideoEncoderConfiguration(const VideoDimensions& d, int f, int b, ORIENTATION_MODE m)
      : codecType(VIDEO_CODEC_H264),
        dimensions(d),
        frameRate(f),
        bitrate(b),
        minBitrate(DEFAULT_MIN_BITRATE),
        orientationMode(m),
        degradationPreference(MAINTAIN_QUALITY) {}
  VideoEncoderConfiguration(int width, int height, int f, int b, ORIENTATION_MODE m)
      : codecType(VIDEO_CODEC_H264),
        dimensions(width, height),
        frameRate(f),
        bitrate(b),
        minBitrate(DEFAULT_MIN_BITRATE),
        orientationMode(m),
        degradationPreference(MAINTAIN_QUALITY) {}
  VideoEncoderConfiguration(const VideoEncoderConfiguration& config)
      : codecType(config.codecType),
        dimensions(config.dimensions),
        frameRate(config.frameRate),
        bitrate(config.bitrate),
        minBitrate(config.minBitrate),
        orientationMode(config.orientationMode),
        degradationPreference(config.degradationPreference) {}
  VideoEncoderConfiguration()
      : codecType(VIDEO_CODEC_H264),
        dimensions(FRAME_WIDTH_640, FRAME_HEIGHT_360),
        frameRate(FRAME_RATE_FPS_15),
        bitrate(STANDARD_BITRATE),
        minBitrate(DEFAULT_MIN_BITRATE),
        orientationMode(ORIENTATION_MODE_ADAPTIVE),
        degradationPreference(MAINTAIN_QUALITY) {}
};
/**
 * The stuct of SimulcastStreamConfig.
 */
struct SimulcastStreamConfig {
  /**
   * The video frame dimension, which is used to specify the video quality and measured by the total number
   * of pixels along a frame's width and height: VideoDimensions.
   */
  VideoDimensions dimensions;
  /**
   * The video bitrate in Kbps.
   */
  int bitrate;
  SimulcastStreamConfig() : dimensions(160, 120), bitrate(65) {}
};

/** The relative location of the region to the screen or window.
 */
struct Rectangle {
  /** The horizontal offset from the top-left corner.
   */
  int x;
  /** The vertical offset from the top-left corner.
   */
  int y;
  /** The width of the region.
   */
  int width;
  /** The height of the region.
   */
  int height;

  Rectangle() : x(0), y(0), width(0), height(0) {}
  Rectangle(int xx, int yy, int ww, int hh) : x(xx), y(yy), width(ww), height(hh) {}
};

/** Statistics of a channel. */
struct RtcStats {
  /**
   * The connection ID.
   */
  unsigned int connectionId;
  /**
   * The call duration (s), represented by an aggregate value.
   */
  unsigned int duration;
  /**
   * The total number of bytes transmitted, represented by an aggregate value.
   */
  unsigned int txBytes;
  /**
   * The total number of bytes received, represented by an aggregate value.
   */
  unsigned int rxBytes;
  /**
   * Total number of audio bytes sent (bytes), represented by an aggregate value.
   */
  unsigned int txAudioBytes;
  /**
   * Total number of video bytes sent (bytes), represented by an aggregate value.
   */
  unsigned int txVideoBytes;
  /**
   * Total number of audio bytes received (bytes) before network countermeasures, represented by an aggregate value.
   */
  unsigned int rxAudioBytes;
  /**
   * Total number of video bytes received (bytes), represented by an aggregate value.
   */
  unsigned int rxVideoBytes;
  /**
   * The transmission bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short txKBitRate;
  /**
   * The receive bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short rxKBitRate;
  /**
   * Audio receive bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short rxAudioKBitRate;
  /**
   * The audio transmission bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short txAudioKBitRate;
  /**
   * The video receive bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short rxVideoKBitRate;
  /**
   * The video transmission bitrate (Kbps), represented by an instantaneous value.
   */
  unsigned short txVideoKBitRate;
  /**
   * The VOS client-server latency (ms).
   */
  unsigned short lastmileDelay;
  /**
   * The number of users in the channel.
   */
  unsigned int userCount;
  /**
   * The application CPU usage (%).
   */
  double cpuAppUsage;
  /**
   * The system CPU usage (%).
   */
  double cpuTotalUsage;
  /**
   * The duration (ms) between connection establish and connect start , 0 if not valid
   */
  int connectTimeMs;
  /**
   * The duration(ms) between first audio packet received and connection start, 0 if not valid
   */
  int firstAudioPacketDuration;
  /**
   * The duration(ms) between first video packet received and connection start, 0 if not valid
   */
  int firstVideoPacketDuration;
  /**
   * The duration(ms) between first video key frame received and connection start, 0 if not valid
   */
  int firstVideoKeyFramePacketDuration;
  /**
   * Video packet number before first video key frame received, 0 if not valid
   */
  int packetsBeforeFirstKeyFramePacket;
  RtcStats() :
      connectionId(0),
      duration(0),
      txBytes(0),
      rxBytes(0),
      txAudioBytes(0),
      txVideoBytes(0),
      rxAudioBytes(0),
      rxVideoBytes(0),
      txKBitRate(0),
      rxKBitRate(0),
      rxAudioKBitRate(0),
      txAudioKBitRate(0),
      rxVideoKBitRate(0),
      txVideoKBitRate(0),
      lastmileDelay(0),
      userCount(0),
      cpuAppUsage(0.0),
      cpuTotalUsage(0.0),
      connectTimeMs(0),
      firstAudioPacketDuration(0),
      firstVideoPacketDuration(0),
      firstVideoKeyFramePacketDuration(0),
      packetsBeforeFirstKeyFramePacket(0) {}
};

/** 
 * User roles in a live broadcast.
 */
enum CLIENT_ROLE_TYPE {
  /** 
   * 1: The broadcaster. In a live broadcast channel, a broadcaster can both send and 
   * receive streams.
   */
  CLIENT_ROLE_BROADCASTER = 1,
  /** 
   * 2: The audience. In a live broadcast channel, an audience can only receive streams.
   */
  CLIENT_ROLE_AUDIENCE = 2,
};

///** Audio statistics of a remote user */
struct RemoteAudioStats
{
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
  int networkTransportDelay;
  /**
   * @return Network delay (ms) from the receiver to the jitter buffer.
   */
  int jitterBufferDelay;
  /**
   * The audio frame loss rate in the reported interval.
   */
  int audioLossRate;
  /**
   * The number of channels.
   */
  int numChannels;
  /**
   * The sample rate (Hz) of the received audio stream in the reported interval.
   */
  int receivedSampleRate;
  /**
   * The average bitrate (Kbps) of the received audio stream in the reported interval.
   */
  int receivedBitrate;
  /**
   * The total freeze time (ms) of the remote audio stream after the remote user joins the channel.
   * In a session, audio freeze occurs when the audio frame loss rate reaches 4%.
   * Agora uses 2 seconds as an audio piece unit to calculate the audio freeze time.
   * The total audio freeze time = The audio freeze number &times; 2 seconds
   */
  int totalFrozenTime;
  /**
   * The total audio freeze time as a percentage (%) of the total time when the audio is available.
   * */
  int frozenRate;
  RemoteAudioStats() :
    uid(0),
    quality(0),
    networkTransportDelay(0),
    jitterBufferDelay(0),
    audioLossRate(0),
    numChannels(0),
    receivedSampleRate(0),
    receivedBitrate(0),
    totalFrozenTime(0),
    frozenRate(0) {}
};

/**
 * The audio profile, which sets the sampling rate, bitrate, encode mode, and the number of channels.
 */
enum AUDIO_PROFILE_TYPE {
  /**
   * 0: The default audio profile.
   * - In the Communication profile, the default value is the same with `AUDIO_PROFILE_SPEECH_STANDARD`(1).
   * - In the Live-broadcast profile, it represents a sample rate of 48 kHz, music encoding, mono, and a bitrate
   * of up to 52 Kbps.
   */
  AUDIO_PROFILE_DEFAULT = 0, 
  /**
   * 1: A sample rate of 32 kHz, audio encoding, mono, and a bitrate up to 18 Kbps.
   */
  AUDIO_PROFILE_SPEECH_STANDARD = 1,
  /**
   * 2: A sample rate of 48 kHz, music encoding, mono, and a bitrate of up to 48 Kbps.
   */
  AUDIO_PROFILE_MUSIC_STANDARD = 2, 
  /**
   * 3: A sample rate of 48 kHz, music encoding, stereo, and a bitrate of up to 56
   * Kbps.
   */
  AUDIO_PROFILE_MUSIC_STANDARD_STEREO = 3, 
  /**
   * 4: A sample rate of 48 kHz, music encoding, mono, and a bitrate of up to 128 Kbps.
   */
  AUDIO_PROFILE_MUSIC_HIGH_QUALITY = 4,
  /**
   * 5: A sample rate of 48 kHz, music encoding, stereo, and a bitrate of up to 192 Kbps.
   */
  AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO = 5, 
  AUDIO_PROFILE_NUM = 6,
};

/** 
 * The audio application scenario. 
 */
enum AUDIO_SCENARIO_TYPE {
  /** 
   * 0: The default audio scenario. 
   */
  AUDIO_SCENARIO_DEFAULT = 0,
  /** 
   * 1: The entertainment scenario, which upports voice during gameplay. 
   */
  AUDIO_SCENARIO_CHATROOM_ENTERTAINMENT = 1,
  /** 
   * 2: The education scenario, which prioritizes fluency and stability. 
   */
  AUDIO_SCENARIO_EDUCATION = 2,
  /**
   * 3: The live gaming scenario, which needs to enable the gaming audio effects in
   * the speaker mode in a live broadcast scenario. Choose this scenario to
   * achieve high-fidelity music playback.
   */
  AUDIO_SCENARIO_GAME_STREAMING = 3,
  /**
   * 4: The showroom scenario, which optimizes the audio quality with professional
   * external equipment.
   */
  AUDIO_SCENARIO_SHOWROOM = 4,
  /** 
   * 5: The gaming scenario. 
   */
  AUDIO_SCENARIO_CHATROOM_GAMING = 5,
  /** 
   * 6: Reserved.
   */
  AUDIO_SCENARIO_NUM = 6,
};

/**
 * The struct of VideoFormat.
 */
struct VideoFormat {
  enum : size_t {
    kMaxWidthInPixels = 3840,
    kMaxHeightInPixels = 2160,
    kMaxFps = 60,
  };

  /**
   * The width (px) of the video.
   */
  int width;   // Number of pixels.
  /**
   * The height (px) of the video.
   */
  int height;  // Number of pixels.
  /**
   * The video frame rate (fps).
   */
  int fps;
  VideoFormat() : width(FRAME_WIDTH_640), height(FRAME_HEIGHT_360), fps(FRAME_RATE_FPS_15) {}
  VideoFormat(int w, int h, int f) : width(w), height(h), fps(f) {}
};

/** 
 * Enumerates the video content hints, currently only for screen capture. 
 */
enum VIDEO_CONTENT_HINT {
  /**
   * (Default) No hint, in which case, the Agora video engine makes its best-informed guess on how to 
   * treat the video content. This can for example be inferred by doing content analysis.
   */
  CONTENT_HINT_NONE,
  /**
   * The content hint is motion. The track should be treated as if it contains video where motion is
   * important. This is normally webcam video, movies or video games.
   * Quantization artifacts and downscaling are acceptable in order to preserve
   * motion as well as possible while still retaining target bitrates. During
   * low bitrates when compromises have to be made, more effort is spent on
   * preserving frame rate than edge quality and details.
   */
  CONTENT_HINT_MOTION,
  /**
   * The content hint is details. The track should be treated as if video details are extra important.
   * This is generally applicable to presentations or web pages with text
   * content, painting or line art. This setting would normally optimize for
   * detail in the resulting individual frames rather than smooth playback.
   * Artifacts from quantization or downscaling that make small text or line art
   * unintelligible should be avoided.
   */
  CONTENT_HINT_DETAILS
};

/** 
 * The video display mode. 
 */
enum RENDER_MODE_TYPE {
  /**
   * 1: Uniformly scale the video until it fills the visible boundaries
   * (cropped). One dimension of the video may have clipped contents.
   */
  RENDER_MODE_HIDDEN = 1,
  /**
   * 2: Uniformly scale the video until one of its dimension fits the boundary
   * (zoomed to fit). Areas that are not filled due to the disparity in the
   * aspect ratio will be filled with black.
   */
  RENDER_MODE_FIT = 2,
  /**
   * @deprecated
   * 3: This mode is deprecated.
   */
  RENDER_MODE_ADAPTIVE = 3,
};

/** 
 * The video mirror mode. 
 */
enum VIDEO_MIRROR_MODE_TYPE {
  /** 
   * (Default) 0: The mode mode determined by the SDK. 
   */
  VIDEO_MIRROR_MODE_AUTO = 0,
  /** 
   * 1: Enable the mirror mode.
   */
  VIDEO_MIRROR_MODE_ENABLED = 1,
  /** 
   * 2: Disable the mirror mode. 
   */
  VIDEO_MIRROR_MODE_DISABLED = 2, 
};

/**
 * The remote video stream type.
 */
enum REMOTE_VIDEO_STREAM_TYPE {
  /** 
   * 0: The high-stream video, that is, video featuring in high-resolution and high-bitrate. 
   */
  REMOTE_VIDEO_STREAM_HIGH = 0,
  /** 
   * 1: The low-stream video, that is, video featuring in low-resolution and low-bitrate. 
   */
  REMOTE_VIDEO_STREAM_LOW = 1,
};

/**
 * The local audio state type.
 */
enum LOCAL_AUDIO_STREAM_STATE {
  /**
   * 0: The initial state.
   */
  LOCAL_AUDIO_STREAM_STATE_STOPPED = 0,
  /**
   * 1: The capturer starts successfully.
   */
  LOCAL_AUDIO_STREAM_STATE_RECORDING = 1,
  /**
   * 2: The first video frame is successfully encoded.
   */
  LOCAL_AUDIO_STREAM_STATE_ENCODING = 2,
  /**
   * 3: The local video fails to start.
   */
  LOCAL_AUDIO_STREAM_STATE_FAILED = 3
};

/**
 * The local audio state error code.
 */
enum LOCAL_AUDIO_STREAM_ERROR {
  /**
   * 0: The local audio is normal.
   */
  LOCAL_AUDIO_STREAM_ERROR_OK = 0,
  /**
   * 1: No specified reason for the local audio failure.
   */
  LOCAL_AUDIO_STREAM_ERROR_FAILURE = 1,
  /**
   * 2: No permission to use the local audio device.
   */
  LOCAL_AUDIO_STREAM_ERROR_DEVICE_NO_PERMISSION = 2,
  /**
   * 3: The microphone is in use.
   */
  LOCAL_AUDIO_STREAM_ERROR_DEVICE_BUSY = 3,
  /**
   * 4: The local audio recording fails. Check whether the recording device
   * is working properly.
   */
  LOCAL_AUDIO_STREAM_ERROR_RECORD_FAILURE = 4,
  /**
   * 5: The local audio encoding fails.
   */
  LOCAL_AUDIO_STREAM_ERROR_ENCODE_FAILURE = 5
};

/**
 * The local video state type.
 */
enum LOCAL_VIDEO_STREAM_STATE {
  /**
   * 0: The initial state.
   */
  LOCAL_VIDEO_STREAM_STATE_STOPPED = 0,
  /**
   * 1: The capturer starts successfully.
   */
  LOCAL_VIDEO_STREAM_STATE_CAPTURING = 1,
  /**
   * 2: The first video frame is successfully encoded.
   */
  LOCAL_VIDEO_STREAM_STATE_ENCODING = 2,
  /**
   * 3: The local video fails to start.
   */
  LOCAL_VIDEO_STREAM_STATE_FAILED = 3
};

/**
 * The local video state error code.
 */
enum LOCAL_VIDEO_STREAM_ERROR {
  /**
   * 0: The local video is normal.
   */
  LOCAL_VIDEO_STREAM_ERROR_OK = 0,
  /**
   * 1: No specified reason for the local video failure.
   */
  LOCAL_VIDEO_STREAM_ERROR_FAILURE = 1,
  /**
   * 2: No permission to use the local video device.
   */
  LOCAL_VIDEO_STREAM_ERROR_DEVICE_NO_PERMISSION = 2,
  /**
   * 3: The local video capturer is in use.
   */
  LOCAL_VIDEO_STREAM_ERROR_DEVICE_BUSY = 3,
  /**
   * 4: The local video capture fails. Check whether the capturer is working properly.
   */
  LOCAL_VIDEO_STREAM_ERROR_CAPTURE_FAILURE = 4,
  /**
   * 5: The local video encoding fails.
   */
  LOCAL_VIDEO_STREAM_ERROR_ENCODE_FAILURE = 5
};

/** Remote audio states.
 */
enum REMOTE_AUDIO_STATE
{
  /**
   * 0: The remote audio is in the default state, probably due to
   * #REMOTE_AUDIO_REASON_LOCAL_MUTED (3),
   * #REMOTE_AUDIO_REASON_REMOTE_MUTED (5), or
   * #REMOTE_AUDIO_REASON_REMOTE_OFFLINE (7).
   */
  REMOTE_AUDIO_STATE_STOPPED = 0,  // Default state, audio is started or remote user disabled/muted audio stream
  /**
   * 1: The first remote audio packet is received.
   */
  REMOTE_AUDIO_STATE_STARTING = 1,  // The first audio frame packet has been received
  /**
   * 2: The remote audio stream is decoded and plays normally, probably
   * due to #REMOTE_AUDIO_REASON_NETWORK_RECOVERY (2),
   * #REMOTE_AUDIO_REASON_LOCAL_UNMUTED (4), or
   * #REMOTE_AUDIO_REASON_REMOTE_UNMUTED (6).
   */
  REMOTE_AUDIO_STATE_DECODING = 2,  // The first remote audio frame has been decoded or fronzen state ends
  /**
   * 3: The remote audio is frozen, probably due to
   * #REMOTE_AUDIO_REASON_NETWORK_CONGESTION (1).
   */
  REMOTE_AUDIO_STATE_FROZEN = 3,    // Remote audio is frozen, probably due to network issue
  /**
   * 4: The remote audio fails to start, probably due to
   * #REMOTE_AUDIO_REASON_INTERNAL (0).
   */
  REMOTE_AUDIO_STATE_FAILED = 4,    // Remote audio play failed
};

/** Remote audio state reasons.
 */
enum REMOTE_AUDIO_STATE_REASON
{
  /**
   * 0: Internal reasons.
   */
  REMOTE_AUDIO_REASON_INTERNAL = 0,
  /**
   * 1: Network congestion.
   */
  REMOTE_AUDIO_REASON_NETWORK_CONGESTION = 1,
  /**
   * 2: Network recovery.
   */
  REMOTE_AUDIO_REASON_NETWORK_RECOVERY = 2,
  /**
   * 3: The local user stops receiving the remote audio stream or
   * disables the audio module.
   */
  REMOTE_AUDIO_REASON_LOCAL_MUTED = 3,
  /**
   * 4: The local user resumes receiving the remote audio stream or
   * enables the audio module.
   */
  REMOTE_AUDIO_REASON_LOCAL_UNMUTED = 4,
  /**
   * 5: The remote user stops sending the audio stream or disables the
   * audio module.
   */
  REMOTE_AUDIO_REASON_REMOTE_MUTED = 5,
  /**
   * 6: The remote user resumes sending the audio stream or enables the
   * audio module.
   */
  REMOTE_AUDIO_REASON_REMOTE_UNMUTED = 6,
  /**
   * 7: The remote user leaves the channel.
   */
  REMOTE_AUDIO_REASON_REMOTE_OFFLINE = 7,
};

/**
 * The remote video state.
 */
enum REMOTE_VIDEO_STREAM_STATE {
  /**
   * 0: The initial state.
   */
  REMOTE_VIDEO_STATE_STOPPED = 0,
  /**
   * 1: The video packet has been received, but not decoded yet.
   */
  REMOTE_VIDEO_STATE_STARTING = 1,
  /**
   * 2: The remote video is playing.
   */
  REMOTE_VIDEO_STATE_PLAYING = 2,
  /**
   * 3: The Remote video is frozen, probably due to network issue.
   */
  REMOTE_VIDEO_STATE_FROZEN = 3,
  /**
   * 4: The remote video fails to start.
   */
  REMOTE_VIDEO_STATE_FAILED = 4,
};
/**
 * The remote video state change reasons.
 */
enum REMOTE_VIDEO_STATE_REASON {
  /**
   * 0: The first remote video packet is received.
   */
  REMOTE_VIDEO_STATE_REASON_FIRST_PACKET_RECEIVED = 0,
  /**
   * The remote user stops sending the video data.
   */
  REMOTE_VIDEO_STATE_REASON_USER_MUTE,
  /**
   * The remote user resumes sending the video data.
   */
  REMOTE_VIDEO_STATE_REASON_USER_UNMUTE,
  /**
   * The remote user enables the video.
   */
  REMOTE_VIDEO_STATE_REASON_USER_ENABLE,
  /**
   * The remote user disables the video.
   */
  REMOTE_VIDEO_STATE_REASON_USER_DISABLE,

  REMOTE_VIDEO_STATE_REASON_FIRST_FRAME_DECODED = 0x1000,
  REMOTE_VIDEO_STATE_REASON_MANUALLY,
  REMOTE_VIDEO_STATE_REASON_TRACK_DESTROY,
  REMOTE_VIDEO_STATE_REASON_NETWORK_DESTROY,

  REMOTE_VIDEO_STATE_REASON_UNKOWN = 0xFFFFFFFF,
};

/**
 * The struct of RemoteVideoTrackInfo.
 */
struct VideoTrackInfo {
  /**
   * ID of the user who owns the video track.
   */
  uid_t ownerUid;
  /**
   * ID of the video track.
   */
  track_id_t trackId;
  /**
   * The connection ID of the video track.
   */
  conn_id_t connectionId;
  /**
   * The video stream type: REMOTE_VIDEO_STREAM_TYPE.
   */
  REMOTE_VIDEO_STREAM_TYPE streamType;
  /**
   * The video codec type: VIDEO_CODEC_TYPE.
   */
  VIDEO_CODEC_TYPE codecType;
  /**
   * Whether the video track contains encoded video frame only.
   * - true: The video track contains encoded video frame only.
   * - false: The video track does not contain encoded video frame only.
   */
  bool encodedFrameOnly;
};

/** Properties of the audio volume information.
 */
struct AudioVolumeInfo {
  /** User ID of the speaker.
   */
  uid_t uid;
  user_id_t userId;
  /** The volume of the speaker that ranges from 0 (lowest volume) to 255 (highest volume).
   */
  unsigned int volume;  // [0,255]
};

/** Definition of IPacketObserver.
 */
class IPacketObserver {
 public:
  virtual ~IPacketObserver() {}
  /** Definition of Packet.
   */
  struct Packet {
    const unsigned char* buffer;
    unsigned int size;
  };
  /**
   * The audio packet is about to be sent to the other users.
   * @param packet  buffer *buffer points the data to be sent and the size of
   * the buffer data to be sent.
   * @return True to send out the packet. False to discard the packet.
   */
  virtual bool onSendAudioPacket(Packet& packet) = 0;
  /**
   * The video packet is about to be sent to the other users.
   * @param packet buffer *buffer points the data to be sent and the size of the
   * buffer data to be sent.
   * @return True to send out the packet. False to discard the packet.
   */
  virtual bool onSendVideoPacket(Packet& packet) = 0;
  /**
   * The audio packet is received by the other users.
   * @param packet buffer *buffer points to the data to be sent and the size of
   * the buffer data to be sent.
   * @return True to process the packet. False to discard the packet.
   */
  virtual bool onReceiveAudioPacket(Packet& packet) = 0;
  /**
   * The video packet is received by the other users.
   * @param packet buffer *buffer points the data to be sent and the size of the
   * buffer data to be sent.
   * @return True to process the packet. False to discard the packet.
   */
  virtual bool onReceiveVideoPacket(Packet& packet) = 0;
};
/**
 * The IVideoEncodedImageReceiver class.
 */
class IVideoEncodedImageReceiver {
 public:
  /**
   * Occurs each time the SDK receives an encoded video image.
   * @param imageBuffer The pointer to the video image buffer.
   * @param length The data length of the video image.
   * @param videoEncodedFrameInfo The information of the encoded video frame: EncodedVideoFrameInfo.
   *
   */
  virtual bool OnEncodedVideoImageReceived(const uint8_t* imageBuffer, size_t length,
                                           const EncodedVideoFrameInfo& videoEncodedFrameInfo) = 0;

  virtual ~IVideoEncodedImageReceiver() {}
};


class IVideoFrameObserver {
 public:
  virtual void onFrame(const agora::media::VideoFrame* frame) = 0;
  virtual ~IVideoFrameObserver() {}
};

class IAudioFrameObserver {
 public:
  virtual void onFrame(const rtc::AudioPcmFrame* frame) = 0;
  virtual ~IAudioFrameObserver() {}
};

/**
 * The audio sample rate types.
 */
enum AUDIO_SAMPLE_RATE_TYPE {
  /**
   * 32000: 32 KHz.
   */
  AUDIO_SAMPLE_RATE_32000 = 32000,
  /**
   * 44100: 44.1 KHz.
   */
  AUDIO_SAMPLE_RATE_44100 = 44100,
  /**
   * 48000: 48 KHz.
   */
  AUDIO_SAMPLE_RATE_48000 = 48000,
};
/**
 * The video codec profile types.
 */
enum VIDEO_CODEC_PROFILE_TYPE {
  /**
   * 66: Baseline video codec profile.
   */
  VIDEO_CODEC_PROFILE_BASELINE = 66,
  /**
   * 77: Main video codec profile.
   */
  VIDEO_CODEC_PROFILE_MAIN = 77,
  /**
   * 100: (Default) High video codec profile.
   */
  VIDEO_CODEC_PROFILE_HIGH = 100,
};

/**
 * Audio codec profile types.
 */
enum AUDIO_CODEC_PROFILE_TYPE {
  /**
   * 0: (Default) LC-AAC, which is the low-complexity audio codec type.
   */
  AUDIO_CODEC_PROFILE_LC_AAC = 0,
  /**
   * 1: HE-AAC, which is the high-efficiency audio codec type.
   */
  AUDIO_CODEC_PROFILE_HE_AAC = 1,
};

/**
 * States of the RTMP streaming.
 */
enum RTMP_STREAM_PUBLISH_STATE {
  /**
   * 0: The RTMP streaming has not started or has ended. This state is also triggered after you remove
   * an RTMP address from the CDN by calling removePublishStreamUrl.
   */
  RTMP_STREAM_PUBLISH_STATE_IDLE = 0,
  /**
   * 1: The SDK is connecting to Agora's streaming server and the RTMP server. This state is triggered
   * after you call the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method.
   */
  RTMP_STREAM_PUBLISH_STATE_CONNECTING = 1,
  /**
   * 2: The RTMP streaming publishes. The SDK successfully publishes the RTMP streaming and returns
   * this state.
   */
  RTMP_STREAM_PUBLISH_STATE_RUNNING = 2,
  /**
   * 3: The RTMP streaming is recovering. When exceptions occur to the CDN, or the streaming is
   interrupted, the SDK tries to resume RTMP streaming and returns this state.

   - If the SDK successfully resumes the streaming, #RTMP_STREAM_PUBLISH_STATE_RUNNING (2) returns.
   - If the streaming does not resume within 60 seconds or server errors occur,
   #RTMP_STREAM_PUBLISH_STATE_FAILURE (4) returns. You can also reconnect to the server by calling
   the \ref IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" and \ref
   IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" methods.
   */
  RTMP_STREAM_PUBLISH_STATE_RECOVERING = 3,
  /**
   * 4: The RTMP streaming fails. See the errCode parameter for the detailed error information. You
   * can also call the \ref IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" method to publish
   * the RTMP streaming again.
   */
  RTMP_STREAM_PUBLISH_STATE_FAILURE = 4,
};

/**
 * Error codes of the RTMP streaming.
 */
enum RTMP_STREAM_PUBLISH_ERROR {
  /**
   * -1: The RTMP streaming fails.
   */
  RTMP_STREAM_PUBLISH_ERROR_FAILED = -1,
  /**
   * 0: The RTMP streaming publishes successfully.
   */
  RTMP_STREAM_PUBLISH_ERROR_OK = 0,
  /**
   * 1: Invalid argument used. If, for example, you do not call the \ref IRtcEngine::setLiveTranscoding
   * "setLiveTranscoding" method to configure the LiveTranscoding parameters before calling the
   * addPublishStreamUrl method, the SDK returns this error. Check whether you set the parameters in the
   * `setLiveTranscoding` method properly.
   */
  RTMP_STREAM_PUBLISH_ERROR_INVALID_ARGUMENT = 1,
  /**
   * 2: The RTMP streaming is encrypted and cannot be published.
   */
  RTMP_STREAM_PUBLISH_ERROR_ENCRYPTED_STREAM_NOT_ALLOWED = 2,
  /**
   * 3: Timeout for the RTMP streaming. Call the \ref IRtcEngine::addPublishStreamUrl
   * "addPublishStreamUrl" method to publish the streaming again.
   */
  RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT = 3,
  /**
   * 4: An error occurs in Agora's streaming server. Call the addPublishStreamUrl method to publish
   * the streaming again.
   */
  RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR = 4,
  /**
   * 5: An error occurs in the RTMP server.
   */
  RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR = 5,
  /**
   * 6: The RTMP streaming publishes too frequently.
   */
  RTMP_STREAM_PUBLISH_ERROR_TOO_OFTEN = 6,
  /**
   * 7: The host publishes more than 10 URLs. Delete the unnecessary URLs before adding new ones.
   */
  RTMP_STREAM_PUBLISH_ERROR_REACH_LIMIT = 7,
  /**
   * 8: The host manipulates other hosts' URLs. Check your app logic.
   */
  RTMP_STREAM_PUBLISH_ERROR_NOT_AUTHORIZED = 8,
  /**
   * 9: Agora's server fails to find the RTMP streaming.
   */
  RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND = 9,
  /**
   * 10: The format of the RTMP streaming URL is not supported. Check whether the URL format is correct.
   */
  RTMP_STREAM_PUBLISH_ERROR_FORMAT_NOT_SUPPORTED = 10,
  /**
   * 11: CDN related errors. Remove the original URL address and add a new one by calling the
   * \ref agora::rtc::IRtcEngine::removePublishStreamUrl "removePublishStreamUrl" and
   * \ref agora::rtc::IRtcEngine::addPublishStreamUrl "addPublishStreamUrl" methods.
   */
  RTMP_STREAM_PUBLISH_ERROR_CDN_ERROR = 11,
  /**
   * 12: Resources have been occupied and cannot be reused.
   */
  RTMP_STREAM_PUBLISH_ERROR_ALREADY_IN_USE = 12,
};

/** The definition of RtcImage.
 */
typedef struct RtcImage {
  RtcImage() : url(NULL), x(0), y(0), width(0), height(0) {}
  /** URL address of the watermark on the live broadcast video.
   */
  const char* url;
  /** Position of the watermark on the upper left of the live broadcast video on
  the horizontal axis.
  */
  int x;
  /** Position of the watermark on the upper left of the live broadcast video on
  the vertical axis.
  */
  int y;
  /** Width of the watermark on the live broadcast video.
   */
  int width;
  /** Height of the watermark on the live broadcast video.
   */
  int height;
} RtcImage;

enum CONNECTION_STATE_TYPE
{
  /** 1: The SDK is disconnected from Agora's edge server.

   - This is the initial state before calling the \ref agora::rtc::IRtcEngine::joinChannel "joinChannel" method.
   - The SDK also enters this state when the application calls the \ref agora::rtc::IRtcEngine::leaveChannel "leaveChannel" method.
   */
  CONNECTION_STATE_DISCONNECTED = 1,
  /** 2: The SDK is connecting to Agora's edge server.

   - When the application calls the \ref agora::rtc::IRtcEngine::joinChannel "joinChannel" method, the SDK starts to establish a connection to the specified channel, triggers the \ref agora::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, and switches to the #CONNECTION_STATE_CONNECTING state.
   - When the SDK successfully joins the channel, it triggers the \ref agora::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the #CONNECTION_STATE_CONNECTED state.
   - After the SDK joins the channel and when it finishes initializing the media engine, the SDK triggers the \ref agora::rtc::IRtcEngineEventHandler::onJoinChannelSuccess "onJoinChannelSuccess" callback.
   */
  CONNECTION_STATE_CONNECTING = 2,
  /** 3: The SDK is connected to Agora's edge server and has joined a channel. You can now publish or subscribe to a media stream in the channel.

   If the connection to the channel is lost because, for example, the network is down or switched, the SDK automatically tries to reconnect and triggers:
   - The \ref agora::rtc::IRtcEngineEventHandler::onConnectionInterrupted "onConnectionInterrupted" callback (deprecated).
   - The \ref agora::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the #CONNECTION_STATE_RECONNECTING state.
   */
  CONNECTION_STATE_CONNECTED = 3,
  /** 4: The SDK keeps rejoining the channel after being disconnected from a joined channel because of network issues.

   - If the SDK cannot rejoin the channel within 10 seconds after being disconnected from Agora's edge server, the SDK triggers the \ref agora::rtc::IRtcEngineEventHandler::onConnectionLost "onConnectionLost" callback, stays in the #CONNECTION_STATE_RECONNECTING state, and keeps rejoining the channel.
   - If the SDK fails to rejoin the channel 20 minutes after being disconnected from Agora's edge server, the SDK triggers the \ref agora::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, switches to the #CONNECTION_STATE_FAILED state, and stops rejoining the channel.
   */
  CONNECTION_STATE_RECONNECTING = 4,
  /** 5: The SDK fails to connect to Agora's edge server or join the channel.

   You must call the \ref agora::rtc::IRtcEngine::leaveChannel "leaveChannel" method to leave this state, and call the \ref agora::rtc::IRtcEngine::joinChannel "joinChannel" method again to rejoin the channel.

   If the SDK is banned from joining the channel by Agora's edge server (through the RESTful API), the SDK triggers the \ref agora::rtc::IRtcEngineEventHandler::onConnectionBanned "onConnectionBanned" (deprecated) and \ref agora::rtc::IRtcEngineEventHandler::onConnectionStateChanged "onConnectionStateChanged" callbacks.
   */
  CONNECTION_STATE_FAILED = 5,
};

/** The definition of TranscodingUsers.
 */
struct TranscodingUser {
  /** User ID of the CDN live.
   */
  uid_t uid;
  user_id_t userId;
  /** Horizontal position of the top left corner of the video frame.
   */
  int x;
  /** Vertical position of the top left corner of the video frame.
   */
  int y;
  /** Width of the video frame.
   */
  int width;
  /** Height of the video frame.
   */
  int height;

  /** The layer of the video frame. Between 1 and 100:
  - 1: Default, lowest
  - 100: Highest
  */
  int zOrder;
  /** The transparency of the video frame.
   */
  double alpha;
  /**
   * The audio channel of the sound. The default value is 0:
   *  - 0: (default) Supports dual channels at most, depending on the upstream of
   *  the broadcaster.
   *  - 1: The audio stream of the broadcaster is in the FL audio channel. If the
   *  upstream of the broadcaster uses dual sound channel, only the left sound
   *  channel will be used for streaming.
   *  - 2: The audio stream of the broadcaster is in the FC audio channel. If the
   *  upstream of the broadcaster uses dual sound channel, only the left sound
   *  channel will be used for streaming.
   *  - 3: The audio stream of the broadcaster is in the FR audio channel. If the
   *  upstream of the broadcaster uses dual sound channel, only the left sound
   *  channel will be used for streaming.
   *  - 4: The audio stream of the broadcaster is in the BL audio channel. If the
   *  upstream of the broadcaster uses dual sound channel, only the left sound
   *  channel will be used for streaming.
   *  - 5: The audio stream of the broadcaster is in the BR audio channel.
   *  If the upstream of the broadcaster uses dual sound channel, only the left
   *  sound channel will be used for streaming.
  */
  int audioChannel;
  TranscodingUser()
      : uid(0),
        userId(NULL),
        x(0),
        y(0),
        width(0),
        height(0),
        zOrder(0),
        alpha(1.0),
        audioChannel(0) {}
};

/** 
 * The definition of LiveTranscoding.
 */
struct LiveTranscoding {
  /** 
   * The width of the video.
   */
  int width;
  /** 
   * The height of the video.
   */
  int height;
  /** 
   * The bitrate (Kbps) of the output data stream set for CDN live. The default value is 400.
   */
  int videoBitrate;
  /** 
   * The frame rate (fps) of the output data stream set for CDN live. The default value is 15.
   */
  int videoFramerate;
  /**
   * Determines whether to enable low latency.
   * - true: Low latency with unassured quality.
   * - false: (Default) High latency with assured quality.
   */
  bool lowLatency;
  /** 
   * The time interval (s) between two consecutive I frames. The default value is 2.
   */
  int videoGop;
  /** 
   * Self-defined video codec profiles: VIDEO_CODEC_PROFILE_TYPE.
   */
  VIDEO_CODEC_PROFILE_TYPE videoCodecProfile;
  /** 
   * The background color to set in RGB hex value. Value only, do not include a #.
   * For example, 0xFFB6C1 (light pink). The default value is 0x000000 (black).
   */
  unsigned int backgroundColor; 
  /** 
   * The number of users in the live broadcast.
   */
  unsigned int userCount;
  /** 
   * The TranscodingUser class.
   */
  TranscodingUser* transcodingUsers;
  /**
   * Extra user-defined information to be sent to the CDN client. The extra
   * infomation will be transmitted by SEI packets.
   */
  const char* transcodingExtraInfo;
  /** 
   * The pointer to the metadata to be sent to CDN client defined by rtmp or FLV metadata. 
   */
  const char* metadata;
  /**
   * The HTTP/HTTPS URL address of the watermark image added to the CDN
   * publishing stream. The audience of the CDN publishing stream can see the
   * watermark.
  */
  RtcImage* watermark;
  /**
   * The HTTP/HTTPS URL address of the background image added to the CDN
   * publishing stream. The audience of the CDN publishing stream can see the
   * background image.
  */
  RtcImage* backgroundImage;
  /** 
   * The self-defined audio-sampling rates: #AUDIO_SAMPLE_RATE_TYPE
   */
  AUDIO_SAMPLE_RATE_TYPE audioSampleRate;
  /**
   * The bitrate (Kbps) of the audio-output stream set for CDN live. The highest
   * value is 128.
  */
  int audioBitrate;
  /**
   * Agora's self-defined audio-channel types. Agora recommends choosing 1 or
   * 2:
   * - 1: Mono (default)
   * - 2: Dual-sound channels
   * - 3: Three-sound channels
   * - 4: Four-sound channels
   * - 5: Five-sound channels
  */
  int audioChannels;

  /**
   * The audio codec profile type: #AUDIO_CODEC_PROFILE_TYPE.
   */
  AUDIO_CODEC_PROFILE_TYPE audioCodecProfile;

  LiveTranscoding()
      : width(360),
        height(640),
        videoBitrate(400),
        videoFramerate(15),
        lowLatency(false),
        videoGop(30),
        videoCodecProfile(VIDEO_CODEC_PROFILE_HIGH),
        backgroundColor(0x000000),
        userCount(0),
        transcodingUsers(NULL),
        transcodingExtraInfo(NULL),
        metadata(NULL),
        watermark(NULL),
        backgroundImage(NULL),
        audioSampleRate(AUDIO_SAMPLE_RATE_48000),
        audioBitrate(48),
        audioChannels(1),
        audioCodecProfile(AUDIO_CODEC_PROFILE_LC_AAC) {}
};

}  // namespace rtc

namespace base {

class IEngineBase {
 public:
  virtual int queryInterface(rtc::INTERFACE_ID_TYPE iid, void** inter) = 0;
  virtual ~IEngineBase() {}
};

class AParameter : public agora::util::AutoPtr<IAgoraParameter> {
 public:
  AParameter(IEngineBase& engine) { initialize(&engine); }
  AParameter(IEngineBase* engine) { initialize(engine); }
  AParameter(IAgoraParameter* p) : agora::util::AutoPtr<IAgoraParameter>(p) {}

 private:
  bool initialize(IEngineBase* engine) {
    IAgoraParameter* p = NULL;
    if (engine && !engine->queryInterface(rtc::AGORA_IID_PARAMETER_ENGINE, (void**)&p)) reset(p);
    return p != NULL;
  }
};

}  // namespace base

}  // namespace agora

/**
 * Gets the version number of the SDK.
 * @param [in, out] build
 *        The build number of Agora SDK
 * @return The string of the version of the SDK.
 */
AGORA_API const char* AGORA_CALL getAgoraSdkVersion(int* build);

/**
 * Gets error description from error code
 * @param [in] err
 *        The error code
 * @return The description of the error code.
 */
AGORA_API const char* AGORA_CALL getAgoraSdkErrorDescription(int err);

AGORA_API int AGORA_CALL setAgoraSdkExternalSymbolLoader(void* (*func)(const char* symname));
