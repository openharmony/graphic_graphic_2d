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
#include "wrapper_log.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <errors.h>
#include <cinttypes>
#include <unistd.h>
#include <fcntl.h>
#include <thread>

namespace OHOS {
static const char* CACHE_MAGIC = "OSOH";
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
    if (data == nullptr) {
        dataSize = 0;
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
    maxShaderSize_ = MAX_SHADER_SIZE;
    initStatus_ = false;
    fileName_ = std::string("/blobShader");
    saveStatus_ = false;
    blobSize_ = 0;
}

BlobCache::~BlobCache()
{
    if (blobCache_) {
        blobCache_ = nullptr;
    }
}

void BlobCache::Terminate()
{
    if (blobCache_) {
        blobCache_->WriteToDisk();
    }
    blobCache_ = nullptr;
}

int BlobCache::GetMapSize() const
{
    if (blobCache_) {
        return (int)mBlobMap_.size();
    }
    return 0;
}

void BlobCache::Init(EglWrapperDisplay* display)
{
    if (!initStatus_) {
        return;
    }
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    if (table->isLoad && table->egl.eglSetBlobCacheFuncsANDROID) {
        table->egl.eglSetBlobCacheFuncsANDROID(display->GetEglDisplay(),
                                               BlobCache::SetBlobFunc, BlobCache::GetBlobFunc);
        ReadFromDisk();
    } else {
        WLOGE("eglSetBlobCacheFuncsANDROID not found.");
    }
}

void BlobCache::SetBlobFunc(const void* key, EGLsizeiANDROID keySize, const void* value,
                            EGLsizeiANDROID valueSize)
{
    BlobCache::Get()->SetBlobLock(key, keySize, value, valueSize);
}
EGLsizeiANDROID BlobCache::GetBlobFunc(const void *key, EGLsizeiANDROID keySize, void *value,
                                       EGLsizeiANDROID valueSize)
{
    return BlobCache::Get()->GetBlobLock(key, keySize, value, valueSize);
}

void BlobCache::SetBlobLock(const void* key, EGLsizeiANDROID keySize, const void* value,
                            EGLsizeiANDROID valueSize)
{
    std::lock_guard<std::mutex> lock(blobmutex_);
    SetBlob(key, keySize, value, valueSize);
}
EGLsizeiANDROID BlobCache::GetBlobLock(const void *key, EGLsizeiANDROID keySize, void *value,
                                       EGLsizeiANDROID valueSize)
{
    std::lock_guard<std::mutex> lock(blobmutex_);
    return GetBlob(key, keySize, value, valueSize);
}

void BlobCache::MoveToFront(std::shared_ptr<Blob>& cur)
{
    head_->next_->prev_ = cur;
    cur->next_ = head_->next_;
    head_->next_ = cur;
    cur->prev_ = head_;
}

void BlobCache::SetBlob(const void *key, EGLsizeiANDROID keySize, const void *value,
                        EGLsizeiANDROID valueSize)
{
    if (keySize <= 0 || valueSize <= 0 || key == nullptr || value == nullptr) {
        return;
    }
    if (!readStatus_) {
        ReadFromDisk();
    }
    std::shared_ptr<Blob> keyBlob = std::make_shared<Blob>(key, (size_t)keySize);
    auto it = mBlobMap_.find(keyBlob);
    if (it != mBlobMap_.end()) {
        free(it->second->data);
        it->second->data = malloc(valueSize);
        if (it->second->data != nullptr) {
            it->second->dataSize = valueSize;
        } else {
            it->second->dataSize = 0;
            return;
        }
        memcpy_s(it->second->data, valueSize, value, valueSize);
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
    MoveToFront(keyBlob);
    ++blobSize_;
    if (!saveStatus_) {
        saveStatus_ = true;
        std::thread deferSaveThread([this]() {
            sleep(DEFER_SAVE_MIN);
            if (blobCache_) {
                blobCache_->WriteToDisk();
            }
            saveStatus_ = false;
        });
        deferSaveThread.detach();
    }
}

EGLsizeiANDROID BlobCache::GetBlob(const void *key, EGLsizeiANDROID keySize, void *value,
                                   EGLsizeiANDROID valueSize)
{
    EGLsizeiANDROID ret = 0;
    if (keySize <= 0 || key == nullptr) {
        return ret;
    }
    if (!readStatus_) {
        ReadFromDisk();
    }
    std::shared_ptr<Blob> keyBlob = std::make_shared<Blob>(key, (size_t)keySize);
    auto it = mBlobMap_.find(keyBlob);
    if (it != mBlobMap_.end()) {
        ret = static_cast<EGLsizeiANDROID>(it->second->dataSize);
        if (valueSize < ret) {
            WLOGE("valueSize not enough");
        } else if (ret == 0) {
            WLOGE("shader not exist");
        } else {
            errno_t ret = memcpy_s(value, valueSize, it->second->data, it->second->dataSize);
            if (ret != EOK) {
                WLOGE("memcpy_s failed");
                return ret;
            }
            auto moveblob = it->first;
            moveblob->prev_->next_ = moveblob->next_;
            moveblob->next_->prev_ = moveblob->prev_;
            MoveToFront(moveblob);
        }
    }

    return ret;
}

void BlobCache::SetCacheDir(const std::string dir)
{
    struct stat dirStat;
    if (stat(dir.c_str(), &dirStat) != 0) {
        WLOGE("inputdir not Create");
        initStatus_ = false;
        return;
    }

    struct stat cachedirstat;
    struct stat ddkdirstat;
    cacheDir_ = dir + std::string("/blobShader");
    ddkCacheDir_ = dir + std::string("/ddkShader");

    if (stat(cacheDir_.c_str(), &cachedirstat) != 0) {
        initStatus_ = false;
        int ret = mkdir(cacheDir_.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);
        if (ret == -1) {
            initStatus_ = false;
            WLOGE("cacheDir_ not Create");
        } else {
            initStatus_ = true;
        }
    } else {
        initStatus_ = true;
    }

    if (stat(ddkCacheDir_.c_str(), &ddkdirstat) != 0) {
        int ret = mkdir(ddkCacheDir_.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);
        if (ret == -1 && errno != EEXIST) {
            WLOGE("ddkCacheDir_ not Create");
        }
    }
}

void BlobCache::SetCacheShaderSize(int shadermax)
{
    if (shadermax <= 0 || shadermax > MAX_SHADER) {
        return;
    }
    blobSizeMax_ = shadermax;
}

static inline size_t Formatfile(size_t size)
{
    return (size + FORMAT_OFFSET) & ~FORMAT_OFFSET;
}

size_t BlobCache::GetCacheSize() const
{
    size_t headSize = sizeof(CacheHeader);
    size_t ret = 0;
    for (auto item = mBlobMap_.begin(); item != mBlobMap_.end(); ++item) {
        size_t innerSize = headSize + item->first->dataSize + item->second->dataSize;
        ret += Formatfile(innerSize);
    }
    return ret;
}

void BlobCache::WriteToDisk()
{
    std::lock_guard<std::mutex> lock(blobmutex_);
    struct stat dirStat;
    if (stat(cacheDir_.c_str(), &dirStat) != 0) {
        return;
    }
    std::string storefile = cacheDir_ + fileName_;
    int fd = open(storefile.c_str(), O_CREAT | O_EXCL | O_RDWR, 0);
    if (fd == -1) {
        if (errno == EEXIST) {
            if (unlink(storefile.c_str()) == -1) {
                return;
            }
            fd = open(storefile.c_str(), O_CREAT | O_EXCL | O_RDWR, 0);
        }
        if (fd == -1) {
            return;
        }
    }
    size_t filesize = GetCacheSize();
    size_t headsize = sizeof(CacheHeader);
    size_t bufsize = filesize + CACHE_HEAD;
    uint8_t *buf = new uint8_t[bufsize];
    size_t offset = CACHE_HEAD;
    for (auto item = mBlobMap_.begin(); item != mBlobMap_.end(); ++item) {
        CacheHeader* eheader = reinterpret_cast<CacheHeader*>(&buf[offset]);
        size_t keysize = item->first->dataSize;
        size_t valuesize = item->second->dataSize;
        eheader->keySize = keysize;
        eheader->valueSize = valuesize;
        memcpy_s(eheader->mData, keysize, item->first->data, keysize);
        memcpy_s(eheader->mData + keysize, valuesize, item->second->data, valuesize);
        size_t innerSize = headsize + keysize + valuesize;
        offset += Formatfile(innerSize);
    }
    if (memcpy_s(buf, bufsize, CACHE_MAGIC, CACHE_MAGIC_HEAD) != 0) {
        delete[] buf;
        close(fd);
        return;
    }
    uint32_t *crc = reinterpret_cast<uint32_t*>(buf + CACHE_MAGIC_HEAD);
    *crc = CrcGen(buf + CACHE_HEAD, filesize);
    if (write(fd, buf, bufsize) != -1) {
        fchmod(fd, S_IRUSR);
    }
    delete[] buf;
    close(fd);
}

void BlobCache::ReadFromDisk()
{
    readStatus_ = true;
    std::string storefile = cacheDir_ + fileName_;
    int fd = open(storefile.c_str(), O_RDONLY, 0);
    if (fd == -1) {
        close(fd);
        return;
    }
    struct stat bufstat;
    if (fstat(fd, &bufstat) == -1) {
        close(fd);
        return;
    }
    size_t filesize = bufstat.st_size;
    if (filesize > maxShaderSize_ + maxShaderSize_ || filesize <= 0) {
        close(fd);
        return;
    }
    uint8_t *buf = reinterpret_cast<uint8_t*>(mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0));
    if (buf == MAP_FAILED) {
        close(fd);
        return;
    }
    if (!ValidFile(buf, filesize)) {
        munmap(buf, filesize);
        close(fd);
        return;
    }
    size_t headsize = sizeof(CacheHeader);
    size_t byteoffset = CACHE_HEAD;
    while (byteoffset < filesize - CACHE_HEAD) {
        const CacheHeader* eheader = reinterpret_cast<CacheHeader*>(&buf[byteoffset]);
        size_t keysize = eheader->keySize;
        size_t valuesize = eheader->valueSize;
        if (byteoffset + headsize + keysize > filesize) {
            break;
        }
        const uint8_t *key = eheader->mData;
        if (byteoffset + headsize + keysize + valuesize > filesize) {
            break;
        }
        const uint8_t *value = eheader->mData + keysize;
        SetBlob(key, static_cast<EGLsizeiANDROID>(keysize), value, static_cast<EGLsizeiANDROID>(valuesize));
        size_t innerSize = headsize + keysize + valuesize;
        byteoffset += Formatfile(innerSize);
    }
    munmap(buf, filesize);
    close(fd);
}

//CRC standard function
uint32_t BlobCache::CrcGen(const uint8_t *buf, size_t len)
{
    const uint32_t polynoimal = 0xEDB88320;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < len ; ++i) {
        crc ^= (static_cast<uint32_t>(buf[i]) << CRC_NUM);
        for (size_t j = 0; j < BYTE_SIZE; ++j) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ polynoimal;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool BlobCache::ValidFile(uint8_t *buf, size_t len)
{
    if (memcmp(buf, CACHE_MAGIC, CACHE_MAGIC_HEAD) != 0) {
        WLOGE("CACHE_MAGIC failed");
        return false;
    }

    uint32_t* crcfile = reinterpret_cast<uint32_t*>(buf + CACHE_MAGIC_HEAD);
    uint32_t crccur = CrcGen(buf + CACHE_HEAD, len - CACHE_HEAD);
    if (crccur != *crcfile) {
        WLOGE("crc check failed");
        return false;
    }

    return true;
}

void EglSetCacheDir(const std::string dir)
{
    BlobCache::Get()->SetCacheDir(dir);
}
}