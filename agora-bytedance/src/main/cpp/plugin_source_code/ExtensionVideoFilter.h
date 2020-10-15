//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H
#define AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H

#include "AgoraRtcKit/NGIAgoraExtensionVideoFilter.h"
#include <AgoraRtcKit/AgoraRefCountedObject.h>
#include "AgoraRtcKit/AgoraRefPtr.h"
#include "ByteDanceProcessor.h"

namespace agora {
    namespace extension {
        class ExtensionVideoFilter : public agora::rtc::IExtensionVideoFilter {
        public:
            ExtensionVideoFilter(agora_refptr<ByteDanceProcessor> byteDanceProcessor);

            ~ExtensionVideoFilter();

            bool filter(const agora::media::base::VideoFrame &capturedFrame,
                             agora::media::base::VideoFrame &adaptedFrame) override;

            bool setExtensionFacility(agora::rtc::IExtensionFacility* facility) override;

            bool setProperty(const char* key, const char* json_value) override;

            unsigned int property(const char* key,
                                  char* json_value_buffer, unsigned int json_value_buffer_size) const override;

        private:
            agora_refptr<ByteDanceProcessor> byteDanceProcessor_;
        protected:
            ExtensionVideoFilter() = default;

        };


    }
}


#endif //AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H
