//
// Created by 张涛 on 2020/4/30.
//

#ifndef AGORAWITHBYTEDANCE_PLUGINMANAGER_H
#define AGORAWITHBYTEDANCE_PLUGINMANAGER_H

#include <jni.h>
#include <string>
#include "../agora/api2/IAgoraService.h"
#include "ExtensionProvider.h"
#include "../logutils.h"
#include "JniHelper.h"
#include "../agora/AgoraRefPtr.h"
#include "error_code.h"
#include "ByteDanceProcessor.h"

#define Extension_Provider reinterpret_cast<ExtensionProvider*>(extensionProvider.get())

#define CHECK_AGORA_SERVICE if (!agoraService_) { \
                                PRINTF_ERROR("Agora extension call api: %s err: %d", __FUNCTION__, ERROR_CODE::ERROR_NOT_INIT_RTC_ENGINE); \
                                return ERROR_CODE::ERROR_NOT_INIT_RTC_ENGINE;\
                            }\

#define CHECK_AGORA_SERVICE_BOOL if (!agoraService_) { \
                                PRINTF_ERROR("Agora extension call api: %s err: %d", __FUNCTION__, ERROR_CODE::ERROR_NOT_INIT_RTC_ENGINE); \
                                return false;\
                            }\

namespace agora {
    namespace extension {
        class PluginManager {
        public:
            int loadPlugin();

            int unloadPlugin();

            int setParameters(std::string parameter);

            int setEnable(bool enable);

            bool isEnable();

            size_t setProperty(const char *key, const void *buf, size_t buf_size);

            size_t getProperty(const char *key, void *buf, size_t buf_size);

        private:
            agora_refptr<ByteDanceProcessor> byteDanceProcessor_ = nullptr;
            agora::base::IAgoraService* agoraService_ = nullptr;
        };
    }
}
#endif //AGORAWITHBYTEDANCE_PLUGINMANAGER_H
