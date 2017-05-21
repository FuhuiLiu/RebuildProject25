//
// Created by Administrator on 2017/4/10.
//
#include "native-lib.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>




int fun_attachBaseContext(JNIEnv *env, _jobject *objApplication, _jobject *context);
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
jobject g_NewApplication;
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
                                                    return false;
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
                                                        return false;
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
                                                            return false;
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
                                                                return false;
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
char g_filePath[0xff];
/**
 * 释放文件
 */
int u_releaseFile(JNIEnv *env, jobject objApplication)
{
    char timestamp[0xff];
    char clsdexPath[0xff];
    int isUpdata = 0;
    strcpy(timestamp, g_filePath);
    strcpy(clsdexPath, g_filePath);
    strcat(timestamp, "/.timestamp");
    if(!access(timestamp, 0))
    {
        stat stat1;
        int  nOpenResult = open(timestamp, O_RDWR);
        if(!fstat(nOpenResult, &stat1))
        {
            FILE* file = fdopen(nOpenResult, "w+");
            void *pNew = calloc(stat1.st_blksize, 1);
            fread(pNew, stat1.st_blksize, 1, file);
            fclose(file);
            MYLOGI("debug", "buf:%s timestamp:%s", pNew, "Thu Sep 18 19:00:59 CST 2014");
            int nCmpResult = strncmp((const char*)pNew, "Thu Sep 18 19:00:59 CST 2014", 0x1Cu);//检查是否更新
            if(nCmpResult)
                isUpdata = 1;
        }
        if(isUpdata)
        {
            MYLOGI("debug", "after update, remove files");
            strcat(clsdexPath, "/cls.dex");
            int nunlinkResult = unlink(clsdexPath);
            if(nunlinkResult)
            {
                //errno(nunlinkResult);
                MYLOGI("debug", "failed to rm files with error:%d", 999);
            }
            FILE* file = fopen(timestamp, "w");
            fwrite("Thu Sep 18 19:00:59 CST 2014", 0x1Cu, 1u, file);
            fclose(file);
        }
    }
    jclass clsApplication = env->GetObjectClass(objApplication);
    jmethodID mIDgetAssets = env->GetMethodID(clsApplication, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject objAssetManager = env->CallObjectMethod(objApplication, mIDgetAssets);
    //AssetsManager = AAssetManager_fromJava((int)env_, (int)objAssetsManager);// ????
    const char* fileAry[] = {"Charfak.jar", "Charcls.jar", "Charjuice.data"};
    for (int i = 0; i < sizeof(fileAry) / sizeof(fileAry[0]); ++i) {
        char curPath[0xff];
        sprintf(curPath, "%s/%s", g_filePath, fileAry[i]);
        if(isUpdata || access(curPath, 0))
        {
            //AAssetManager_open(AssetsManager_, pChar, ACCESS_STREAMING);// 调用AssetsManager.open获取InputStream
        }
    }
    return 0;
}
int fun_attachBaseContext(JNIEnv *env, _jobject *objApplication, _jobject *context)
{
    MYLOG("debug", "in...");
    //初始化必要类及方法
    bool bRet = initClassesAndMethods(env);
    //初始化成功
    if(bRet)
    {
        //Application;->attachBaseContext
        env->CallNonvirtualVoidMethod(objApplication, g_clsContextWrapper, g_MethodIDattachBaseContext, context);
        jclass clsApplication = env->GetObjectClass(objApplication);
        jmethodID mIDgetFilesDir = env->GetMethodID(clsApplication, "getFilesDir", "()Ljava/io/File;");
        //Application;->getFilesDir
        jobject objFileFilesDir = env->CallObjectMethod(objApplication, mIDgetFilesDir);
        //File
        jclass clsFile = env->GetObjectClass(objFileFilesDir);
        jmethodID mIDgetAbsolutePath = env->GetMethodID(clsFile, "getAbsolutePath", "()Ljava/lang/String;");
        //File;->getAbsolutePath
        jstring strFilesDirAbsolutePath = (jstring)env->CallObjectMethod(objFileFilesDir, mIDgetAbsolutePath);
        //获取app目录\files全路径
        const char* pPath = env->GetStringUTFChars(strFilesDirAbsolutePath, JNI_FALSE);
        if(pPath == NULL)
        {
            return 0;
        }
        MYLOGI("FilesDir:%s", pPath);
        strcpy(g_filePath, pPath);
        jobject objApplicationInfo = env->CallObjectMethod(objApplication, g_MethodIDgetApplicationInfo, JNI_FALSE);
        if(!objApplicationInfo)
        {
            return 0;
        }
        char NativeLibraryDir[0xff];
        NativeLibraryDir[0] = '\0';
        if(g_nSDKINT <= 8)
        {
            jstring strDataDir = (jstring)env->GetObjectField(objApplicationInfo, g_FieldIDdataDir);
            const char* pDataDir = env->GetStringUTFChars(strDataDir, JNI_FALSE);
            if(!pDataDir)
                return 0;
            strncpy(NativeLibraryDir, pDataDir, strlen(pDataDir));
            strcat(NativeLibraryDir, "/lib");
        }
        else
        {
            jstring strnativeLibraryDir = (jstring)env->GetObjectField(objApplicationInfo, g_FieldIDnativeLibraryDir);
            const char* pDataDir = env->GetStringUTFChars(strnativeLibraryDir, JNI_FALSE);
            if(!pDataDir)
                return 0;
            strncpy(NativeLibraryDir, pDataDir, strlen(pDataDir));
            strcat(NativeLibraryDir, "/lib");
        }
        MYLOGI("global native path is %s", NativeLibraryDir);
        //application;->getPackageResourcePath
        jstring strPackageResourcePath = (jstring)env->CallObjectMethod(objApplication, g_MethodIDgetPackageResourcePath);
        const char* pPackageResourcePath = env->GetStringUTFChars(strPackageResourcePath, JNI_FALSE);
        if(!pPackageResourcePath)
            return 0;
        setenv("APKPATH", pPackageResourcePath, 1);
        MYLOGI( "global apk path is %s", pPackageResourcePath);
        //释放三个加密相关的文件
        //....
        u_releaseFile(env, objApplication);

        //context;->getPackageName
        jstring strPackageName = (jstring)env->CallObjectMethod(context, g_MethodIDgetPackageName);
        const char* pPackageName = NULL;
        if(strPackageName)
        {
            pPackageName = env->GetStringUTFChars(strPackageName, JNI_FALSE);
        }
        if(!pPackageName)
            return 0;
        jobject objClassLoader = env->CallObjectMethod(context, g_MethodIDgetClassLoader);
        //解析dex
        //parse_dex ...
        //replace_classloader_cookie
        MYLOGI("debug", "enter new application");
        jstring string_androidappApplication = env->NewStringUTF("android.app.Application");
        //classLoader;->findClass("android.app.Application")
        jobject objNewApplication = NULL;
        jclass clsApplication2 = (jclass)env->CallObjectMethod(objClassLoader, g_MethodIDfindClass, string_androidappApplication);
        //new一个Application并调用其attach到context
        if(clsApplication2)
        {
            jmethodID mIDinit = env->GetMethodID(clsApplication2, "<init>", "()V");
            objNewApplication = env->NewObject(clsApplication2, mIDinit);
            env->CallVoidMethod(g_NewApplication, g_MethodIDattach, context);
            env->DeleteLocalRef(clsApplication2);
            MYLOGI("debug", "exit new application");
        }
        else
        {
            MYLOGI("debug", "can't findClass realAppClass");
        }
        if(objNewApplication)
        {
            g_NewApplication = env->NewGlobalRef(objNewApplication);
        }
        MYLOGI("debug", "exit attachBaseContext");
    }
    return 0;
}
/*
 * 还有问题！
 */
int fun_onCreate(JNIEnv *env, jobject objBundle)
{
    if (g_NewApplication)
    {
        MYLOGI("debug", "enter replace_app_application");
        jobject objcurrentActivityThread = env->CallStaticObjectMethod(g_ClsActivityThread, g_MethodIDcurrentActivityThread, 0xFFFFFC4C);
        //ActivityThread;->mPackages
        jobject objmPackages = env->GetObjectField(objcurrentActivityThread, g_FieldIDmPackages);
        //这个字串来源不明
        jstring strUnknow = env->NewStringUTF("");
        if(g_nSDKINT > 18)
        {
            env->CallObjectMethod(objmPackages, g_MethodIDfindClass, strUnknow);
            //jobject obj_mPackages = env->CallObjectMethod(objmPackages, )
        }
        else
        {}
        jobject objLoadedApk = env->CallObjectMethod(objmPackages, g_MethodIDget);
        env->SetObjectField(objLoadedApk, g_FieldIDmApplication, g_NewApplication);
        env->SetObjectField(objcurrentActivityThread, g_FieldIDmInitialApplication, g_NewApplication);
        jobject objmBoundApplication = env->GetObjectField(objcurrentActivityThread, g_FieldIDmBoundApplication);
        jobject objLoadedApk2 = env->GetObjectField(objmBoundApplication, g_FieldIDinfo);
        env->SetObjectField(objLoadedApk2, g_FieldIDmApplication, g_NewApplication);
        jobject objmAllApplications = env->GetObjectField(objcurrentActivityThread, g_FieldIDmAllApplications);
        int nSize = env->CallIntMethod(objmAllApplications, g_MethodIDsize);
        for (int i = 0; i < nSize; ++i) {
            jobject objApplicationN = env->CallObjectMethod(objmAllApplications, g_MethodIDget, i);
            MYLOGI("compare: i=%d item=%p", i, objApplicationN);
            jboolean bResult = env->IsSameObject(objBundle, objApplicationN);
            if(bResult)
            {
                MYLOGI("debug", "replace: find same replace");
                env->CallObjectMethod(objmAllApplications, g_MethodIDset, i, g_NewApplication);
            }
            env->DeleteLocalRef(objApplicationN);
        }
        env->DeleteLocalRef(objmAllApplications);
        env->DeleteLocalRef(objLoadedApk2);
        env->DeleteLocalRef(objmBoundApplication);
        env->DeleteLocalRef(objLoadedApk);
        env->DeleteLocalRef(objmPackages);
        env->DeleteLocalRef(objcurrentActivityThread);
        MYLOGI( "debug", "exit replace_app_application");
        MYLOGI( "debug", "call onCreate start");
        env->CallVoidMethod(g_NewApplication, g_MethodIDonCreate);
        MYLOGI( "debug", "call onCreate end");
        env->DeleteGlobalRef(g_NewApplication);
        return 1;
    }
    else
        MYLOGI("debug", "no g_realApplication just free classes and assets");
    return 0;
}