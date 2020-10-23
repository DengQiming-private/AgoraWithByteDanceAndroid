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

        bool isInitOpenGL = false;
        bool ExtensionVideoFilter::adaptVideoFrame(const agora::media::base::VideoFrame &capturedFrame,
                             agora::media::base::VideoFrame &adaptedFrame) {
//            PRINTF_INFO("filter VideoFrame");
            if (!isInitOpenGL) {
                isInitOpenGL = byteDanceProcessor_->initOpenGL();
            }
            byteDanceProcessor_->processFrame(capturedFrame);
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

        void ExtensionVideoFilter::setEnabled(bool enable) {
            int a = 10;
        }

        bool ExtensionVideoFilter::isEnabled() {
            return true;
        }
    }
}
