#include <jni.h>
#include <string>
//#include "AgoraRtcKit/IAgoraService.h"
#include "plugin_source_code/ExtensionProvider.h"
#include "logutils.h"
#include "plugin_source_code/JniHelper.h"
//#include "AgoraRtcKit/AgoraRefPtr.h"
#include "plugin_source_code/PluginManager.h"
#include "plugin_source_code/JniHelper.h"
#include "plugin_source_code/EGLCore.h"

using namespace agora::extension;
static PluginManager* pluginManager = nullptr;
static EglCore* eglCore = nullptr;

#define CHECK_PLUGIN_MANAGER_INT if(!pluginManager) { \
                                PRINTF_ERROR("Agora extension call api: %s err: %d", __FUNCTION__, ERROR_CODE::ERR_NOT_INIT_PLUGIN_MANAGER); \
                                return ERROR_CODE::ERR_NOT_INIT_PLUGIN_MANAGER; \
                             }

#define CHECK_PLUGIN_MANAGER_VOID if(!pluginManager) { \
                                PRINTF_ERROR("Agora extension call api: %s err: %d", __FUNCTION__, ERROR_CODE::ERR_NOT_INIT_PLUGIN_MANAGER); \
                                return; \
                             }

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    PRINTF_INFO("JNI_OnLoad");
    JniHelper* jniHelper = JniHelper::createJniHelper(vm);

    JNIEnv *env;
    int status = vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    jclass clz = env->FindClass("io/agora/extension/AgoraPluginManager");
    jniHelper->agoraByteDanceNativeClz = reinterpret_cast<jclass>(env->NewGlobalRef(clz));
    pluginManager = new PluginManager();
    pluginManager->loadPlugin();
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
    PRINTF_INFO("JNI_OnUnload");
    CHECK_PLUGIN_MANAGER_VOID;
    pluginManager->unloadPlugin();
    if (pluginManager) {
        delete(pluginManager);
        pluginManager = nullptr;
    }
    JniHelper::release();
}

extern "C" JNIEXPORT int JNICALL Java_io_agora_extension_AgoraPluginManager_setParameters(
        JNIEnv* env,
        jclass clazz, jstring parameters) {
    CHECK_PLUGIN_MANAGER_INT;
    const char* charParameter = env->GetStringUTFChars(parameters, NULL);
    std::string stringParameter(charParameter);
    pluginManager->setParameters(charParameter);
    env->ReleaseStringUTFChars(parameters, charParameter);
    return 0;
}

extern "C" JNIEXPORT int JNICALL
Java_io_agora_extension_AgoraPluginManager_setContext(
        JNIEnv* env,
        jclass clazz, jobject context) {
    CHECK_PLUGIN_MANAGER_INT;
    jobject globalContext = env->NewGlobalRef(context);
    AndroidContextHelper::setContext(globalContext);

//    bef_effect_handle_t specialEffectHandler_;
//    bef_effect_result_t result;
//    result = bef_effect_ai_create(&specialEffectHandler_);
//    PRINTF_INFO("&&&bef_effect_ai_create result : %d", result);
//
//    std::string licensePath_ = "/storage/sdcard0/Android/data/com.bytedance.labcv.demo/files/assets/resource/LicenseBag.bundle/labcv_test_20200314_20200515_com.bytedance.labcv.demo_labcv_test_v3.9.0.licbag";
//    result = bef_effect_ai_check_license(
//            env,
//            context, specialEffectHandler_,
//            licensePath_.c_str());
//    PRINTF_INFO("&&&bef_effect_ai_check_license : %d , license_path : %s", result,
//                licensePath_.c_str());
////    result = bef_effect_ai_init(specialEffectHandler_, 0, 0, modelDir_.c_str(), "");
//    PRINTF_INFO("&&&bef_effect_ai_init result : %d ", result);

    return ERROR_CODE::ERROR_OK;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_io_agora_extension_AgoraPluginManager_isEnable(
        JNIEnv* env,
        jclass clazz) {
    CHECK_PLUGIN_MANAGER_INT;
    return pluginManager->isEnable();
}

extern "C" JNIEXPORT int JNICALL
Java_io_agora_extension_AgoraPluginManager_setEnable(
        JNIEnv* env,
        jclass clazz, jboolean enable) {
    CHECK_PLUGIN_MANAGER_INT;
    pluginManager->setEnable(enable);
    return ERROR_CODE::ERROR_OK;
}

extern "C" JNIEXPORT int JNICALL
Java_io_agora_extension_AgoraPluginManager_releaseContext(
        JNIEnv* env,
        jclass clazz) {
    AndroidContextHelper::releaseContext(env);
    return ERROR_CODE::ERROR_OK;
}

extern "C" void initGL() {
//    if (!eglCore) {
//        eglCore = new EglCore();
//        eglCore->makeCurrent();
//    }
}
extern "C" void releaseGL() {
//    if (eglCore) {
//        delete eglCore;
//        eglCore = nullptr;
//    }
}
extern "C" void makeCurrent() {
//    if (eglCore) {
//        eglCore->makeCurrent();
//    }
}
extern "C" void dataCallback(const char * data) {
    JniHelper * jniHelper = JniHelper::getJniHelper();
    JNIEnv* env = jniHelper->attachCurrentTnread();
    if (env != nullptr) {
        jmethodID onDataCallbackFunc = env->GetStaticMethodID(jniHelper->agoraByteDanceNativeClz, "onDataCallback", "(Ljava/lang/String;)V");
        jstring javaMsg = env->NewStringUTF(data);
        env->CallStaticVoidMethod(jniHelper->agoraByteDanceNativeClz, onDataCallbackFunc, javaMsg);
        env->DeleteLocalRef(javaMsg);
    }
}
