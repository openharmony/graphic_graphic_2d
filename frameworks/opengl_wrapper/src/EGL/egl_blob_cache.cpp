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
#include "egl_blob_cache.h"
#include "../wrapper_log.h"

namespace OHOS {
BlobCache* BlobCache::blobCache_;

BlobCache* BlobCache::Get()
{
    if (BlobCache::blobCache_ == nullptr) {
        BlobCache::blobCache_ = new BlobCache();
    }
    return BlobCache::blobCache_;
}

BlobCache::Blob::Blob(const void *dataIn, size_t size) :dataSize(size)
{
    prev_ = nullptr;
    next_ = nullptr;
    data = nullptr;
    if (size > 0) {
        data = malloc(size);
    }
    if (data != nullptr) {
        memcpy_s(data, size, const_cast<void *>(dataIn), size);
    }
}

BlobCache::Blob::~Blob()
{
    free(data);
}

BlobCache::BlobCache()
{
    head_ = std::make_shared<Blob>(nullptr, 0);
    tail_ = std::make_shared<Blob>(nullptr, 0);
    head_->next_ = tail_;
    tail_->prev_ = head_;
    blobSizeMax_ = MAX_SHADER;
    blobSize_ = 0;
}

BlobCache::~BlobCache()
{
}

void BlobCache::Init()
{
}

void BlobCache::setBlobFunc(const void* key, EGLsizeiANDROID keySize, const void* value,
                            EGLsizeiANDROID valueSize)
{
    BlobCache::Get()->setBlob(key, keySize, value, valueSize);
}
EGLsizeiANDROID BlobCache::getBlobFunc(const void *key, EGLsizeiANDROID keySize, void *value,
                                       EGLsizeiANDROID valueSize)
{
    return BlobCache::Get()->getBlob(key, keySize, value, valueSize);
}

void BlobCache::setBlob(const void *key, EGLsizeiANDROID keySize, const void *value,
                        EGLsizeiANDROID valueSize)
{
    if (keySize <= 0) {
        return;
    }
    std::shared_ptr<Blob> keyBlob = std::make_shared<Blob>(key, (size_t)keySize);
    auto it = mBlobMap_.find(keyBlob);
    if (it != mBlobMap_.end()) {
        free(it->second->data);
        if (valueSize < 0) {
            WLOGE("valueSize error");
            return;
        }
        it->second->data = malloc(valueSize);
        memcpy_s(it->second->data, valueSize, value, valueSize);
        head_->next_->prev_ = it->first;
        it->first->next_ = head_->next_;
        head_->next_ = it->first;
        it->first->prev_ = head_;
        return;
    }

    if (blobSize_ >= blobSizeMax_) {
        int count = 0;
        while (count <= MAX_SHADER_DELETE) {
            std::shared_ptr<Blob> deleteblob = tail_->prev_;
            deleteblob->prev_->next_ = tail_;
            tail_->prev_ = deleteblob->prev_;
            mBlobMap_.erase(deleteblob);
            --blobSize_;
            ++count;
        }
    }

    std::shared_ptr<Blob> valueBlob = std::make_shared<Blob>(value, (size_t)valueSize);
    mBlobMap_.emplace(keyBlob, valueBlob);
    head_->next_->prev_ = keyBlob;
    keyBlob->next_ = head_->next_;
    head_->next_ = keyBlob;
    keyBlob->prev_ = head_;
    ++blobSize_;
}

EGLsizeiANDROID BlobCache::getBlob(const void *key, EGLsizeiANDROID keySize, void *value,
                                   EGLsizeiANDROID valueSize)
{
    EGLsizeiANDROID ret = 0;
    if (keySize <= 0) {
        return ret;
    }
    std::shared_ptr<Blob> keyBlob = std::make_shared<Blob>(key, (size_t)keySize);
    auto it = mBlobMap_.find(keyBlob);
    if (it != mBlobMap_.end()) {
        ret = (EGLsizeiANDROID)it->second->dataSize;
        if (valueSize < ret) {
            WLOGE("valueSize not enough");
        } else {
            memcpy_s(value, valueSize, it->second->data, it->second->dataSize);
            auto moveblob = it->first;
            moveblob->prev_->next_ = moveblob->next_;
            moveblob->next_->prev_ = moveblob->prev_;
            head_->next_->prev_ = moveblob;
            moveblob->next_ = head_->next_;
            head_->next_ = moveblob;
            moveblob->prev_ = head_;
        }
    }

    return ret;
}

void BlobCache::SetCacheDir(const std::string dir)
{
    cacheDir_ = dir;
}

void EglSetCacheDir(const std::string dir)
{
    BlobCache::Get()->SetCacheDir(dir);
}
}