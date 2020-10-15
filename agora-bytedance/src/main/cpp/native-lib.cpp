#include <jni.h>
#include <string>
#include <plugin_source_code/error_code.h>
//#include "AgoraRtcKit/IAgoraService.h"
#include "plugin_source_code/ExtensionProvider.h"
#include "logutils.h"
#include "plugin_source_code/JniHelper.h"
//#include "AgoraRtcKit/AgoraRefPtr.h"
#include "plugin_source_code/JniHelper.h"
#include "plugin_source_code/EGLCore.h"

using namespace agora::extension;
static agora::extension::ExtensionProvider* extensionProvider = nullptr;

#define CHECK_EXTENSION_PROVIDER_INT if(!extensionProvider) { \
                                PRINTF_ERROR("Agora extension call api: %s err: %d", __FUNCTION__, ERROR_CODE::ERR_NOT_INIT_EXTENSION_PROVIDER); \
                                return ERROR_CODE::ERR_NOT_INIT_EXTENSION_PROVIDER; \
                             }

#define CHECK_EXTENSION_PROVIDER_VOID if(!extensionProvider) { \
                                PRINTF_ERROR("Agora extension call api: %s err: %d", __FUNCTION__, ERROR_CODE::ERR_NOT_INIT_EXTENSION_PROVIDER); \
                                return; \
                             }

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    PRINTF_INFO("JNI_OnLoad");
    JniHelper* jniHelper = JniHelper::createJniHelper(vm);

    JNIEnv *env;
    int status = vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    jclass clz = env->FindClass("io/agora/extension/AgoraPluginManager");
    jniHelper->agoraByteDanceNativeClz = reinterpret_cast<jclass>(env->NewGlobalRef(clz));
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
    PRINTF_INFO("JNI_OnUnload");
    CHECK_EXTENSION_PROVIDER_VOID;
    if (extensionProvider) {
        delete(extensionProvider);
        extensionProvider = nullptr;
    }
    JniHelper::release();
}

extern "C" JNIEXPORT jlong JNICALL
Java_io_agora_extension_provider_ByteDanceExtensionProvider_nativeCreateProvider(
        JNIEnv* env,
        jclass clazz, jobject context) {
    if (extensionProvider == nullptr){
        jobject globalContext = env->NewGlobalRef(context);
        AndroidContextHelper::setContext(globalContext);
        extensionProvider = new agora::RefCountedObject<agora::extension::ExtensionProvider>();
    }
    return reinterpret_cast<intptr_t>(extensionProvider);
}

extern "C" JNIEXPORT int JNICALL Java_io_agora_extension_AgoraPluginManager_setParameters(
                JNIEnv* env,
                jclass clazz, jstring parameters) {
    CHECK_EXTENSION_PROVIDER_INT;
    PRINTF_INFO("setParameters");
    const char* charParameter = env->GetStringUTFChars(parameters, NULL);
    std::string stringParameter(charParameter);
    extensionProvider->setParameters(charParameter);
    env->ReleaseStringUTFChars(parameters, charParameter);
    return 0;
}
