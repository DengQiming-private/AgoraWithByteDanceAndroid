//
// Created by DYF on 2020/7/13.
//

#include "ByteDanceProcessor.h"
#include "JniHelper.h"
#include "../logutils.h"
#include "rapidjson/document.h"

namespace agora {
    namespace extension {
        using namespace rapidjson;
        bool ByteDanceProcessor::initOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            if (!eglCore_) {
                eglCore_ = new EglCore();
            }
            return true;
        }

        bool ByteDanceProcessor::releaseOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            delete eglCore_;
            eglCore_ = nullptr;
            return true;
        }

        int ByteDanceProcessor::initEffectEngine() {
            const std::lock_guard<std::mutex> lock(mutex_);
            if (!aiEffectEnabled_) {
                return 0;
            }
            if (aiEffectLoaded_) {
                return 0;
            }
            if (!specialEffectHandler_) {
                bef_effect_result_t result;
                result = bef_effect_ai_create(&specialEffectHandler_);
                PRINTF_INFO("&&&bef_effect_ai_create result : %d", result);

                void * context = AndroidContextHelper::getContext();
                result = bef_effect_ai_check_license(
                        JniHelper::getJniHelper()->attachCurrentTnread(),
                        reinterpret_cast<jobject>(context), specialEffectHandler_,
                        licensePath_.c_str());
                PRINTF_INFO("&&&bef_effect_ai_check_license : %d , license_path : %s", result,
                            licensePath_.c_str());
                result = bef_effect_ai_init(specialEffectHandler_, 0, 0, modelDir_.c_str(), "");
                PRINTF_INFO("&&&bef_effect_ai_init result : %d ", result);
                aiEffectLoaded_ = true;
            }
            return 0;
        }


        int ByteDanceProcessor::releaseEffectEngine() {
            const std::lock_guard<std::mutex> lock(mutex_);
            if (specialEffectHandler_) {
                bef_effect_ai_destroy(specialEffectHandler_);
                specialEffectHandler_ = nullptr;
            }
            return 0;
        }

        int ByteDanceProcessor::setParameters(std::string parameter) {
            const std::lock_guard<std::mutex> lock(mutex_);
            Document d;
            d.Parse(parameter.c_str());
            if (d.HasParseError()) {
                return -100;
            }

            if (d.HasMember("plugin.bytedance.licensePath")) {
                Value& licensePath = d["plugin.bytedance.licensePath"];
                if (!licensePath.IsString()) {
                    return -101;
                }
                licensePath_ = std::string(licensePath.GetString());
            }

            if (d.HasMember("plugin.bytedance.modelDir")) {
                Value& modelDir = d["plugin.bytedance.modelDir"];
                if (!modelDir.IsString()) {
                    return -101;
                }
                modelDir_ = std::string(modelDir.GetString());
            }

            if (d.HasMember("plugin.bytedance.aiEffectEnabled")) {
                Value& enabled = d["plugin.bytedance.aiEffectEnabled"];
                if (!enabled.IsBool()) {
                    return -101;
                }
                aiEffectEnabled_ = enabled.GetBool();
            }

            return 0;
        }

        std::thread::id ByteDanceProcessor::getThreadId() {
            std::thread::id id = std::this_thread::get_id();
            return id;
        }

    }
}
