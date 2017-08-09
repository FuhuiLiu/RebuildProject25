//
// Created by Administrator on 2017/4/10.
//
#include "native-lib.h"
#include "../../../../../../Android/android-sdk/ndk-bundle/platforms/android-15/arch-x86/usr/include/sys/stat.h"
#include "../../../../../../Android/android-sdk/ndk-bundle/platforms/android-21/arch-mips64/usr/include/sys/stat.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include <sys/mman.h> // mmap
#include <dlfcn.h>
#include <jni.h>


//typedef void ( *FuncOpenDex)(unsigned int * bAry, DexOrJar **);
typedef void (*FuncOpenDex)(const unsigned int *, jvalue *);

int parse_dex(JNIEnv *env, DexOrJar **pOutDexOrJar);
int fun_attachBaseContext(JNIEnv *env, _jobject *objApplication, _jobject *context);
int fun_onCreate(JNIEnv *env, jobject objBundle);
// 替换对应结构Cookie结构，用pDexOrJar指针
int replase_classloader_cookie(JNIEnv *env, jobject classloader, DexOrJar* pDexOrJar);

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
jmethodID g_MethodIDArrayListsize;
jmethodID g_MethodIDArrayListget;
jmethodID g_MethodIDArrayMapget;
jmethodID g_MethodIDHashMapget;
jmethodID g_MethodIDArrayListset;
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
char g_PackageName[0xff]; //存储包名
int g_nSDKINT;

