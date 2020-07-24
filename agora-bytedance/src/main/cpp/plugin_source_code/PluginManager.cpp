//
// Created by 张涛 on 2020/4/30.
//

#include "PluginManager.h"

namespace agora {
    namespace extension {

        int PluginManager::loadPlugin() {
            agoraService_ = createAgoraService();
            CHECK_AGORA_SERVICE;
            byteDanceProcessor_ = new agora::RefCountedObject<ByteDanceProcessor>();
            auto extensionProvider = new agora::RefCountedObject<agora::extension::ExtensionProvider>(byteDanceProcessor_);
            int ret = agoraService_->getExtensionControl()->registerExtensionProvider("bytedance", extensionProvider);
            return ret;
        }

        int PluginManager::unloadPlugin() {
            CHECK_AGORA_SERVICE;
            int ret = agoraService_->getExtensionControl()->unregisterExtensionProvider("bytedance");
            return ret;
        }

        int PluginManager::setParameters(std::string parameter) {
            byteDanceProcessor_->setParameters(parameter);
            return 0;
        }

        int PluginManager::setEnable(bool enable) {
            CHECK_AGORA_SERVICE;
            return ERROR_CODE::ERROR_OK;
        }

        bool PluginManager::isEnable() {
            CHECK_AGORA_SERVICE_BOOL;
            return true;
        }

        size_t PluginManager::setProperty(const char *key, const void *buf, size_t buf_size) {
            CHECK_AGORA_SERVICE;
            return 0;
        }

        size_t PluginManager::getProperty(const char *key, void *buf, size_t buf_size) {
            CHECK_AGORA_SERVICE;
            return 0;
        }
    }
}