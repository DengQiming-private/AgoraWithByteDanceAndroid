//
//  Agora SDK
//
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "../AgoraBase.h"
#include "../AgoraRefPtr.h"

namespace agora {
namespace rtc {
class IVideoSinkBase;
class ICameraCaptureObserver;
class IVideoRenderer;
/**
 * The ICameraCapturer class, which provides access to a camera capturer.
 */
class ICameraCapturer : public RefCountInterface {

 public:
  /**
   * The state of the camera capturer.
   */
  enum CAPTURE_STATE {
    /**
     * The camera capturer has stopped or has not started yet.
     */
    CAPTURE_STATE_STOPPED,
    /**
     * The camera capturer is in the process of starting. This state does not necessarily mean that the capturer
     * has successfully started.
     */
    CAPTURE_STATE_STARTING,
    /**
     * The camera capturer has started successfully and is now capturing video data.
     */
    CAPTURE_STATE_RUNNING,
    /**
     * The camera capturer is in the process of stopping. This state does not necessarily mean that the capturer
     * has successfully stopped.
     */
    CAPTURE_STATE_STOPPING,
    /**
     * The camera capturer fails to start.
     */
    CAPTURE_STATE_FAILED,
  };
  /**
   * The camera source.
   */
  enum CAMERA_SOURCE {
    /**
     * The camera source is the front camera.
     */
    CAMERA_FRONT,
    /**
     * The camera source is the rear camera.
     */
    CAMERA_BACK,
  };

  // Interface for receiving information about available camera devices.
  /**
   * The IDeviceInfo class, which manages the information of available cameras.
   */
  class IDeviceInfo {
   public:
    virtual ~IDeviceInfo() = default;

    /**
     * Releases the device.
     */
    virtual void release() = 0;

    /**
     * Gets the number of all available cameras.
     * @return
     * - The number of all available cameras.
     */
    virtual uint32_t NumberOfDevices() = 0;

    /**
     * Gets the name of a specified camera.
     * @param deviceNumber The index number of the device.
     * @param deviceNameUTF8 The name of the device.
     * @param deviceNameLength The length of the device name.
     * @param deviceUniqueIdUTF8 The unique ID of the device, if any.
     * @param deviceUniqueIdLength The length of the device ID, if any.
     * @param productUniqueIdUTF8 The unique ID of the product, if any.
     * @param productUniqueIdLength The length of the product ID, if any.
     * @return
     * The name of the device in the UTF8 format, if the method call succeeds.
     */
    virtual int32_t GetDeviceName(uint32_t deviceNumber, char* deviceNameUTF8,
                                  uint32_t deviceNameLength, char* deviceUniqueIdUTF8,
                                  uint32_t deviceUniqueIdLength, char* productUniqueIdUTF8 = 0,
                                  uint32_t productUniqueIdLength = 0) = 0;

    /**
     * Sets the capability number for a specified device.
     * @param deviceUniqueIdUTF8 The pointer to the ID of the device in the UTF8 format.
     * @return
     * The capability number of the device.
     */
    virtual int32_t NumberOfCapabilities(const char* deviceUniqueIdUTF8) = 0;
    /**
     * Gets the capability of a specified device.
     * @param deviceUniqueIdUTF8 The pointer to the ID of the device in the UTF8 format.
     * @param deviceCapabilityNumber The capability number of the device.
     * @param capability The reference to the video capability: VideoFormat.
     * @return
     * The capability number of the device.
     */

    virtual int32_t GetCapability(const char* deviceUniqueIdUTF8,
                                  const uint32_t deviceCapabilityNumber,
                                  VideoFormat& capability) = 0;
  };

 public:
#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IPHONE)
  /**
   * Sets the camera source.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @param source The camera source that you want to capture: #CAMERA_SOURCE.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setCameraSource(CAMERA_SOURCE source) = 0;
  /**
   * Gets the camera source.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @return The camera source: #CAMERA_SOURCE.
   */
  virtual CAMERA_SOURCE getCameraSource() = 0;
  /**
   * Switches the camera source, for example, from the front camera, to the rear camera.
   *
   * @note
   * This method applies to Android and iOS only.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int switchCamera() = 0;
#elif defined(_WIN32) || (defined(__APPLE__) && !TARGET_OS_IPHONE && TARGET_OS_MAC) || \
    (defined(__linux__) && !defined(__ANDROID__))

  /**
   * Creates a DeviceInfo object.
   *
   * @note
   * This method applies to Windows, macOS, and Linux only.
   * @return
   * - The pointer to IDeviceInfo, if the method call succeeds.
   * - The empty pointer NULL, if the method call fails.
   */
  virtual IDeviceInfo* createDeviceInfo() = 0;
  /**
   * Initializes the device with the device ID.
   *
   * @note
   * This method applies to Windows, macOS, and Linux only.
   *
   * @param deviceId The pointer to the device ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initWithDeviceId(const char* deviceId) = 0;
  /**
   * Initializes the device with the device name.
   *
   * @note
   * This method applies to Windows, macOS, and Linux only.
   *
   * @param deviceName The pointer to the device name.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initWithDeviceName(const char* deviceName) = 0;
#endif

  /**
   * Sets the format of the video captured by the camera.
   *
   * If you do not set the video capturing format, the SDK automatically chooses a proper format according to the video encoder configuration of the video track.
   *
   * @param capture_format The reference to the video format: VideoFormat.
   */
  virtual void setCaptureFormat(const VideoFormat& capture_format) = 0;

  /**
   * Gets the format of the video captured by the camera.
   * @return
   * VideoFormat.
   */
  virtual VideoFormat getCaptureFormat() = 0;
  /**
   * Gets the state of the camera capture.
   * @return
   * The capture state: #CAPTURE_STATE.
   */
  virtual CAPTURE_STATE getCaptureState() = 0;
  /**
   * Registers a camera capture observer object.
   * @param observer The pointer to ICameraCaptureObserver.
   */
  virtual void registerCameraCaptureObserver(ICameraCaptureObserver* observer) = 0;
  /**
   * Releases the camera capture observer object.
   * @param observer The pointer to ICameraCaptureObserver.
   */
  virtual void unregisterCameraCaptureObserver(ICameraCaptureObserver* observer) = 0;

 protected:
  ~ICameraCapturer() {}
};

/**
 * The ICameraCaptureObserver class, which contains the callback that reports the state change of the camera capture.
 */
class ICameraCaptureObserver {
 public:
  virtual ~ICameraCaptureObserver() {}

  /**
   * Occurs when the state of the camera capture changes.
   * @param state The current state of the camera capture: \ref ICameraCapture::CAPTURE_STATE "CAPTURE_STATE".
   * @param error The error code: #ERROR_CODE_TYPE.
   */
  virtual void onCaptureStateChanged(ICameraCapturer::CAPTURE_STATE state,
                                     ERROR_CODE_TYPE error) = 0;
};

}  // namespace rtc
}  // namespace agora
