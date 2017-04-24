//
// Created by Administrator on 2017/4/10.
//
#include "native-lib.h"
int fun_attachBaseContext(JNIEnv *env, _jobject *obj, _jobject *context);
int fun_onCreate(JNIEnv *env, jobject objBundle);
static const char *gClassName = "aqcxbom/rebuildproject25/MyApplication";
static JNINativeMethod gMethods[] = {
        {"attachBaseContext", "(Landroid/content/Context;)V", (void *)fun_attachBaseContext},
        {"onCreate", "()V", (void *)fun_onCreate},
        //{"stringFromJNI", "()Ljava/lang/String;", (void *)stringFromJNI},
};

void test(JNIEnv *env)
{
    char temp[0xff] = "\0";
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("JNI_OnLoad");
    JNIEnv *env = NULL;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    if (registerNativeMethods(env, gClassName, gMethods,
                              sizeof(gMethods) / sizeof(gMethods[0])) == JNI_FALSE) {
        return JNI_ERR;
    }
    LOGI("So load success");
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
LOGI("JNI_OnUnload");
return;
}
int fun_attachBaseContext(JNIEnv *env, _jobject *obj, _jobject *context)
{
    return 0;
}
int fun_onCreate(JNIEnv *env, jobject objBundle)
{
    return 0;
}