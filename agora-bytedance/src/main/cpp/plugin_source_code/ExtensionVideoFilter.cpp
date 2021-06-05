//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionVideoFilter.h"
#include "../logutils.h"
#include <sstream>

namespace agora {
    namespace extension {

        ExtensionVideoFilter::ExtensionVideoFilter(agora_refptr<ByteDanceProcessor> byteDanceProcessor) {
            byteDanceProcessor_ = byteDanceProcessor;
        }

        ExtensionVideoFilter::~ExtensionVideoFilter() {
            byteDanceProcessor_->releaseOpenGL();
        }

        void ExtensionVideoFilter::getProcessMode(ProcessMode& mode, bool& isolated) {
            mode = ProcessMode::kSync;
            isolated = false;
            mode_ = mode;
        }

        int ExtensionVideoFilter::start(agora::agora_refptr<Control> control) {
            PRINTF_INFO("ExtensionVideoFilter::start");
            if (control) {
                control_ = control;
            }
            if (byteDanceProcessor_) {
                isInitOpenGL = byteDanceProcessor_->initOpenGL();
                byteDanceProcessor_->setExtensionControl(control);
            }
            return 0;
        }

        int ExtensionVideoFilter::stop() {
            PRINTF_INFO("ExtensionVideoFilter::stop");
            if (byteDanceProcessor_) {
                byteDanceProcessor_->releaseOpenGL();
                isInitOpenGL = false;
            }
            return 0;
        }

        void ExtensionVideoFilter::getVideoFormatWanted(rtc::VideoFrameInfo::Type& type,
                                                        rtc::MemPixelBuffer::Format& format) {
            type = rtc::VideoFrameInfo::Type::kMemPixels;
            format = rtc::MemPixelBuffer::Format::kI420;
        }

        rtc::IExtensionVideoFilter::ProcessResult ExtensionVideoFilter::pendVideoFrame(agora::agora_refptr<rtc::IVideoFrame> frame) {
            if (!frame || !isInitOpenGL) {
                return kBypass;
            }

            bool isAsyncMode = (mode_ == ProcessMode::kAsync);
            if (isAsyncMode && byteDanceProcessor_ && control_) {
                rtc::VideoFrameInfo info;
                frame->getVideoFrameInfo(info);
                byteDanceProcessor_->processFrame(info);
                auto dst = control_->getMemoryPool()->createVideoFrame(info);
                control_->deliverVideoFrame(dst);
                return kSuccess;
            }
            return kBypass;
        }

        rtc::IExtensionVideoFilter::ProcessResult ExtensionVideoFilter::adaptVideoFrame(agora::agora_refptr<rtc::IVideoFrame> src,
                                                                               agora::agora_refptr<rtc::IVideoFrame>& dst) {
            if (!isInitOpenGL) {
                return kBypass;
            }
            bool isSyncMode = (mode_ == ProcessMode::kSync);
            if (isSyncMode && byteDanceProcessor_) {
                rtc::VideoFrameInfo srcInfo;
                src->getVideoFrameInfo(srcInfo);
                byteDanceProcessor_->processFrame(srcInfo);
                dst = src;
                return kSuccess;
            }
            return kBypass;
        }

        int ExtensionVideoFilter::setProperty(const char *key, const void *buf,
                                                 size_t buf_size) {
            PRINTF_INFO("setProperty  %s  %s", key, buf);
            std::string stringParameter((char*)buf);
            byteDanceProcessor_->setParameters(stringParameter);
            return 0;
        }

        int ExtensionVideoFilter::getProperty(const char *key, void *buf, size_t buf_size) {
            return -1;
        }

        void ExtensionVideoFilter::setEnabled(bool enable) {
        }

        bool ExtensionVideoFilter::isEnabled() {
            return true;
        }
    }
}
