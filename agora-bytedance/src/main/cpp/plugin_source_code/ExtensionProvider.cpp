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

        agora::rtc::IExtensionVideoFilter* ExtensionProvider::create() {
//            PRINTF_INFO("ExtensionProvider::createVideoFilter");
            auto videoFilter = new agora::RefCountedObject<agora::extension::ExtensionVideoFilter>(byteDanceProcessor_);
            return videoFilter;
        }

        void ExtensionProvider::destroy(agora::rtc::IExtensionVideoFilter* filter) {

        }

        agora::rtc::Video_Filter_Position ExtensionProvider::position() const {
            return agora::rtc::Video_Filter_Position::Video_Filter_Pre_Encode;
        }

        unsigned int ExtensionProvider::extensionFrameworkVersion(char* buffer, unsigned int buffer_size) const {
            std::string version = "1.0.0";
            sprintf(buffer, "%s", version.c_str());
            return version.length();
        }

        unsigned int ExtensionProvider::vendor(char* buffer, unsigned int buffer_size) const {
            std::string vender = "ByteDance";
            sprintf(buffer, "%s", vender.c_str());
            return vender.length();
        }

        unsigned int ExtensionProvider::name(char* buffer, unsigned int buffer_size) const {
            std::string name = "ByteDance Extension Provider";
            sprintf(buffer, "%s", name.c_str());
            return name.length();
        }

        unsigned int ExtensionProvider::version(char* buffer, unsigned int buffer_size) const {
            std::string version = "1.0.0";
            sprintf(buffer, "%s", version.c_str());
            return version.length();
        }
    }
}
