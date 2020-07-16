//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H
#define AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H

#include "../agora/api2/NGIAgoraMediaNodeFactory.h"
#include "../agora/AgoraRefPtr.h"
#include "ByteDanceProcessor.h"

namespace agora {
    namespace extension {
        class ExtensionVideoFilter : public agora::rtc::IVideoFilter {
        public:
            ExtensionVideoFilter(agora_refptr<ByteDanceProcessor> byteDanceProcessor);

            ~ExtensionVideoFilter();

            bool adaptVideoFrame(const agora::media::VideoFrame &capturedFrame,
                                 agora::media::VideoFrame &adaptedFrame) override;

            void setEnabled(bool enable) override;

            bool isEnabled() override;

            size_t setProperty(const char *key, const void *buf, size_t buf_size) override;

            size_t getProperty(const char *key, void *buf, size_t buf_size) override;

            bool onDataStreamWillStart() override;

            void onDataStreamWillStop() override;

        private:
            agora_refptr<ByteDanceProcessor> byteDanceProcessor_;
        protected:
            ExtensionVideoFilter() = default;

        };


    }
}


#endif //AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H
