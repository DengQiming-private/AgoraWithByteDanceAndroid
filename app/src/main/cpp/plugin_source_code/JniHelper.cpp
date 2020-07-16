//
// Created by 张涛 on 2020/2/27.
//

#include "JniHelper.h"

namespace agora {
    namespace extension {
        JniHelper *JniHelper::jniHelper = nullptr;

        JniHelper::JniHelper(JavaVM *jvm) : javaVm(jvm) {

        }

        JniHelper::~JniHelper() {
            javaVm = nullptr;
        }

        JNIEnv *JniHelper::getEnv() {
            JNIEnv *env = nullptr;
            int status = javaVm->GetEnv((void **) &env, JNI_VERSION_1_6);
            return env;
        }

        void JniHelper::release() {
            if (jniHelper) {
                delete jniHelper;
                jniHelper = nullptr;
            }
        }

        JniHelper *JniHelper::createJniHelper(JavaVM *jvm) {
            if (!jniHelper) {
                jniHelper = new JniHelper(jvm);
            }
            return jniHelper;
        }

        JniHelper *JniHelper::getJniHelper() {
            return jniHelper;
        }

        JNIEnv *JniHelper::attachCurrentTnread() {
            JNIEnv *env = nullptr;
            int status = javaVm->GetEnv((void **) &env, JNI_VERSION_1_6);
            bool isAttached = false;
            if (status == JNI_EDETACHED) {
                status = javaVm->AttachCurrentThread(&env, nullptr);
                if (status != 0) {
                    return nullptr;
                }
                isAttached = true;
                return env;
            }
            return env;
        }

        void JniHelper::detachCurrentThread() {
            if (isAttached) {
                javaVm->DetachCurrentThread();
            }
            isAttached = false;
        }

        void * AndroidContextHelper::_context = nullptr;
        void AndroidContextHelper::setContext(void *context) {
            _context = context;
        }
        void * AndroidContextHelper::getContext() {
            return _context;
        }
        void AndroidContextHelper::releaseContext(JNIEnv *env) {
            if(_context) {
                env->DeleteGlobalRef(static_cast<jobject>(_context));
                _context = nullptr;
            }
        }
    }
}
