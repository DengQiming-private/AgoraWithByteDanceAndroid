//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionProvider.h"
#include "../logutils.h"
#include "ByteDanceProcessor.h"

namespace agora {
    namespace extension {

        ExtensionProvider::ExtensionProvider(agora_refptr<ByteDanceProcessor> byteDanceProcessor) {
            byteDanceProcessor_ = byteDanceProcessor;
        }

        ExtensionProvider::~ExtensionProvider() {

        }

        agora_refptr<agora::rtc::IVideoFilter>
        ExtensionProvider::createVideoFilter(const char *name) {
//            PRINTF_INFO("ExtensionProvider::createVideoFilter");
            auto videoFilter = new agora::RefCountedObject<agora::extension::ExtensionVideoFilter>(byteDanceProcessor_);
            return videoFilter;
        }

        agora_refptr<agora::rtc::IAudioFilter>
        ExtensionProvider::createAudioFilter(const char *name) {
            return nullptr;
        }

        agora_refptr<agora::rtc::IVideoSinkBase>
        ExtensionProvider::createVideoSink(const char *name) {
            return nullptr;
        }

    }
}
