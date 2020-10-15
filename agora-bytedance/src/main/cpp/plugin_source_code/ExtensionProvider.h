//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H
#define AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H

#include "AgoraRtcKit/NGIAgoraExtensionVideoFilter.h"
#include "ExtensionVideoFilter.h"

namespace agora {
    namespace extension {

        class ByteDanceProcessor;

        class ExtensionProvider : public agora::rtc::IExtensionVideoFilterProvider {
        private:
            agora_refptr<ByteDanceProcessor> byteDanceProcessor_;

        public:
            ExtensionProvider();

            ~ExtensionProvider();

            virtual unsigned int extensionFrameworkVersion(char* buffer, unsigned int buffer_size) const override;

            virtual unsigned int vendor(char* buffer, unsigned int buffer_size) const override;

            virtual unsigned int name(char* buffer, unsigned int buffer_size) const override;

            virtual unsigned int version(char* buffer, unsigned int buffer_size) const override;

            virtual agora::rtc::Video_Filter_Position position() const override;

            virtual agora::rtc::IExtensionVideoFilter* create() override;

            virtual void destroy(agora::rtc::IExtensionVideoFilter* filter) override;

            int setParameters(std::string parameter);
        };
    }
}
#endif //AGORAWITHBYTEDANCE_EXTENSIONPROVIDER_H
