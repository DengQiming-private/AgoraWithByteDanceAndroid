//
// Created by DYF on 2020/7/13.
//

#ifndef AGORAWITHBYTEDANCE_BYTEDANCEPROCESSOR_H
#define AGORAWITHBYTEDANCE_BYTEDANCEPROCESSOR_H

#include <thread>
#include <string>
#include <mutex>
#include <vector>

#include "../agora/AgoraRefPtr.h"
#include "../agora/AgoraMediaBase.h"
#include "../bytedance/bef_effect_ai_api.h"
#include "EGLCore.h"
#include "rapidjson/rapidjson.h"

namespace agora {
    namespace extension {
        class ByteDanceProcessor  : public RefCountInterface {
        public:
            bool initOpenGL();

            bool releaseOpenGL();

            int processFrame(const agora::media::VideoFrame &capturedFrame);

            int releaseEffectEngine();

            int setParameters(std::string parameter);
            void onDataCallback(std::string ballback);

            std::thread::id getThreadId();
        protected:
            ~ByteDanceProcessor() {}
        private:
            void processFaceDetect();
            void processEffect(const agora::media::VideoFrame &capturedFrame);
            void prepareCachedVideoFrame(const agora::media::VideoFrame &capturedFrame);

            EglCore *eglCore_ = nullptr;
            EGLSurface offscreenSurface_ = nullptr;
            std::mutex mutex_;

            bef_effect_handle_t byteEffectHandler_ = nullptr;
            std::string licensePath_;
            std::string modelDir_;
            bool aiEffectEnabled_ = false;
            char** aiNodes_ = nullptr;
            rapidjson::SizeType aiNodeCount_ = 0;
            std::vector<float> aiNodeIntensities_;
            std::vector<std::string> aiNodeKeys_;
            bool aiEffectNeedUpdate_ = false;

            bool faceAttributeEnabled_ = false;
            std::string faceDetectModelPath_;
            std::string faceAttributeModelPath_;
            bef_effect_handle_t faceDetectHandler_ = nullptr;
            bef_effect_handle_t faceAttributesHandler_ = nullptr;

            agora::media::VideoFrame prevFrame_ = {
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
            unsigned char* yuvBuffer_ = nullptr;
            unsigned char* rgbaBuffer_ = nullptr;

        };
    }
}


#endif //AGORAWITHBYTEDANCE_BYTEDANCEPROCESSOR_H
