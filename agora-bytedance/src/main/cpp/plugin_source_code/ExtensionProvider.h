//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H
#define AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H

#include "AgoraRtcKit/NGIAgoraExtensionControl.h"
#include "ExtensionVideoFilter.h"

namespace agora {
    namespace extension {

        class ByteDanceProcessor;

        class ExtensionProvider : public agora::rtc::IExtensionProvider {
        private:
            agora_refptr<ByteDanceProcessor> byteDanceProcessor_;

        public:
            ExtensionProvider();

            ~ExtensionProvider();

            PROVIDER_TYPE getProviderType() override;

            virtual void setExtensionControl(rtc::IExtensionControl* control) override;

            virtual agora_refptr<rtc::IAudioFilter> createAudioFilter(const char* id) override;

            virtual agora_refptr<rtc::IVideoFilter> createVideoFilter(const char* id) override;

            virtual agora_refptr<rtc::IVideoSinkBase> createVideoSink(const char* id) override;

            int setParameters(std::string parameter);
        };
    }
}
#endif //AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H
