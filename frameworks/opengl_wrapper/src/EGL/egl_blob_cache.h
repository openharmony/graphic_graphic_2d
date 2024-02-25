/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_BLOB_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_BLOB_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <securec.h>
#include <unordered_map>
#include <functional>
#include "include/private/EGL/cache.h"

namespace OHOS {
const int MAX_SHADER = 600;
const int MAX_SHADER_DELETE = 150;
const int HASH_NUM = 31;

class BlobCache {
public:
    struct Blob {
        Blob(const void *dataIn, size_t size);
        ~Blob();
        std::shared_ptr<Blob> prev_;
        std::shared_ptr<Blob> next_;
        void *data;
        size_t dataSize;
    };

    //BLobByteHash is the basic hash algorithm to caculate shader.
    struct BlobByteHash {
        size_t operator()(std::shared_ptr<Blob> ptr) const
        {
            const unsigned char *bytes = static_cast<const unsigned char *>(ptr->data);
            size_t hash = 0;
            for (size_t i = 0; i < ptr->dataSize; ++i) {
                hash = hash * HASH_NUM + bytes[i];
            }
            return hash;
        }
    };

    //use memcmp to avoid hash collisions
    struct BlobByteEqual {
        bool operator()(std::shared_ptr<Blob> lhs, std::shared_ptr<Blob> rhs) const
        {
            if (lhs->dataSize == rhs->dataSize) {
                return memcmp(lhs->data, rhs->data, rhs->dataSize) == 0;
            } else {
                return false;
            }
        }
    };
    
    BlobCache();
    ~BlobCache();

    //Singoton achievement
    static BlobCache* Get();

    //used by ddk
    static void setBlobFunc(const void* key, EGLsizeiANDROID keySize, const void* value,
                            EGLsizeiANDROID valueSize);

    //used by ddk
    static EGLsizeiANDROID getBlobFunc(const void *key, EGLsizeiANDROID keySize, void *value,
                            EGLsizeiANDROID valueSize);

    //inner set func
    void setBlob(const void *key, EGLsizeiANDROID keySize, const void *value,
                EGLsizeiANDROID valueSize);

    //inner get func
    EGLsizeiANDROID getBlob(const void *key, EGLsizeiANDROID keySize, void *value,
                            EGLsizeiANDROID valueSize);
    
    void Init();

    //get cache dir from upper layer
    void SetCacheDir(const std::string dir);
private:
    static BlobCache *blobCache_;
    std::unordered_map<std::shared_ptr<Blob>, std::shared_ptr<Blob>, BlobByteHash, BlobByteEqual> mBlobMap_;
    int blobSize_;
    int blobSizeMax_;
    std::shared_ptr<Blob> head_;
    std::shared_ptr<Blob> tail_;
    std::string cacheDir_;
};

}

#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_BLOB_H
