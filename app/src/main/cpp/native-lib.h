

#include "Art.h"
#ifndef REBUILDPROJECT25_NATIVE_H
#define REBUILDPROJECT25_NATIVE_H


typedef uint8_t             u1;
typedef uint16_t             u2;
typedef uint32_t             u4;
typedef uint64_t             u8;


struct MemMapping {
    void*   addr;           /* start of data */
    size_t  length;         /* length of data */

    void*   baseAddr;       /* page-aligned base address */
    size_t  baseLength;     /* length of mapping */
};


struct DvmDex {
    /* pointer to the DexFile we're associated with */
    //DexFile*            pDexFile;
    int*            pDexFile;

    /* clone of pDexFile->pHeader (it's used frequently enough) */
    //const DexHeader*    pHeader;
    int*    pHeader;

    /* interned strings; parallel to "stringIds" */
    struct StringObject** pResStrings;

    /* resolved classes; parallel to "typeIds" */
    struct ClassObject** pResClasses;

    /* resolved methods; parallel to "methodIds" */
    struct Method**     pResMethods;

    /* resolved instance fields; parallel to "fieldIds" */
    /* (this holds both InstField and StaticField) */
    struct Field**      pResFields;

    /* interface method lookup cache */
    struct AtomicCache* pInterfaceCache;

    /* shared memory region with file contents */
    bool                isMappedReadOnly;
    MemMapping          memMap;

    jobject dex_object;

    /* lock ensuring mutual exclusion during updates */
    //pthread_mutex_t     modLock;
    int     modLock;
};

struct RawDexFile {
    char*       cacheFileName;
    DvmDex*     pDvmDex;
};

struct ZipArchive {
    /* open Zip archive */
    int         mFd;

    /* mapped central directory area */
    off_t       mDirectoryOffset;
    MemMapping  mDirectoryMap;

    /* number of entries in the Zip archive */
    int         mNumEntries;

    /*
     * We know how many entries are in the Zip archive, so we can have a
     * fixed-size hash table.  We probe on collisions.
     */
    int         mHashTableSize;
    //ZipHashEntry* mHashTable;
    int* mHashTable;
};

struct JarFile {
    ZipArchive  archive;
    //MemMapping  map;
    char*       cacheFileName;
    DvmDex*     pDvmDex;
};

struct DexOrJar {
    char*       fileName;
    bool        isDex;
    bool        okayToFree;
    RawDexFile* pRawDexFile;
    JarFile*    pJarFile;
    u1*         pDexMemory; // malloc()ed memory, if any
};

struct Object {
    //ClassObject*    clazz;
    void*    clazz;
    u4              lock;
};

struct ArrayObject : Object {
    u4              length;
    u8              contents[1];
};
#endif
