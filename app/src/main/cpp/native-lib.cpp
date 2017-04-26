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

int g_NewClsGlabel(JNIEnv *pEnv, jclass *pCls, const char* pClsName)
{
    int nRet = -1;
    jclass clsFound = pEnv->FindClass(pClsName);
    if(clsFound)
    {
        jobject obj = pEnv->NewGlobalRef(clsFound);
        *pCls = (jclass)obj;
        nRet = 0;
    }
    return nRet;
}
jclass g_ClsBuildVersion;
jclass g_ClsActivityThread;
jclass g_ClsArrayMap;
jclass g_ClsHashMap;
jclass g_clsAppBindData;
jclass g_clsArrayList;
jclass g_clsContext;
jclass g_clsWeakReference;
jclass g_clsLoadedApk;
jclass g_clsApplication;
jclass g_clsContextWrapper;
jclass g_clsPathClassLoader;
jclass g_clsBaseDexClassLoader;
jclass g_clsDexPathList;
jclass g_clsElement;
jclass g_clsFile;
jclass g_clsDexFile;
jclass g_clsClassLoader;
jclass g_clsSystem;

jfieldID g_FieldIDSDK_INT;
jfieldID g_FieldIDmPackages;
jfieldID g_FieldIDmBoundApplication;
jfieldID g_FieldIDmInitialApplication;
jfieldID g_FieldIDmAllApplications;
jfieldID g_FieldIDinfo;
jfieldID g_FieldIDmClassLoader;
jfieldID g_FieldIDmApplication;
jfieldID g_FieldIDdataDir;
jfieldID g_FieldIDnativeLibraryDir;
jfieldID g_FieldIDsourceDir;
jfieldID g_FieldIDpathList;
jfieldID g_FieldIDdexElements;
jfieldID g_FieldIDdexfile;
jfieldID g_FieldIDfile;
jfieldID g_FieldIDmDexs;
jfieldID g_FieldIDmCookie;

jmethodID g_MethodIDcurrentActivityThread;
jmethodID g_MethodIDsize;
jmethodID g_MethodIDget;
jmethodID g_MethodIDset;
jmethodID g_MethodIDgetPackageName;
jmethodID g_MethodIDgetApplicationInfo;
jmethodID g_MethodIDgetClassLoader;
jmethodID g_MethodIDgetAssets;
jmethodID g_MethodIDgetPackageResourcePath;
jmethodID g_MethodIDWeakReferenceget;
jmethodID g_MethodIDonCreate;
jmethodID g_MethodIDattach;
jmethodID g_MethodIDattachBaseContext;
jmethodID g_MethodIDgetAbsolutePath;
jmethodID g_StaticMethodIDopenDexFile;
jmethodID g_MethodIDfindClass;
jmethodID g_StaticMethodIDgetProperty;

bool g_isDalvik;
bool g_isArt;