bool initClassesAndMethods(JNIEnv *pEnv)
{
    MYLOGI("enter init classes");
    if ( g_NewClsGlabel(pEnv, &g_ClsBuildVersion, "android/os/Build$VERSION") )
    {
        MYLOGI("ERROR: class version");
    }
    else
    {
        g_FieldIDSDK_INT = pEnv->GetStaticFieldID(g_ClsBuildVersion, "SDK_INT", "I");
        g_nSDKINT = pEnv->GetStaticIntField(g_ClsBuildVersion, g_FieldIDSDK_INT);
        if ( g_NewClsGlabel(pEnv, &g_ClsActivityThread, "android/app/ActivityThread") )
        {
            MYLOGI("ERROR: class ActivityThread");
        }
        else
        {
            MYLOGI("sdk_int is %d", g_nSDKINT);
            if(g_nSDKINT > 18)
            {
                g_FieldIDmPackages = pEnv->GetFieldID(g_ClsActivityThread, "mPackages", "Landroid/util/ArrayMap;");
                if ( g_NewClsGlabel(pEnv, &g_ClsArrayMap, "android/util/ArrayMap") )
                {
                    MYLOGI("ERROR: ArrayMap");
                    return false;
                }
                g_MethodIDArrayMapget = pEnv->GetMethodID(g_ClsArrayMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
            }
            else
            {
                g_FieldIDmPackages = pEnv->GetFieldID(g_ClsActivityThread, "mPackages", "Ljava/util/HashMap;");
                if ( g_NewClsGlabel(pEnv, &g_ClsHashMap, "android/util/HashMap") )
                {
                    MYLOGI("ERROR: HashMap");
                    return false;
                }
                g_MethodIDHashMapget = pEnv->GetMethodID(g_ClsHashMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
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
                MYLOGI("ERROR: class AppBindData");
            }
            else
            {
                g_FieldIDinfo = pEnv->GetFieldID(g_clsAppBindData, "info", "Landroid/app/LoadedApk;");
                if ( g_NewClsGlabel(pEnv, &g_clsArrayList, "java/util/ArrayList") )
                {
                    MYLOGI("ERROR: class ArrayList");
                }
                else
                {
                    g_MethodIDArrayListsize  = pEnv->GetMethodID(g_clsArrayList, "size", "()I");
                    g_MethodIDArrayListget  = pEnv->GetMethodID(g_clsArrayList, "get", "(I)Ljava/lang/Object;");
                    g_MethodIDArrayListset  = pEnv->GetMethodID(g_clsArrayList, "set", "(ILjava/lang/Object;)Ljava/lang/Object;");
                    if ( g_NewClsGlabel(pEnv, &g_clsContext, "android/content/Context") )
                    {
                        MYLOGI("ERROR: class Context");
                    }
                    else
                    {
                        g_MethodIDgetPackageName = pEnv->GetMethodID(g_clsContext, "getPackageName", "()Ljava/lang/String;");
                        g_MethodIDgetApplicationInfo = pEnv->GetMethodID(g_clsContext, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
                        g_MethodIDgetClassLoader = pEnv->GetMethodID(g_clsContext, "getClassLoader", "()Ljava/lang/ClassLoader;");
                        g_MethodIDgetAssets = pEnv->GetMethodID(g_clsContext, "getAssets", "()Landroid/content/res/AssetManager;");
                        g_MethodIDgetPackageResourcePath = pEnv->GetMethodID(g_clsContext, "getPackageResourcePath", "()Ljava/lang/String;");
                        if(g_NewClsGlabel(pEnv, &g_clsWeakReference, "java/lang/ref/WeakReference"))
                        {
                            MYLOGI("ERROR: class WeakReference");
                        }
                        else
                        {
                            g_MethodIDWeakReferenceget = pEnv->GetMethodID(g_clsWeakReference, "get", "()Ljava/lang/Object;");
                            if ( g_NewClsGlabel(pEnv, &g_clsLoadedApk, "android/app/LoadedApk") )
                            {
                                MYLOGI("ERROR: class LoadedApk");
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
                                    MYLOGI("ERROR: class ApplicationInfo");
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
                                        MYLOGI("ERROR: class Application");
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
                                            MYLOGI("ERROR: class ContextWrapper");
                                        }
                                        else
                                        {
                                            g_MethodIDattachBaseContext = pEnv->GetMethodID(
                                                    g_clsContextWrapper,
                                                    "attachBaseContext",
                                                    "(Landroid/content/Context;)V");
                                            MYLOGI("PathClassLoader start");
                                            if ( g_NewClsGlabel(pEnv, &g_clsPathClassLoader, "dalvik/system/PathClassLoader") )
                                            {
                                                MYLOGI("ERROR: class PathClassLoader");
                                            }
                                            else
                                            {
                                                if(g_nSDKINT > 13)
                                                {
                                                    if ( g_NewClsGlabel(pEnv, &g_clsBaseDexClassLoader, "dalvik/system/BaseDexClassLoader") )
                                                    {
                                                        MYLOGI("ERROR: BaseDexClassLoader");
                                                    }
                                                    g_FieldIDpathList = pEnv->GetFieldID(
                                                            g_clsBaseDexClassLoader,
                                                            "pathList",
                                                            "Ldalvik/system/DexPathList;");
                                                    if ( g_NewClsGlabel(pEnv, &g_clsDexPathList, "dalvik/system/DexPathList") )
                                                    {
                                                        MYLOGI("ERROR: class DexPathList");
                                                        return false;
                                                    }
                                                    g_FieldIDdexElements = pEnv->GetFieldID(
                                                            g_clsDexPathList,
                                                            "dexElements",
                                                            "[Ldalvik/system/DexPathList$Element;");
                                                    if ( g_NewClsGlabel(pEnv, &g_clsElement, "dalvik/system/DexPathList$Element") )
                                                    {
                                                        MYLOGI("ERROR: class Element");
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
                                                    MYLOGI("ERROR: class File");
                                                    return false;
                                                }
                                                else
                                                {
                                                    g_MethodIDgetAbsolutePath = pEnv->GetMethodID(
                                                            g_clsFile,
                                                            "getAbsolutePath",
                                                            "()Ljava/lang/String;");
                                                    MYLOGI("PathClassLoader end");
                                                    if ( g_NewClsGlabel(pEnv, &g_clsDexFile, "dalvik/system/DexFile") )
                                                    {
                                                        MYLOGI("ERROR: class DexFile");
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
                                                            MYLOGI("ERROR: class ClassLoader");
                                                            return false;
                                                        }
                                                        else
                                                        {
                                                            g_MethodIDfindClass = pEnv->GetMethodID(
                                                                    g_clsClassLoader,
                                                                    "findClass",
                                                                    "(Ljava/lang/String;)Ljava/lang/Class;");
                                                            MYLOGI("System start");
                                                            if(g_NewClsGlabel(pEnv, &g_clsSystem, "java/lang/System"))
                                                            {
                                                                MYLOGI("ERROR: class System");
                                                                return false;
                                                            }
                                                            else
                                                            {
                                                                g_StaticMethodIDgetProperty = pEnv->GetStaticMethodID(
                                                                        g_clsSystem,
                                                                        "getProperty",
                                                                        "(Ljava/lang/String;)Ljava/lang/String;");
                                                                MYLOGI("exit init classes normal");
                                                                jstring strVersion = pEnv->NewStringUTF("java.vm.version");
                                                                jstring objVersion = (jstring)pEnv->CallStaticObjectMethod(g_clsSystem, g_StaticMethodIDgetProperty, strVersion);
                                                                const char* pVersion = (const char*)pEnv->GetStringUTFChars(objVersion, 0);
                                                                int nSub = *pVersion - 49;
                                                                g_isArt = nSub > 0;
                                                                g_isDalvik = !(g_isArt == true);

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
    MYLOGI("JNI_OnLoad");
    JNIEnv *env = NULL;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    if (registerNativeMethods(env, gClassName, gMethods,
                              sizeof(gMethods) / sizeof(gMethods[0])) == JNI_FALSE) {
        return JNI_ERR;
    }
    MYLOGI("So load success");
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
MYLOGI("JNI_OnUnload");
return;
}
//data/data/packagename/files
char g_filePath[0xff];
/**
 * 释放文件
 */
int u_releaseFile(JNIEnv *env, jobject objApplication)
{
#define STAMPFILECONTEXT "Thu Sep 18 19:00:59 CST 2014"
    char timestamp[0xff];
    char clsdexPath[0xff];
    int isUpdata = 0;
    strcpy(timestamp, g_filePath);
    strcpy(clsdexPath, g_filePath);
    strcat(timestamp, "/.timestamp");
    //assess测试目标文件是否可以被访问，可以返回0
    if(!access(timestamp, 0))//如果timestamp文件可以访问
    {
        struct stat stat1;
        int  nOpenResult = open(timestamp, O_RDWR);
        //Get file information, given a file description
        if(!fstat(nOpenResult, &stat1)) //如果文件信息正常拿到
        {
            FILE* file = fdopen(nOpenResult, "w+");
            void *pNew = calloc((size_t)stat1.st_blksize, 1);
            if(pNew)
            {
                MYLOGI("calloc error: %d", errno);
                return false;
            }
            //读取文件内容并判断是否与预期一致
            fread(pNew, (size_t)stat1.st_blksize, 1, file);
            fclose(file);
            MYLOGI("buf:%s timestamp:%s", pNew, STAMPFILECONTEXT);
            //检查存储内容是否与预设一致
            int nCmpResult = strncmp((const char*)pNew, STAMPFILECONTEXT, strlen(STAMPFILECONTEXT));
            //不一致就到后面删除并重新写入预期的内容
            if(nCmpResult)
                isUpdata = 1;
            free(pNew);
        }
        //写入预期的内容作为防止重复释放的标志
        if(isUpdata)
        {
            MYLOGI("after update, remove files");
            strcat(clsdexPath, "/cls.dex");
            int nunlinkResult = unlink(clsdexPath); // unlink为文件删除命令,成功执行返回0
            if(nunlinkResult)
                MYLOGI("failed to rm files with error:%d", errno);
            FILE* file = fopen(timestamp, "w");
            fwrite(STAMPFILECONTEXT, strlen(STAMPFILECONTEXT), 1u, file);
            fclose(file);
        }
    }
    //获取Application类及其getAssets方法id
    jclass clsApplication = env->GetObjectClass(objApplication);
    jmethodID mIDgetAssets = env->GetMethodID(clsApplication, "getAssets", "()Landroid/content/res/AssetManager;");
    //通过getAssets方法ID获取到AssetManager对象
    jobject objAssetsManager = env->CallObjectMethod(objApplication, mIDgetAssets);
    //给定一个Dalvik AssetManager对象，绑定对应的本地AAssetManager*对象
    AAssetManager* AssetsManager = AAssetManager_fromJava(env, objAssetsManager);
    //准备释放三个文件
    const char* fileAry[] = {"fak.jar", "cls.jar", "juice.data"};
    for (int i = 0; i < sizeof(fileAry) / sizeof(fileAry[0]); ++i)
    {
        char curFilePath[0xff];
        char temp[1024];
        temp[0]  = '\0';
        //拼接文件路径
        sprintf(curFilePath, "%s/%s", g_filePath, fileAry[i]);
        if(isUpdata || access(curFilePath, F_OK))
        {
            //AAssetManager打开目标文件i准备读取
            AAsset * aa = AAssetManager_open(AssetsManager, fileAry[i], AASSET_MODE_STREAMING);// 调用AssetsManager.open获取InputStream
            MYLOGI("extract file %s", curFilePath);
            if(aa)
            {
                int nRead = 1;
                //写入模式打开存储位置
                FILE *file = fopen(curFilePath, "w");
                while(nRead > 0)
                {
                    nRead = AAsset_read(aa, temp, 1024);
                    fwrite(temp, nRead, 1, file);
                }
                fclose(file);
                AAsset_close(aa);
            }
            else
                MYLOGI("file not found");
        }
    }
    return 0;
}
int fun_attachBaseContext(JNIEnv *env, _jobject *objApplication, _jobject *context)
{
    MYLOGI("in...");
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
        //获取ApplicationInfo
        jobject objApplicationInfo = env->CallObjectMethod(objApplication, g_MethodIDgetApplicationInfo, JNI_FALSE);
        if(!objApplicationInfo)
        {
            return 0;
        }
        char NativeLibraryDir[0xff];
        NativeLibraryDir[0] = '\0';
        //如果SDK版本小于等于8，获取ApplicationInfo的dataDir域并拼接/lib为nativeLibraryDir目录
        if(g_nSDKINT <= 8)
        {
            jstring strDataDir = (jstring)env->GetObjectField(objApplicationInfo, g_FieldIDdataDir);
            const char* pDataDir = env->GetStringUTFChars(strDataDir, JNI_FALSE);
            if(!pDataDir)
                return 0;
            strncpy(NativeLibraryDir, pDataDir, strlen(pDataDir));
            strcat(NativeLibraryDir, "/lib");
        }
        //如果SDK版本大于8，获取ApplicationInfo的nativeLibraryDir域并拼接/lib为nativeLibraryDir目录
        else
        {
            jstring strnativeLibraryDir = (jstring)env->GetObjectField(objApplicationInfo, g_FieldIDnativeLibraryDir);
            const char* pDataDir = env->GetStringUTFChars(strnativeLibraryDir, JNI_FALSE);
            if(!pDataDir)
                return 0;
            //strncpy(NativeLibraryDir, pDataDir, strlen(pDataDir) + 1);
            strcpy(NativeLibraryDir, pDataDir);
            strcat(NativeLibraryDir, "/lib");
        }
        MYLOGI("global native path is %s", NativeLibraryDir);
        //调用application;->getPackageResourcePath获取资源目录路径
        jstring strPackageResourcePath = (jstring)env->CallObjectMethod(objApplication, g_MethodIDgetPackageResourcePath);
        //"/data/app/aqcxbom.rebuildproject25-1.apk"
        const char* pPackageResourcePath = env->GetStringUTFChars(strPackageResourcePath, JNI_FALSE);
        if(!pPackageResourcePath)
            return 0;
        //参数不为0：如果存在同名项则覆盖
        setenv("APKPATH", pPackageResourcePath, 1);
        MYLOGI( "global apk path is %s", pPackageResourcePath);
        //释放三个加密相关的文件
        u_releaseFile(env, objApplication);

        //context;->getPackageName 获取包名
        jstring strPackageName = (jstring)env->CallObjectMethod(context, g_MethodIDgetPackageName);
        const char* pPackageName = NULL;
        if(strPackageName)
        {
            pPackageName = env->GetStringUTFChars(strPackageName, JNI_FALSE);
        }
        if(!pPackageName)
            return 0;
        strcpy(g_PackageName, pPackageName);
        //从Context获取ClassLoader
        jobject objClassLoader = env->CallObjectMethod(context, g_MethodIDgetClassLoader);
        //解析dex
        //parse_dex 加载JAR包，返回DexOrJar的指针
        DexOrJar *pDexOrJar;
        parse_dex(env, &pDexOrJar);
        //replace_classloader_cookie 替换ClassLoader相关字段
        jobject classloader = env->CallObjectMethod(context, g_MethodIDgetClassLoader);
        replase_classloader_cookie(env, classloader, pDexOrJar);
        MYLOGI("enter new application");
        // new一个Application并attach到当前的参数context
        // 第一步调用ClassLoader的findClass方法获得android.app.Application类的class
        jstring string_androidappApplication = env->NewStringUTF("android.app.Application");
        //classLoader;->findClass("android.app.Application")
        jobject objNewApplication = NULL;
        jclass clsApplication2 = (jclass)env->CallObjectMethod(objClassLoader, g_MethodIDfindClass, string_androidappApplication);

        if(clsApplication2)
        {
            // 第二步调用Applicaton类的初始化方法
            jmethodID mIDinit = env->GetMethodID(clsApplication2, "<init>", "()V");
            objNewApplication = env->NewObject(clsApplication2, mIDinit);
            // 第三步完成attach操作
            env->CallVoidMethod(objNewApplication, g_MethodIDattach, context);
            env->DeleteLocalRef(clsApplication2);
            MYLOGI("exit new application");
        }
        else
        {
            MYLOGI("can't findClass realAppClass");
        }
        if(objNewApplication)
        {
            g_NewApplication = env->NewGlobalRef(objNewApplication);
        }
        MYLOGI("exit attachBaseContext");
    }
    return 0;
}

int fun_onCreate(JNIEnv *env, jobject objBundle)
{
    if (g_NewApplication)
    {
        MYLOGI("enter replace_app_application");
        //从ActivityThread类调用currentActivityThread方法获取currentActivityThread对象
        jobject objcurrentActivityThread = env->CallStaticObjectMethod(g_ClsActivityThread, g_MethodIDcurrentActivityThread, 0xFFFFFC4C);
        //ActivityThread;->mPackages 获取mPackage域成员对象
        jobject objmPackages = env->GetObjectField(objcurrentActivityThread, g_FieldIDmPackages);
        //构建包名
        jstring strPackageName = env->NewStringUTF(g_PackageName);
        jobject objWeakReference = NULL;
        //WeakReference wr = (WeakReference) mPackages.get(packageName);
        if(g_nSDKINT > 18)
        {
            objWeakReference = env->CallObjectMethod(objmPackages, g_MethodIDArrayMapget, strPackageName);
        }
        else
        {
            objWeakReference = env->CallObjectMethod(objmPackages, g_MethodIDHashMapget, strPackageName);
        }
        env->DeleteLocalRef(strPackageName);
        //WeakReference.get()获取LoadedApk对象
        jobject objLoadedApk = env->CallObjectMethod(objWeakReference, g_MethodIDWeakReferenceget);
        //设置LoadedApk中application字段为新建的Application
        env->SetObjectField(objLoadedApk, g_FieldIDmApplication, g_NewApplication);
        //设置ActivityThread中InitialApplication字段为新建的Application
        env->SetObjectField(objcurrentActivityThread, g_FieldIDmInitialApplication, g_NewApplication);
        //获取ActivityThread的mBoundApplication对象
        jobject objmBoundApplication = env->GetObjectField(objcurrentActivityThread, g_FieldIDmBoundApplication);
        //获取mBoundApplication对象的info域（LoadedApk结构）
        jobject objLoadedApk2 = env->GetObjectField(objmBoundApplication, g_FieldIDinfo);
        //设置其中application字段为新建的Application
        env->SetObjectField(objLoadedApk2, g_FieldIDmApplication, g_NewApplication);
        //获取ActivityThread的mAllApplications字段对象类型为ArrayList
        jobject objmAllApplications = env->GetObjectField(objcurrentActivityThread, g_FieldIDmAllApplications);
        int nSize = env->CallIntMethod(objmAllApplications, g_MethodIDArrayListsize);
        //依次遍历其中，查找与参数objBundle为同一对象的对象，将新的Application设置替换
        for (int i = 0; i < nSize; ++i)
        {
            jobject objApplicationN = env->CallObjectMethod(objmAllApplications, g_MethodIDArrayListget, i);
            MYLOGI("compare: i=%d item=%p", i, objApplicationN);
            jboolean bResult = env->IsSameObject(objBundle, objApplicationN);
            if(bResult)
            {
                MYLOGI("replace: find same replace");
                env->CallObjectMethod(objmAllApplications, g_MethodIDArrayListset, i, g_NewApplication);
            }
            env->DeleteLocalRef(objApplicationN);
        }
        env->DeleteLocalRef(objmAllApplications);
        env->DeleteLocalRef(objLoadedApk2);
        env->DeleteLocalRef(objmBoundApplication);
        env->DeleteLocalRef(objLoadedApk);
        env->DeleteLocalRef(objmPackages);
        env->DeleteLocalRef(objcurrentActivityThread);
        MYLOGI("exit replace_app_application");
        MYLOGI( "call onCreate start");
        env->CallVoidMethod(g_NewApplication, g_MethodIDonCreate);
        MYLOGI( "call onCreate end");
        env->DeleteGlobalRef(g_NewApplication);
        return 1;
    }
    else
        MYLOGI("no g_realApplication just free classes and assets");
    return 0;
}
// 替换对应结构Cookie结构，用pDexOrJar指针
int replase_classloader_cookie(JNIEnv *env, jobject classloader, DexOrJar* pDexOrJar)
{
    MYLOGI("enter replace_classloader_cookie");
    if(g_nSDKINT > 13)
    {
        // dalvik.system.DexClassLoader;
        // 获取BaseDexClassLoader类的DexPathList结构成员
        jobject pathList = env->GetObjectField(classloader, g_FieldIDpathList);
        // DexPathList结构中的Element[] dexElements数组对象
        jobjectArray eleAry = (jobjectArray )env->GetObjectField(pathList, g_FieldIDdexElements);
        int nEleLen = env->GetArrayLength(eleAry);
        MYLOGI("element count: %u", nEleLen);
        // 从Element数组中遍历
        for(int i = 0; i < nEleLen; i++)
        {
            jobject elei = env->GetObjectArrayElement(eleAry, i);
            jobject objFile = env->GetObjectField(elei, g_FieldIDfile);
            // 获取file字段的绝对路径
            jstring absPath = (jstring)env->CallObjectMethod(objFile, g_MethodIDgetAbsolutePath);
            const char* pAbsPath = env->GetStringUTFChars(absPath, JNI_FALSE);
            MYLOGI("element is %s", pAbsPath);
            int nAbsLen = env->GetStringUTFLength(absPath);
            // 检索是否与apk结尾
            int nCmpResult = strncasecmp("apk", &pAbsPath[nAbsLen - 3], 3);
            if(!nCmpResult) // apk结尾就表示找到目标
            {
                jobject objDexFile = env->GetObjectField(elei, g_FieldIDdexfile);
                /**
                 * 适用 5.x
                 * 6.x 及以上都是 Object 即API>22
                 **/
                if(g_nSDKINT > 19)
                    env->SetLongField(objDexFile, g_FieldIDmCookie, (long)pDexOrJar);
                else // API 在14-19 的4.x 没错
                    env->SetIntField(objDexFile, g_FieldIDmCookie, *((int*)&pDexOrJar));
                env->DeleteLocalRef(objDexFile);
                env->DeleteLocalRef(elei);
                env->DeleteLocalRef(eleAry);
                env->DeleteLocalRef(pathList);
                break;
            }
        }
    }
    else
    {
        // dalvik.system.DexClassLoader;
        // API <= 13 ==> DexClassLoader, mDexs字段是DexFile数组
        jobjectArray dexAry = (jobjectArray)env->GetObjectField(classloader, g_FieldIDmDexs);
        // 获取第0个元素,即是个DexFile结构
        jobject ele = env->GetObjectArrayElement(dexAry, 0);
        // 获取mCookie值，其是openDexFile函数的返回值，
        int cookie = env->GetIntField(ele, g_FieldIDmCookie);
        // 替换DexOrJar结构的pJarFile指针
        //struct DexOrJar {
        //    char*       fileName; +0
        //    bool        isDex; + 4
        //    bool        okayToFree; +5
        //    RawDexFile* pRawDexFile; +8
        //    JarFile*    pJarFile; + c
        //    u1*         pDexMemory; // malloc()ed memory, if any
        //};
        *(int*)(cookie + 12) = *(int*)(pDexOrJar + 12);
        env->DeleteLocalRef(ele);
        env->DeleteLocalRef(dexAry);
    }
    MYLOGI("exit replace_classloader_cookie");
    return 0;
}

typedef struct st_enc{
    int m_blksize;
    void* m_mmapResult;
    const char* m_pFilePath;
} ST_ENC;

// 初始化Enc结构
ST_ENC* EncInit(ST_ENC *pEnc, const char* path)
{
    pEnc->m_blksize = 0;
    pEnc->m_mmapResult = 0;
    if(path)
        pEnc->m_pFilePath = strndup(path, 1024);
    return pEnc;
}

const char* g_constKey = "YRq&rxh6Nsbh^W1nI5RfZzJZ";
int crcValue = 0;
int crcXorKey(char *pOutKey)
{
    strcpy(pOutKey, g_constKey);
    if(crcValue == 0)
    {
        char* pathApk = getenv("APKPATH");
        MYLOGI("apkPah:%s", pathApk);
    }
    return 1;
}

void decryptRc4()
{
}

// 映射文件到内存
void* openWithHeader(ST_ENC *pST_ENC, int *pOutMemAddr, int *pOutFileSize, int nRel)
{
    if(!pST_ENC->m_pFilePath)
    {
        MYLOGI("file path is null");
        return 0;
    }
    int openResult = open(pST_ENC->m_pFilePath, O_RDONLY);
    struct stat stat1;
    int nfstat = fstat(openResult, &stat1);
    if(nfstat)
    {
        MYLOGI("fstat failed");
        return 0;
    }
    // 这两个有疑问
    pST_ENC->m_blksize = stat1.st_size - 16;
    void* mmapResult = mmap(NULL , stat1.st_size + 16, PROT_WRITE|PROT_READ, MAP_PRIVATE, openResult, 0);// 将文件映射到内存
    if(mmapResult == (void*)-1){
        MYLOGI("mmap dex file :%s", strerror(errno));
        MYLOGI("exit parse_dex error");
        return NULL;
    }
    pST_ENC->m_mmapResult = mmapResult;
    close(openResult);
    MYLOGI("dex magic %c %c %c %c %c %c %c",
           ((char*)mmapResult+4)[0],
           ((char*)mmapResult+4)[1],
           ((char*)mmapResult+4)[2],
           ((char*)mmapResult+4)[3],
           ((char*)mmapResult+4)[4],
           ((char*)mmapResult+4)[5],
           ((char*)mmapResult+4)[6]);

    *pOutFileSize = pST_ENC->m_blksize;
    *pOutMemAddr = *((int*)&pST_ENC->m_mmapResult);
    return pST_ENC->m_mmapResult;
}

bool lookup(JNINativeMethod *method, const char *pFuncName, const char *pFuncSig, FuncOpenDex *ppOutFuncAddr)
{
    for(JNINativeMethod* i = method; i->name; i++)
    {
        if(!strcmp(pFuncName, i->name) && !strcmp(pFuncSig, i->signature))
        {
            *ppOutFuncAddr = (FuncOpenDex)i->fnPtr;
            return true;
        }
    }
    return false;
}

int parse_dex(JNIEnv *env, DexOrJar **pOutDexOrJar)
{
    MYLOGI("enter parse_dex");
    if(g_isDalvik)
    {
        char clsPath[0xff];
        strcpy(clsPath, g_filePath);
        strcat(clsPath, "/cls.jar");
        ST_ENC *pEnc = new ST_ENC();
        EncInit(pEnc, clsPath);
        ArrayObject* aObj;
        int dexFileMemAddr;
        if(g_nSDKINT > 13)
        {
            int OutMemAddr;
            int OutFileSize;
            // mmap该文件到内存中，最后一个参数完全没用上
            aObj = (ArrayObject*)openWithHeader(pEnc, &OutMemAddr, &OutFileSize, 16);
            if(aObj == NULL)
            {
                MYLOGI("mmap dex file :%s", strerror(errno));
                MYLOGI("exit parse_dex error");
                return -1;
            }
            // 加载的这个文件其实就是伪造的加载了dex文件内容的ArrayObject结构,dex文件内容就在+16位置
            dexFileMemAddr = (int)&aObj->contents;
            //dexFileMemAddr = *((int*)&mmapAddr) + 16;

            u4 fileSize = *(u4 *)(dexFileMemAddr + 0x20); // dex偏移0x20为文件大小

            aObj->length = fileSize;
            //*(int*)(*((int*)&mmapAddr) + 8) = fileSize;  // 覆写map地址+8位置内容

            // 动态调用libdvm库的openDexFile加载内存中的JAR包
            void* dvmHandle = dlopen("libdvm.so", RTLD_LAZY);
            JNINativeMethod* dexfile = (JNINativeMethod*)dlsym(dvmHandle, "dvm_dalvik_system_DexFile");

            FuncOpenDex pfuncopenDexFile;
            lookup(dexfile, "openDexFile", "([B)I", &pfuncopenDexFile);
            *((int *)OutMemAddr + 2) = OutFileSize;
            DexOrJar *pDexOrJar = NULL;
            // openDexFile参数一为ArrayObject指针，参数二为DexOrJar指针输出
            pfuncopenDexFile((unsigned int *)&OutMemAddr, (jvalue*)&pDexOrJar);
            if(pDexOrJar == NULL)
            {
                MYLOGI("openDexFile dex file :%s", strerror(errno));
                return -1;
            }
            pDexOrJar->pRawDexFile->pDvmDex->memMap.addr = pDexOrJar->pDexMemory;
            pDexOrJar->pRawDexFile->pDvmDex->memMap.length = fileSize;
            delete pEnc;
            *pOutDexOrJar = pDexOrJar;
            MYLOGI("exit parse_dex");
        }
    }
    return 1;
}