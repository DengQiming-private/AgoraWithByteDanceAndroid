//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionVideoProvider.h"
#include "../logutils.h"
#include "VideoProcessor.h"

namespace agora {
    namespace extension {
        ExtensionVideoProvider* ExtensionVideoProvider::instance_;
        ExtensionVideoProvider::ExtensionVideoProvider() {
            PRINTF_INFO("ExtensionVideoProvider create");
            byteDanceProcessor_ = new agora::RefCountedObject<ByteDanceProcessor>();
        }

        ExtensionVideoProvider::~ExtensionVideoProvider() {
            PRINTF_INFO("ExtensionVideoProvider destroy");
            instance_ = nullptr;
        }

        void ExtensionVideoProvider::enumerateExtensions(ExtensionMetaInfo* extension_list,
                                                           int& extension_count) {
            extension_count = 1;
            ExtensionMetaInfo i;
            i.type = EXTENSION_TYPE::VIDEO_PRE_PROCESSING_FILTER;
            i.extension_name = "Beauty";
            extension_list[0] = i;
        }

        agora_refptr<agora::rtc::IExtensionVideoFilter> ExtensionVideoProvider::createVideoFilter(const char* name) {
            PRINTF_INFO("ExtensionVideoProvider::createVideoFilter %s", name);
            auto videoFilter = new agora::RefCountedObject<agora::extension::ExtensionVideoFilter>(byteDanceProcessor_);
            return videoFilter;
        }

        agora_refptr<agora::rtc::IAudioFilter> ExtensionVideoProvider::createAudioFilter(const char* name) {
            return nullptr;
        }

        agora_refptr<agora::rtc::IVideoSinkBase> ExtensionVideoProvider::createVideoSink(const char* name) {
            return nullptr;
        }

        void ExtensionVideoProvider::setExtensionControl(rtc::IExtensionControl* control){
        }
    }
}
