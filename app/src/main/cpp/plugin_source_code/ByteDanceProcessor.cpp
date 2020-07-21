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
                offscreenSurface_ = eglCore_->createOffscreenSurface(0, 0);
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

        void ByteDanceProcessor::prepareCachedVideoFrame(const agora::media::VideoFrame &capturedFrame) {
            int ysize = capturedFrame.yStride * capturedFrame.height;
            int usize = capturedFrame.uStride * capturedFrame.height / 2;
            int vsize = capturedFrame.vStride * capturedFrame.height / 2;
            if (yuvBuffer_ == nullptr ||
            rgbaBuffer_ == nullptr ||
            prevFrame_.width != capturedFrame.width ||
            prevFrame_.height != capturedFrame.height ||
            prevFrame_.yStride != capturedFrame.yStride ||
            prevFrame_.uStride != capturedFrame.uStride ||
            prevFrame_.vStride != capturedFrame.vStride) {
                if (yuvBuffer_) {
                    free(yuvBuffer_);
                    yuvBuffer_ = nullptr;
                }
                if (rgbaBuffer_) {
                    free(rgbaBuffer_);
                    rgbaBuffer_ = nullptr;
                }

                yuvBuffer_ = (unsigned char*)malloc(ysize + usize + vsize);
                rgbaBuffer_ = (unsigned char*)malloc(capturedFrame.yStride * capturedFrame.height * 4);
            }
            // update YUV buffer
            memcpy(yuvBuffer_, capturedFrame.yBuffer, ysize);
            memcpy(yuvBuffer_ + ysize, capturedFrame.uBuffer, usize);
            memcpy(yuvBuffer_ + ysize + usize, capturedFrame.vBuffer, vsize);

            // update RGBA buffer
            cvt_yuv2rgba(yuvBuffer_, rgbaBuffer_, BEF_AI_PIX_FMT_YUV420P, capturedFrame.width,
                         capturedFrame.height, capturedFrame.width, capturedFrame.height,
                         BEF_AI_CLOCKWISE_ROTATE_0,
                         false);
            prevFrame_ = capturedFrame;

        }

        void ByteDanceProcessor::processEffect(const agora::media::VideoFrame &capturedFrame) {
            if (!byteEffectHandler_) {
                bef_effect_result_t ret;
                ret = bef_effect_ai_create(&byteEffectHandler_);
                CHECK_BEF_AI_RET_SUCCESS(ret,
                                         "ByteDanceProcessor::processEffect create effect handle failed ! %d",
                                         ret);

                void *context = AndroidContextHelper::getContext();
                ret = bef_effect_ai_check_license(
                        JniHelper::getJniHelper()->attachCurrentTnread(),
                        reinterpret_cast<jobject>(context), byteEffectHandler_,
                        licensePath_.c_str());
                CHECK_BEF_AI_RET_SUCCESS(ret,
                                         "ByteDanceProcessor::processEffect check license failed, %d path: %s",
                                         ret, licensePath_.c_str());

                ret = bef_effect_ai_init(byteEffectHandler_, 0, 0, modelDir_.c_str(), "");
                CHECK_BEF_AI_RET_SUCCESS(ret,
                                         "ByteDanceProcessor::processEffect init effect handler failed, %d model path: %s",
                                         ret, modelDir_.c_str());

                ret = bef_effect_ai_composer_set_mode(byteEffectHandler_, 1, 0);
                CHECK_BEF_AI_RET_SUCCESS(ret,
                                         "ByteDanceProcessor::processEffect set composer mode failed %d",
                                         ret);
            }

            if (aiEffectNeedUpdate_) {
                bef_effect_result_t ret;
                if (aiNodeCount_ <= 0) {
                    const char *nodes[] = {};
                    ret = bef_effect_ai_composer_set_nodes(byteEffectHandler_,
                                                           nodes, 0);
                    CHECK_BEF_AI_RET_SUCCESS(ret,
                                             "ByteDanceProcessor::processEffect composer set nodes to empty failed ! %d",
                                             ret);
                } else {
                    ret = bef_effect_ai_composer_set_nodes(byteEffectHandler_,
                                                           (const char **) aiNodes_, aiNodeCount_);
                    CHECK_BEF_AI_RET_SUCCESS(ret,
                                             "ByteDanceProcessor::processEffect composer set nodes failed ! %d",
                                             ret);

                    for (SizeType i = 0; i < aiNodeCount_; i++) {
                        ret = bef_effect_ai_composer_update_node(byteEffectHandler_, aiNodes_[i],
                                                                 aiNodeKeys_[i].c_str(),
                                                                 aiNodeIntensities_[i]);
                        CHECK_BEF_AI_RET_SUCCESS(ret,
                                                 "ByteDanceProcessor::processEffect update composer failed %d %s %s %f",
                                                 ret, aiNodeKeys_[i].c_str(), aiNodes_[i],
                                                 aiNodeIntensities_[i]);
                    }
                }
                aiEffectNeedUpdate_ = false;
            }

            uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            bef_effect_ai_set_width_height(byteEffectHandler_, capturedFrame.width,
                                           capturedFrame.height);

            bef_effect_result_t ret;
            ret = bef_effect_ai_algorithm_buffer(byteEffectHandler_, rgbaBuffer_,
                                                 BEF_AI_PIX_FMT_RGBA8888, capturedFrame.width,
                                                 capturedFrame.height, capturedFrame.yStride * 4,
                                                 timestamp);
            CHECK_BEF_AI_RET_SUCCESS(ret,
                                     "ByteDanceProcessor::updateEffect ai algorithm buffer failed %d",
                                     ret);
            ret = bef_effect_ai_process_buffer(byteEffectHandler_, rgbaBuffer_,
                                               BEF_AI_PIX_FMT_RGBA8888, capturedFrame.yStride,
                                               capturedFrame.height, capturedFrame.yStride * 4,
                                               rgbaBuffer_, BEF_AI_PIX_FMT_RGBA8888, timestamp);
            CHECK_BEF_AI_RET_SUCCESS(ret,
                                     "ByteDanceProcessor::updateEffect ai process buffer failed %d",
                                     ret);

            cvt_rgba2yuv(rgbaBuffer_, yuvBuffer_, BEF_AI_PIX_FMT_YUV420P, capturedFrame.yStride,
                         capturedFrame.height);

            int ysize = capturedFrame.yStride * capturedFrame.height;
            int usize = capturedFrame.uStride * capturedFrame.height / 2;
            int vsize = capturedFrame.vStride * capturedFrame.height / 2;
            memcpy(capturedFrame.yBuffer, yuvBuffer_, ysize);
            memcpy(capturedFrame.uBuffer, yuvBuffer_ + ysize, usize);
            memcpy(capturedFrame.vBuffer, yuvBuffer_ + ysize + usize, vsize);
        }

        void ByteDanceProcessor::processFaceDetect() {
            if (!faceDetectHandler_) {
                bef_effect_result_t ret;
                ret = bef_effect_ai_face_detect_create(
                        BEF_DETECT_SMALL_MODEL | BEF_DETECT_FULL | BEF_DETECT_MODE_VIDEO,
                        faceDetectModelPath_.c_str(), &faceDetectHandler_);
                CHECK_BEF_AI_RET_SUCCESS(ret,
                                         "ByteDanceProcessor::processFaceDetect create face detect handle failed ! %d",
                                         ret);

                void *context = AndroidContextHelper::getContext();
                ret = bef_effect_ai_face_check_license(
                        JniHelper::getJniHelper()->attachCurrentTnread(),
                        reinterpret_cast<jobject>(context), faceDetectHandler_,
                        licensePath_.c_str());
                CHECK_BEF_AI_RET_SUCCESS(ret,
                                         "ByteDanceProcessor::processFaceDetect check_license face detect failed ! %d",
                                         ret);

                ret = bef_effect_ai_face_detect_setparam(faceDetectHandler_,
                                                         BEF_FACE_PARAM_FACE_DETECT_INTERVAL, 15);

                ret = bef_effect_ai_face_detect_setparam(faceDetectHandler_,
                                                         BEF_FACE_PARAM_MAX_FACE_NUM,
                                                         BEF_MAX_FACE_NUM);
            }
            if (!faceAttributesHandler_) {
                bef_effect_result_t ret;
                ret = bef_effect_ai_face_attribute_create(0, faceAttributeModelPath_.c_str(),
                                                          &faceAttributesHandler_);
                CHECK_BEF_AI_RET_SUCCESS(ret,
                                         "ByteDanceProcessor::processFaceDetect create face attribute handle failed ! %d",
                                         ret);
                void *context = AndroidContextHelper::getContext();
                ret = bef_effect_ai_face_attribute_check_license(
                        JniHelper::getJniHelper()->attachCurrentTnread(),
                        reinterpret_cast<jobject>(context), faceAttributesHandler_,
                        licensePath_.c_str());
                CHECK_BEF_AI_RET_SUCCESS(ret,
                                         "ByteDanceProcessor::processFaceDetect check_license face attribute failed ! %d",
                                         ret);
            }

            bef_ai_face_info faceInfo;
            memset(&faceInfo, 0, sizeof(bef_ai_face_info));
            bef_effect_result_t ret;
            ret = bef_effect_ai_face_detect(faceDetectHandler_, rgbaBuffer_, BEF_AI_PIX_FMT_RGBA8888, prevFrame_.yStride, prevFrame_.height, prevFrame_.yStride * 4, BEF_AI_CLOCKWISE_ROTATE_0, BEF_DETECT_MODE_VIDEO | BEF_DETECT_FULL, &faceInfo);
            CHECK_BEF_AI_RET_SUCCESS(ret, "ByteDanceProcessor::processFaceDetect face info detect failed ! %d", ret);
            if (faceInfo.face_count != 0) {
                unsigned long long attriConfig =
                        BEF_FACE_ATTRIBUTE_AGE | BEF_FACE_ATTRIBUTE_HAPPINESS |
                        BEF_FACE_ATTRIBUTE_EXPRESSION | BEF_FACE_ATTRIBUTE_GENDER
                        | BEF_FACE_ATTRIBUTE_RACIAL | BEF_FACE_ATTRIBUTE_ATTRACTIVE;
                bef_ai_face_attribute_result attributeResult;
                ret = bef_effect_ai_face_attribute_detect_batch(faceAttributesHandler_, rgbaBuffer_,
                                                                BEF_AI_PIX_FMT_RGBA8888,
                                                                prevFrame_.yStride,
                                                                prevFrame_.height,
                                                                prevFrame_.yStride * 4,
                                                                faceInfo.base_infos,
                                                                faceInfo.face_count, attriConfig,
                                                                &attributeResult);
                CHECK_BEF_AI_RET_SUCCESS(ret, "face attribute detect failed ! %d", ret);
            }
        }


        int ByteDanceProcessor::processFrame(const agora::media::VideoFrame &capturedFrame) {
            const std::lock_guard<std::mutex> lock(mutex_);

            if (aiEffectEnabled_ || faceAttributeEnabled_) {
                prepareCachedVideoFrame(capturedFrame);
            }

            if (faceAttributeEnabled_) {
                processFaceDetect();
            }

            if (aiEffectEnabled_) {
                processEffect(capturedFrame);
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

            if (yuvBuffer_) {
                free(yuvBuffer_);
                yuvBuffer_ = nullptr;
            }
            if (rgbaBuffer_) {
                free(rgbaBuffer_);
                rgbaBuffer_ = nullptr;
            }
            prevFrame_ = {
                    media::VIDEO_PIXEL_I420,
                    0,
                    0,
                    0,
                    0,
                    0,
                    nullptr,
                    nullptr,
                    nullptr,
                    0,
                    0,
                    0
            };

            faceAttributeEnabled_ = false;
            faceDetectModelPath_.clear();
            faceAttributeModelPath_.clear();
            if (faceDetectHandler_) {
                bef_effect_ai_face_detect_destroy(faceDetectHandler_);
                faceDetectHandler_ = nullptr;
            }
            if (faceAttributesHandler_) {
                bef_effect_ai_face_attribute_destroy(faceAttributesHandler_);
                faceAttributesHandler_ = nullptr;
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
                if (aiNodeCount_ > 0) {
                    aiNodes_ = (char **) malloc(nodes.Size() * sizeof(char *));
                    for (SizeType i = 0; i < nodes.Size(); i++) {
                        if (!nodes[i].IsObject()) {
                            return -101;
                        }
                        Value &node = nodes[i];

                        if (node.HasMember("path") && node.HasMember("key") &&
                            node.HasMember("intensity")) {
                            Value &vPath = node["path"];
                            Value &vKey = node["key"];
                            Value &vIntensity = node["intensity"];
                            const char *path = vPath.GetString();
                            size_t strLength = strlen(path);
                            aiNodes_[i] = (char *) malloc((strLength + 1) * sizeof(char *));
                            strncpy(aiNodes_[i], path, strLength);
                            aiNodes_[i][strLength] = '\0';
                            aiNodeKeys_.push_back(vKey.GetString());
                            aiNodeIntensities_.push_back(vIntensity.GetFloat());
                        } else {
                            PRINTF_ERROR("plugin.bytedance.ai.composer.nodes param error: idx %d",
                                         i);
                        }
                    }
                }
                aiEffectNeedUpdate_ = true;
            }

            if (d.HasMember("plugin.bytedance.faceAttributeEnabled")) {
                Value& enabled = d["plugin.bytedance.faceAttributeEnabled"];
                if (!enabled.IsBool()) {
                    return -101;
                }
                faceAttributeEnabled_ = enabled.GetBool();
            }

            if (d.HasMember("plugin.bytedance.faceDetectModelPath")) {
                Value& faceDetectModelPath = d["plugin.bytedance.faceDetectModelPath"];
                if (!faceDetectModelPath.IsString()) {
                    return -101;
                }
                faceDetectModelPath_ = std::string(faceDetectModelPath.GetString());
            }

            if (d.HasMember("plugin.bytedance.faceAttributeModelPath")) {
                Value& attributeModelPath = d["plugin.bytedance.faceAttributeModelPath"];
                if (!attributeModelPath.IsString()) {
                    return -101;
                }
                faceAttributeModelPath_ = std::string(attributeModelPath.GetString());
            }
            return 0;
        }

        std::thread::id ByteDanceProcessor::getThreadId() {
            std::thread::id id = std::this_thread::get_id();
            return id;
        }

    }
}
