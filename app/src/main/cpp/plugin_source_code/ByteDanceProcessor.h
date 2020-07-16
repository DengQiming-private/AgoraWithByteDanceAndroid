//
// Created by DYF on 2020/7/13.
//

#ifndef AGORAWITHBYTEDANCE_BYTEDANCEPROCESSOR_H
#define AGORAWITHBYTEDANCE_BYTEDANCEPROCESSOR_H

#include <thread>
#include <string>
#include <mutex>

#include "../agora/AgoraRefPtr.h"
#include "../bytedance/bef_effect_ai_api.h"
#include "EGLCore.h"

namespace agora {
    namespace extension {
        class ByteDanceProcessor  : public RefCountInterface {
        public:
            bool initOpenGL();

            bool releaseOpenGL();

            int initEffectEngine();

            int releaseEffectEngine();

            int setParameters(std::string parameter);

            std::thread::id getThreadId();
        protected:
            ~ByteDanceProcessor() {
                PRINTF_ERROR("&&&&&&&&&&&&&~ByteDanceProcessor()");
            }
        private:
            EglCore *eglCore_ = nullptr;
            bef_effect_handle_t specialEffectHandler_ = nullptr;
            std::string licensePath_;
            std::string modelDir_;
            bool engineLoaded = false;
            std::mutex mutex_;
            bool aiEffectEnabled_ = false;
            bool aiEffectLoaded_ = false;
        };
    }
}


#endif //AGORAWITHBYTEDANCE_BYTEDANCEPROCESSOR_H