int g_nSDKINT;
int initClassesAndMethods(JNIEnv *pEnv)
{
    MYLOG("debug", "enter init classes");
    if ( g_NewClsGlabel(pEnv, &g_ClsBuildVersion, "android/os/Build$VERSION") )
    {
        MYLOG("debug", "ERROR: class version");
    }
    else
    {
        g_FieldIDSDK_INT = pEnv->GetStaticFieldID(g_ClsBuildVersion, "SDK_INT", "I");
        g_nSDKINT = pEnv->GetStaticIntField(g_ClsBuildVersion, g_FieldIDSDK_INT);
        if ( g_NewClsGlabel(pEnv, &g_ClsActivityThread, "android/app/ActivityThread") )
        {
            MYLOG("debug", "ERROR: class ActivityThread");
        }
        else
        {
            MYLOG( "debug", "sdk_int is %d", g_nSDKINT);
            if(g_nSDKINT > 18)
            {
                g_FieldIDmPackages = pEnv->GetFieldID(g_ClsActivityThread, "mPackages", "Landroid/util/ArrayMap;");
                if ( g_NewClsGlabel(pEnv, &g_ClsArrayMap, "android/util/ArrayMap") )
                {
                    MYLOG( "debug", "ERROR: ArrayMap");
                    return false;
                }
                g_MethodIDget = pEnv->GetMethodID(g_ClsArrayMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
            }
            else
            {
                g_FieldIDmPackages = pEnv->GetFieldID(g_ClsActivityThread, "mPackages", "Ljava/util/HashMap;");
                if ( g_NewClsGlabel(pEnv, &g_ClsHashMap, "android/util/ArrayMap") )
                {
                    MYLOG( "debug", "ERROR: HashMap");
                    return false;
                }
                g_MethodIDget = pEnv->GetMethodID(g_ClsHashMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
            }
            g_FieldIDmBoundApplication = pEnv->GetFieldID(g_ClsActivityThread, "mBoundApplication", "Landroid/app/ActivityThread$AppBindData;");
            g_FieldIDmInitialApplication = pEnv->GetFieldID(g_ClsActivityThread,
                                                            "mInitialApplication",
                                                            "Landroid/app/Application;");
            g_FieldIDmAllApplications = pEnv->GetFieldID(
                                                        g_ClsActivityThread,
                                                        "mAllApplications",
                                                        "Ljava/util/ArrayList;");
            g_MethodIDcurrentActivityThread = pEnv->GetStaticMethodID(
                                                                    g_ClsActivityThread,
                                                                    "currentActivityThread",
                                                                    "()Landroid/app/ActivityThread;");
            if ( g_NewClsGlabel(pEnv, &g_clsAppBindData, "android/app/ActivityThread$AppBindData") )
            {
                MYLOG( "debug", "ERROR: class AppBindData");
            }
            else
            {
                g_FieldIDinfo = pEnv->GetFieldID(g_clsAppBindData, "info", "Landroid/app/LoadedApk;");
                if ( g_NewClsGlabel(pEnv, &g_clsArrayList, "java/util/ArrayList") )
                {
                    MYLOG( "debug", "ERROR: class ArrayList");
                }
                else
                {
                    g_MethodIDsize  = pEnv->GetMethodID(g_clsArrayList, "size", "()I");
                    g_MethodIDget  = pEnv->GetMethodID(g_clsArrayList, "get", "(I)Ljava/lang/Object;");
                    g_MethodIDset  = pEnv->GetMethodID(g_clsArrayList, "set", "(ILjava/lang/Object;)Ljava/lang/Object;");
                    if ( g_NewClsGlabel(pEnv, &g_clsContext, "android/content/Context") )
                    {
                        MYLOG( "debug", "ERROR: class Context");
                    }
                    else
                    {
                        g_MethodIDgetPackageName = pEnv->GetMethodID(g_clsContext, "getPackageName", "()Ljava/lang/String;");
                        g_MethodIDgetPackageName = pEnv->GetMethodID(g_clsContext, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
                        g_MethodIDgetPackageName = pEnv->GetMethodID(g_clsContext, "getClassLoader", "()Ljava/lang/ClassLoader;");
                        g_MethodIDgetPackageName = pEnv->GetMethodID(g_clsContext, "getAssets", "()Landroid/content/res/AssetManager;");
                        g_MethodIDgetPackageName = pEnv->GetMethodID(g_clsContext, "getPackageResourcePath", "()Ljava/lang/String;");
                        if(g_NewClsGlabel(pEnv, &g_clsWeakReference, "java/lang/ref/WeakReference"))
                        {
                            MYLOG( "debug", "ERROR: class WeakReference");
                        }
                        else
                        {
                            g_MethodIDWeakReferenceget = pEnv->GetMethodID(g_clsWeakReference, "get", "()Ljava/lang/Object;");
                            if ( g_NewClsGlabel(pEnv, &g_clsLoadedApk, "android/app/LoadedApk") )
                            {
                                MYLOG( "debug", "ERROR: class LoadedApk");
                            }
                            else
                            {
                                jclass g_clsApplicationInfo;
                                g_FieldIDmClassLoader = pEnv->GetFieldID(
                                                                        g_clsLoadedApk,
                                                                        "mClassLoader",
                                                                        "Ljava/lang/ClassLoader;");
                                g_FieldIDmApplication = pEnv->GetFieldID(
                                                                        g_clsLoadedApk,
                                                                        "mApplication",
                                                                        "Landroid/app/Application;");
                                if ( g_NewClsGlabel(pEnv, &g_clsApplicationInfo, "android/content/pm/ApplicationInfo") )
                                {
                                    MYLOG( "debug", "ERROR: class ApplicationInfo");
                                }
                                else
                                {
                                    g_FieldIDdataDir = pEnv->GetFieldID(g_clsApplicationInfo, "dataDir", "Ljava/lang/String;");
                                    g_FieldIDnativeLibraryDir = pEnv->GetFieldID(
                                            g_clsApplicationInfo,
                                            "nativeLibraryDir",
                                            "Ljava/lang/String;");
                                    g_FieldIDsourceDir = pEnv->GetFieldID(
                                            g_clsApplicationInfo,
                                            "sourceDir",
                                            "Ljava/lang/String;");
                                    if ( g_NewClsGlabel(pEnv, &g_clsApplication, "android/app/Application") )
                                    {
                                        MYLOG( "debug", "ERROR: class Application");
                                    }
                                    else
                                    {
                                        g_MethodIDonCreate = pEnv->GetMethodID(g_clsApplication, "onCreate", "()V");
                                        g_MethodIDattach = pEnv->GetMethodID(
                                                g_clsApplication,
                                                "attach",
                                                "(Landroid/content/Context;)V");
                                        if(g_NewClsGlabel(pEnv, &g_clsContextWrapper, "android/content/ContextWrapper"))
                                        {
                                            MYLOG( "debug", "ERROR: class ContextWrapper");
                                        }
                                        else
                                        {
                                            g_MethodIDattachBaseContext = pEnv->GetMethodID(
                                                    g_clsContextWrapper,
                                                    "attachBaseContext",
                                                    "(Landroid/content/Context;)V");
                                            MYLOG( "debug", "PathClassLoader start");
                                            if ( g_NewClsGlabel(pEnv, &g_clsPathClassLoader, "dalvik/system/PathClassLoader") )
                                            {
                                                MYLOG( "debug", "ERROR: class PathClassLoader");
                                            }
                                            else
                                            {
                                                if(g_nSDKINT > 13)
                                                {
                                                    if ( g_NewClsGlabel(pEnv, &g_clsBaseDexClassLoader, "dalvik/system/BaseDexClassLoader") )
                                                    {
                                                        MYLOG( "debug", "ERROR: BaseDexClassLoader");
                                                    }
                                                    g_FieldIDpathList = pEnv->GetFieldID(
                                                            g_clsBaseDexClassLoader,
                                                            "pathList",
                                                            "Ldalvik/system/DexPathList;");
                                                    if ( g_NewClsGlabel(pEnv, &g_clsDexPathList, "dalvik/system/DexPathList") )
                                                    {
                                                        MYLOG( "debug", "ERROR: class DexPathList");
                                                        return false;
                                                    }
                                                    g_FieldIDdexElements = pEnv->GetFieldID(
                                                            g_clsDexPathList,
                                                            "dexElements",
                                                            "[Ldalvik/system/DexPathList$Element;");
                                                    if ( g_NewClsGlabel(pEnv, &g_clsElement, "dalvik/system/DexPathList$Element") )
                                                    {
                                                        MYLOG( "debug", "ERROR: class Element");
                                                        return false;
                                                    }
                                                    g_FieldIDdexfile = pEnv->GetFieldID(
                                                            g_clsElement,
                                                            "dexFile",
                                                            "Ldalvik/system/DexFile;");
                                                    g_FieldIDfile = pEnv->GetFieldID(g_clsElement, "file", "Ljava/io/File;");
                                                }
                                                else
                                                {
                                                    g_FieldIDmDexs = pEnv->GetFieldID(
                                                            g_clsPathClassLoader,
                                                            "mDexs",
                                                            "[Ldalvik/system/DexFile;");
                                                }
                                                if ( g_NewClsGlabel(pEnv, &g_clsFile, "java/io/File") )
                                                {
                                                    MYLOG( "debug", "ERROR: class File");
                                                }
                                                else
                                                {
                                                    g_MethodIDgetAbsolutePath = pEnv->GetMethodID(
                                                            g_clsFile,
                                                            "getAbsolutePath",
                                                            "()Ljava/lang/String;");
                                                    MYLOG( "debug", "PathClassLoader end");
                                                    if ( g_NewClsGlabel(pEnv, &g_clsDexFile, "dalvik/system/DexFile") )
                                                    {
                                                        MYLOG( "debug", "ERROR: class DexFile");
                                                    }
                                                    else
                                                    {
                                                        if(g_nSDKINT > 19)
                                                        {
                                                            g_FieldIDmCookie = pEnv->GetFieldID(
                                                                    g_clsDexFile,
                                                                    "mCookie",
                                                                    "J");
                                                            g_StaticMethodIDopenDexFile = pEnv->GetStaticMethodID(g_clsDexFile, "openDexFile", "(Ljava/lang/String;Ljava/lang/String;I)J");
                                                        }
                                                        else
                                                        {
                                                            g_FieldIDmCookie = pEnv->GetFieldID(
                                                                    g_clsDexFile,
                                                                    "mCookie",
                                                                    "I");
                                                            g_StaticMethodIDopenDexFile = pEnv->GetStaticMethodID(g_clsDexFile, "openDexFile", "(Ljava/lang/String;Ljava/lang/String;I)I");
                                                        }
                                                        if ( g_NewClsGlabel(pEnv, &g_clsClassLoader, "java/lang/ClassLoader") )
                                                        {
                                                            MYLOG( "debug", "ERROR: class ClassLoader");
                                                        }
                                                        else
                                                        {
                                                            g_MethodIDfindClass = pEnv->GetMethodID(
                                                                    g_clsClassLoader,
                                                                    "findClass",
                                                                    "(Ljava/lang/String;)Ljava/lang/Class;");
                                                            MYLOG( "debug", "System start");
                                                            if(g_NewClsGlabel(pEnv, &g_clsSystem, "java/lang/System"))
                                                            {
                                                                MYLOG( "debug", "ERROR: class System");
                                                            }
                                                            else
                                                            {
                                                                g_StaticMethodIDgetProperty = pEnv->GetStaticMethodID(
                                                                        g_clsSystem,
                                                                        "getProperty",
                                                                        "(Ljava/lang/String;)Ljava/lang/String;");
                                                                MYLOG( "debug", "exit init classes normal");
                                                                jstring strVersion = pEnv->NewStringUTF("java.vm.version");
                                                                jstring objVersion = (jstring)pEnv->CallStaticObjectMethod(g_clsSystem, g_StaticMethodIDgetProperty, strVersion);
                                                                const char* pVersion = (const char*)pEnv->GetStringUTFChars(objVersion, 0);
                                                                int nSub = *pVersion - 49;
                                                                g_isArt = (nSub >0 ? true : false);
                                                                g_isDalvik = g_isArt ^ 1;

                                                                pEnv->ReleaseStringChars(strVersion, (const jchar*)pVersion);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}
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
    MYLOG("debug", "in...");
    initClassesAndMethods(env);
    return 0;
}
int fun_onCreate(JNIEnv *env, jobject objBundle)
{
    return 0;
}