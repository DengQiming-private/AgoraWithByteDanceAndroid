//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionProvider.h"
#include "../logutils.h"
#include "ByteDanceProcessor.h"

namespace agora {
    namespace extension {

        ExtensionProvider::ExtensionProvider() {
            byteDanceProcessor_ = new agora::RefCountedObject<ByteDanceProcessor>();
        }

        ExtensionProvider::~ExtensionProvider() {

        }

        int ExtensionProvider::setParameters(std::string parameter) {
            byteDanceProcessor_->setParameters(parameter);
            return 0;
        }

        agora_refptr<agora::rtc::IVideoFilter> ExtensionProvider::createVideoFilter(const char* id) {
//            PRINTF_INFO("ExtensionProvider::createVideoFilter");
            byteDanceProcessor_->setExtensionVendor(id);
            auto videoFilter = new agora::RefCountedObject<agora::extension::ExtensionVideoFilter>(byteDanceProcessor_);
            return videoFilter;
        }

        agora_refptr<agora::rtc::IAudioFilter> ExtensionProvider::createAudioFilter(const char* id) {
            return nullptr;
        }

        agora_refptr<agora::rtc::IVideoSinkBase> ExtensionProvider::createVideoSink(const char* id) {
            return nullptr;
        }

        ExtensionProvider::PROVIDER_TYPE ExtensionProvider::getProviderType() {
            return ExtensionProvider::PROVIDER_TYPE::LOCAL_VIDEO_FILTER;
        }

        void ExtensionProvider::setExtensionControl(rtc::IExtensionControl* control){
            byteDanceProcessor_->setExtensionControl(control);
        }
    }
}
