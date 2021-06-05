//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H
#define AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H

#include "AgoraRtcKit/NGIAgoraMediaNode.h"
#include <AgoraRtcKit/AgoraRefCountedObject.h>
#include "AgoraRtcKit/AgoraRefPtr.h"
#include "VideoProcessor.h"

namespace agora {
    namespace extension {
        class ExtensionVideoFilter : public agora::rtc::IExtensionVideoFilter {
        public:
            ExtensionVideoFilter(agora_refptr<ByteDanceProcessor> byteDanceProcessor);

            ~ExtensionVideoFilter();

            void getProcessMode(ProcessMode& mode, bool& isolated) override;
            int start(agora::agora_refptr<Control> control) override;
            int stop() override;
            void getVideoFormatWanted(rtc::VideoFrameInfo::Type& type, rtc::MemPixelBuffer::Format& format) override;
            IExtensionVideoFilter::ProcessResult pendVideoFrame(
                    agora::agora_refptr<rtc::IVideoFrame> frame) override;
            IExtensionVideoFilter::ProcessResult adaptVideoFrame(
                    agora::agora_refptr<rtc::IVideoFrame> src, agora::agora_refptr<rtc::IVideoFrame>& dst) override;

            void setEnabled(bool enable) override;
            bool isEnabled() override;
            int setProperty(const char* key, const void* buf, size_t buf_size) override;
            int getProperty(const char* key, void* buf, size_t buf_size) override;

        private:
            agora::agora_refptr<Control> control_;
            agora_refptr<ByteDanceProcessor> byteDanceProcessor_;
            bool isInitOpenGL = false;
            ProcessMode mode_;
        protected:
            ExtensionVideoFilter() = default;

        };
    }
}


#endif //AGORAWITHBYTEDANCE_EXTENSIONVIDEOFILTER_H
