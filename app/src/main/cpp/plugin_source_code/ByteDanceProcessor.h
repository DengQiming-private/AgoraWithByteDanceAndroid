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

            int updateEffect(const agora::media::VideoFrame &capturedFrame);

            int releaseEffectEngine();

            int setParameters(std::string parameter);

            std::thread::id getThreadId();
        protected:
            ~ByteDanceProcessor() {}
        private:
            EglCore *eglCore_ = nullptr;
            EGLSurface offscreenSurface_ = nullptr;

            bef_effect_handle_t byteEffectHandler_ = nullptr;
            std::string licensePath_;
            std::string modelDir_;
            bool engineLoaded = false;
            std::mutex mutex_;
            bool aiEffectEnabled_ = false;

            char** aiNodes_ = nullptr;
            rapidjson::SizeType aiNodeCount_ = 0;
            std::vector<float> aiNodeIntensities_;
            std::vector<std::string> aiNodeKeys_;
            bool aiEffectNeedUpdate_ = false;

        };
    }
}


#endif //AGORAWITHBYTEDANCE_BYTEDANCEPROCESSOR_H
