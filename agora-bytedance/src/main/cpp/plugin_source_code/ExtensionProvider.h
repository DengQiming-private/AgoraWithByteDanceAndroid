//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H
#define AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H

#include "../agora/api2/NGIAgoraMediaNodeFactory.h"
#include "../agora/api2/NGIAgoraExtensionControl.h"
#include "../agora/AgoraRefPtr.h"
#include "../agora/api2/AgoraRefCountedObject.h"
#include "ExtensionVideoFilter.h"

namespace agora {
    namespace extension {

        class ByteDanceProcessor;

        class ExtensionProvider : public agora::rtc::IExtensionProvider {
        private:
            agora_refptr<agora::rtc::IVideoFilter> videoFilterPtr_;
            agora_refptr<ByteDanceProcessor> byteDanceProcessor_;

        public:
            ExtensionProvider(agora_refptr<ByteDanceProcessor> byteDanceProcessor);

            ~ExtensionProvider();

            virtual agora_refptr<agora::rtc::IVideoFilter> createVideoFilter(const char *name) override;

            virtual agora_refptr<agora::rtc::IAudioFilter> createAudioFilter(const char *name) override;

            virtual agora_refptr<agora::rtc::IVideoSinkBase> createVideoSink(const char *name) override;

        protected:
            ExtensionProvider() = default;
        };
    }
}
#endif //AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H
