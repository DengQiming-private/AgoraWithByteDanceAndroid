//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionVideoFilter.h"
#include "../logutils.h"

namespace agora {
    namespace extension {

        ExtensionVideoFilter::ExtensionVideoFilter(agora_refptr<ByteDanceProcessor> byteDanceProcessor) {
            byteDanceProcessor_ = byteDanceProcessor;
        }

        ExtensionVideoFilter::~ExtensionVideoFilter() {
            byteDanceProcessor_->releaseOpenGL();
        }

        void ExtensionVideoFilter::setEnabled(bool enable) {

        }

        bool ExtensionVideoFilter::isEnabled() {
            return false;
        }

        bool ExtensionVideoFilter::adaptVideoFrame(const agora::media::VideoFrame &capturedFrame,
                                                   agora::media::VideoFrame &adaptedFrame) {
//            PRINTF_INFO("adaptVideoFrame");
            byteDanceProcessor_->updateEffect(capturedFrame);
            adaptedFrame = capturedFrame;
            return true;
        }

        size_t ExtensionVideoFilter::setProperty(const char *key, const void *buf,
                                                 size_t buf_size) {
            return 0;
        }

        size_t ExtensionVideoFilter::getProperty(const char *key, void *buf, size_t buf_size) {
            return 0;
        }

        bool ExtensionVideoFilter::onDataStreamWillStart() {
            PRINTF_INFO("ExtensionVideoFilter::onDataStreamWillStart");
            byteDanceProcessor_->initOpenGL();
            return true;
        }

        void ExtensionVideoFilter::onDataStreamWillStop() {
            PRINTF_INFO("ExtensionVideoFilter::onDataStreamWillStop");
            byteDanceProcessor_->releaseOpenGL();
        }

    }
}