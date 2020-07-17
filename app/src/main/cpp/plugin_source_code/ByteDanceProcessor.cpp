//
// Created by DYF on 2020/7/13.
//

#include "ByteDanceProcessor.h"

#include <chrono>

#include "JniHelper.h"
#include "../logutils.h"
#include "rapidjson/document.h"
#include "../bytedance/bef_effect_ai_yuv_process.h"

#define CHECK_BEF_AI_RET_SUCCESS(ret, ...) \
if(ret != 0){\
    PRINTF_ERROR(__VA_ARGS__);\
}

namespace agora {
    namespace extension {
        using namespace rapidjson;
        bool ByteDanceProcessor::initOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            if (!eglCore_) {
                eglCore_ = new EglCore();
                offscreenSurface_ = eglCore_->createOffscreenSurface(320, 640);
                eglCore_->makeCurrent(offscreenSurface_);
            }
            return true;
        }

        bool ByteDanceProcessor::releaseOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            if (eglCore_) {
                if (offscreenSurface_) {
                    eglCore_->releaseSurface(offscreenSurface_);
                }
                delete eglCore_;
                eglCore_ = nullptr;
            }
            return true;
        }

        unsigned char* createYUVBuffer(const agora::media::VideoFrame &capturedFrame) {
            int ysize = capturedFrame.yStride * capturedFrame.height;
            int usize = capturedFrame.uStride * capturedFrame.height / 2;
            int vsize = capturedFrame.vStride * capturedFrame.height / 2;
            unsigned char * yuvBuffer = (unsigned char*)malloc(ysize + usize + vsize);
            memcpy(yuvBuffer, capturedFrame.yBuffer, ysize);
            memcpy(yuvBuffer + ysize, capturedFrame.uBuffer, usize);
            memcpy(yuvBuffer + ysize + usize, capturedFrame.vBuffer, vsize);
            return yuvBuffer;
        }

        unsigned char* createRGBABuffer(const agora::media::VideoFrame &capturedFrame ) {
            return (unsigned char*)malloc(capturedFrame.yStride * capturedFrame.height * 4);
        }
        int ByteDanceProcessor::updateEffect(const agora::media::VideoFrame &capturedFrame) {
            const std::lock_guard<std::mutex> lock(mutex_);
            if (!aiEffectEnabled_) {
                return 0;
            }

            if (!byteEffectHandler_) {
                bef_effect_result_t ret;
                ret = bef_effect_ai_create(&byteEffectHandler_);
                CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::updateEffect create effect handle failed ! %d", ret);

                void * context = AndroidContextHelper::getContext();
                ret = bef_effect_ai_check_license(
                        JniHelper::getJniHelper()->attachCurrentTnread(),
                        reinterpret_cast<jobject>(context), byteEffectHandler_,
                        licensePath_.c_str());
                CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::updateEffect check license failed, %d path: %s", ret, licensePath_.c_str());

                ret = bef_effect_ai_init(byteEffectHandler_, 0, 0, modelDir_.c_str(), "");
                CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::updateEffect init effect handler failed, %d model path: %s", ret, modelDir_.c_str());

                ret = bef_effect_ai_composer_set_mode(byteEffectHandler_, 1, 0);
                CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::updateEffect set composer mode failed %d", ret);
            }

            if (aiEffectNeedUpdate_) {
                bef_effect_result_t ret;
                ret = bef_effect_ai_composer_set_nodes(byteEffectHandler_, (const char **)aiNodes_, aiNodeCount_);
                CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::updateEffect composer set nodes failed ! %d", ret);

                for (SizeType i = 0; i < aiNodeCount_; i++) {
                    ret = bef_effect_ai_composer_update_node(byteEffectHandler_, aiNodes_[i], aiNodeKeys_[i].c_str(), aiNodeIntensities_[i]);
                    CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::updateEffect update composer failed %d %s %s %f", ret, aiNodeKeys_[i].c_str(), aiNodes_[i], aiNodeIntensities_[i]);
                }
                aiEffectNeedUpdate_ = false;
            }
            if (aiEffectEnabled_ ) {
                uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                bef_effect_ai_set_width_height(byteEffectHandler_, capturedFrame.width, capturedFrame.height);
                unsigned char* yuvBuffer = createYUVBuffer(capturedFrame);
                unsigned char* rgbaBuffer = createRGBABuffer(capturedFrame);
                cvt_yuv2rgba(yuvBuffer, rgbaBuffer, BEF_AI_PIX_FMT_YUV420P, capturedFrame.width, capturedFrame.height, capturedFrame.width, capturedFrame.height, BEF_AI_CLOCKWISE_ROTATE_0,
                             false);
                bef_effect_result_t ret;
                ret = bef_effect_ai_algorithm_buffer(byteEffectHandler_, rgbaBuffer, BEF_AI_PIX_FMT_RGBA8888, capturedFrame.width, capturedFrame.height, capturedFrame.yStride * 4, timestamp);
                CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::updateEffect ai algorithm buffer failed %d", ret);
                ret = bef_effect_ai_process_buffer(byteEffectHandler_, rgbaBuffer, BEF_AI_PIX_FMT_RGBA8888, capturedFrame.yStride, capturedFrame.height, capturedFrame.yStride * 4, rgbaBuffer, BEF_AI_PIX_FMT_RGBA8888, timestamp);
                CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::updateEffect ai process buffer failed %d", ret);

                cvt_rgba2yuv(rgbaBuffer, yuvBuffer, BEF_AI_PIX_FMT_YUV420P, capturedFrame.yStride, capturedFrame.height);

                int ysize = capturedFrame.yStride * capturedFrame.height;
                int usize = capturedFrame.uStride * capturedFrame.height / 2;
                int vsize = capturedFrame.vStride * capturedFrame.height / 2;
                memcpy(capturedFrame.yBuffer, yuvBuffer, ysize);
                memcpy(capturedFrame.uBuffer, yuvBuffer + ysize, usize);
                memcpy(capturedFrame.vBuffer, yuvBuffer + ysize + usize, vsize);
                free(rgbaBuffer);
                free(yuvBuffer);

            }

            return 0;
        }


        int ByteDanceProcessor::releaseEffectEngine() {
            const std::lock_guard<std::mutex> lock(mutex_);
            if (byteEffectHandler_) {
                bef_effect_ai_destroy(byteEffectHandler_);
                byteEffectHandler_ = nullptr;
            }
            aiEffectEnabled_ = false;
            licensePath_.clear();
            modelDir_.clear();

            if (aiNodes_) {
                for (int i = 0; i < aiNodeCount_; i++) {
                    free(aiNodes_[i]);
                }
                free(aiNodes_);
            }
            aiNodeIntensities_.clear();
            aiNodeKeys_.clear();
            aiNodeCount_ = 0;
            aiEffectNeedUpdate_ = false;
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

            if (d.HasMember("plugin.bytedance.ai.composer.nodes")) {
                Value& nodes = d["plugin.bytedance.ai.composer.nodes"];
                if (!nodes.IsArray()) {
                    return -101;
                }

                if (aiNodes_) {
                    for (int i = 0; i < aiNodeCount_; i++) {
                        free(aiNodes_[i]);
                    }
                    free(aiNodes_);
                }
                aiNodeIntensities_.clear();
                aiNodeKeys_.clear();
                aiNodeCount_ = nodes.Size();
                aiNodes_ = (char **)malloc(nodes.Size() * sizeof(char *));
                for (SizeType i = 0; i < nodes.Size(); i++) {
                    if (!nodes[i].IsObject()) {
                        return -101;
                    }
                    Value& node = nodes[i];

                    if (node.HasMember("path") && node.HasMember("key") && node.HasMember("intensity")) {
                        Value& vPath = node["path"];
                        Value& vKey = node["key"];
                        Value& vIntensity = node["intensity"];
                        const char* path = vPath.GetString();
                        size_t strLength = strlen(path);
                        aiNodes_[i] = (char *)malloc((strLength + 1) * sizeof(char *));
                        strncpy(aiNodes_[i], path, strLength);
                        aiNodes_[i][strLength] = '\0';
                        aiNodeKeys_.push_back(vKey.GetString());
                        aiNodeIntensities_.push_back(vIntensity.GetFloat());
                    }
                    else {
                        PRINTF_ERROR("plugin.bytedance.ai.composer.nodes param error: idx %d", i);
                    }
                }
                aiEffectNeedUpdate_ = true;
            }

            return 0;
        }

        std::thread::id ByteDanceProcessor::getThreadId() {
            std::thread::id id = std::this_thread::get_id();
            return id;
        }

    }
}
