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
        bool ExtensionVideoFilter::filter(const agora::media::base::VideoFrame &capturedFrame,
                                                   agora::media::base::VideoFrame &adaptedFrame) {
//            PRINTF_INFO("filter VideoFrame");
            if (!isInitOpenGL) {
                isInitOpenGL = byteDanceProcessor_->initOpenGL();
            }
            byteDanceProcessor_->processFrame(capturedFrame);
            adaptedFrame = capturedFrame;
            return true;
        }

        bool ExtensionVideoFilter::setProperty(const char* key, const char* json_value) {
            return true;
        }

        unsigned int ExtensionVideoFilter::property(const char* key,
                              char* json_value_buffer, unsigned int json_value_buffer_size) const {
            return 0;
        }

        bool ExtensionVideoFilter::setExtensionFacility(agora::rtc::IExtensionFacility* facility) {
            byteDanceProcessor_->setExtensionFacility(facility);
            return true;
        }
    }
}
