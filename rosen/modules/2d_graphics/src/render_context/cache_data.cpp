/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "cache_data.h"
#include <cerrno>
#include <chrono>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <securec.h>
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
CacheData::CacheData (const size_t maxKeySize, const size_t maxValueSize,
    const size_t maxTotalSize, const std::string& fileName)
    : maxKeySize_(maxKeySize),
    maxValueSize_(maxValueSize),
    maxTotalSize_(maxTotalSize),
    cacheDir_(fileName) {}

void CacheData::ReadFromFile()
{
    if (cacheDir_.length() <= 0) {
        LOGE("abandon, because of empty filename.");
    }

    int fd = open(cacheDir_.c_str(), O_RDONLY, 0);
    if (fd == ERR_NUMBER) {
        if (errno != ENOENT) {
            LOGE("abandon, because fail to open file");
        }
        return ;
    }
    struct stat statBuf;
    if (fstat(fd, &statBuf) == ERR_NUMBER) {
        LOGE("abandon, because fail to get the file status");
        close(fd);
        return ;
    }
    size_t fileSize = statBuf.st_size;
    if (fileSize > maxTotalSize_ * CLEAN_TO) {
        LOGE("abandon, because cache file is too large");
        close(fd);
        return ;
    }
    void* buffer = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED) {
        LOGE("abandon, because of mmap failure");
        close(fd);
        return ;
    }
    uint8_t* shaderBuffer = reinterpret_cast<uint8_t*>(buffer);
    int err = DeSerialize(shaderBuffer, fileSize);
    if (err < 0) {
        LOGE("abandon, because fail to read file contents");
    }
    munmap(buffer, fileSize);
    close(fd);
}

void CacheData::WriteToFile()
{
    if (cacheDir_.length() <= 0) {
        LOGE("abandon, because of empty filename.");
        return;
    }
    
    size_t cacheSize = SerializedSize();
    int fd = open(cacheDir_.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == ERR_NUMBER) {
        if (errno == EEXIST) {
            if (unlink(cacheDir_.c_str()) == ERR_NUMBER) {
                LOGE("abandon, because unlinking the existing file fails");
                return;
            }
            fd = open(cacheDir_.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
        }
        if (fd == ERR_NUMBER) {
            LOGE("abandon, because the file creation fails");
            return;
        }
    }
    uint8_t* buffer = new uint8_t[cacheSize];
    if (!buffer) {
        LOGE("abandon, because fail to allocate buffer for cache content");
        close(fd);
        unlink(cacheDir_.c_str());
        return;
    }
    int err = Serialize(buffer, cacheSize);
    if (err < 0) {
        LOGE("abandon, because fail to serialize the CacheData");
        delete[] buffer;
        close(fd);
        unlink(cacheDir_.c_str());
        return;
    }

    if (write(fd, buffer, cacheSize) == ERR_NUMBER) {
        LOGE("abandon, because fail to write to disk");
        delete[] buffer;
        close(fd);
        unlink(cacheDir_.c_str());
        return;
    }

    delete[] buffer;
    fchmod(fd, S_IRUSR);
    close(fd);
}

void CacheData::Rewrite(const void *key, const size_t keySize, const void *value, const size_t valueSize)
{
    if (maxKeySize_ < keySize || maxValueSize_ < valueSize || maxTotalSize_ < keySize + valueSize
        || keySize == 0 || valueSize <= 0) {
        LOGE("abandon, because of illegal content size");
        return;
    }

    std::shared_ptr<DataPointer> fakeDataPointer(std::make_shared<DataPointer>(key, keySize, false));
    ShaderPointer fakeShaderPointer(fakeDataPointer, nullptr);
    
    auto index = std::lower_bound(shaderPointers_.begin(), shaderPointers_.end(), fakeShaderPointer);
    if (index == shaderPointers_.end() || fakeShaderPointer < *index) {
        std::shared_ptr<DataPointer> keyPointer(std::make_shared<DataPointer>(key, keySize, true));
        std::shared_ptr<DataPointer> valuePointer(std::make_shared<DataPointer>(value, valueSize, true));
        size_t newTotalSize = totalSize_ + keySize + valueSize;
        CheckClean(newTotalSize);
        shaderPointers_.insert(index, ShaderPointer(keyPointer, valuePointer));
        totalSize_ = newTotalSize;
    } else {
        std::shared_ptr<DataPointer> valuePointer(std::make_shared<DataPointer>(value, valueSize, true));
        std::shared_ptr<DataPointer> oldValuePointer(index->GetValuePointer());
        size_t newTotalSize = totalSize_ + valueSize - oldValuePointer->GetSize();
        CheckClean(newTotalSize);
        index->SetValue(valuePointer);
        totalSize_ = newTotalSize;
    }
}

size_t CacheData::Get(const void *key, const size_t keySize, void *value, const size_t valueSize)
{
    if (maxKeySize_ < keySize) {
        LOGE("abandon, because the key is too large");
        return -EINVAL;
    }
    std::shared_ptr<DataPointer> fakeDataPointer(std::make_shared<DataPointer>(key, keySize, false));
    ShaderPointer fakeShaderPointer(fakeDataPointer, nullptr);
    auto index = std::lower_bound(shaderPointers_.begin(), shaderPointers_.end(), fakeShaderPointer);
    if (index == shaderPointers_.end() || fakeShaderPointer < *index) {
        LOGE("abandon, because no key is found");
        return -EINVAL;
    }
    std::shared_ptr <DataPointer> valuePointer(index->GetValuePointer());
    size_t valuePointerSize = valuePointer->GetSize();
    if (valuePointerSize > valueSize) {
        LOGE("abandon, because of insufficient buffer space");
        return -EINVAL;
    }
    if (!memcpy_s(value, valueSize, valuePointer->GetData(), valuePointerSize)) {
        LOGE("abandon, failed to copy content");
    }
    return valuePointerSize;
}

size_t CacheData::SerializedSize() const
{
    size_t size = Align4(sizeof(Header));
    for (const ShaderPointer &p: shaderPointers_) {
        std::shared_ptr <DataPointer> const &keyPointer = p.GetKeyPointer();
        std::shared_ptr <DataPointer> const &valuePointer = p.GetValuePointer();
        size += Align4(sizeof(ShaderData) + keyPointer->GetSize() + valuePointer->GetSize());
    }
    return size;
}

int CacheData::Serialize(uint8_t *buffer, const size_t size)
{
    if (size < sizeof(Header)) {
        LOGE("abandon because of insufficient buffer space.");
        return -EINVAL;
    }
    Header *header = reinterpret_cast<Header *>(buffer);
    header->numShaders_ = shaderPointers_.size();
    off_t byteOffset = Align4(sizeof(Header));

    uint8_t *byteBuffer = reinterpret_cast<uint8_t *>(buffer);
    for (const ShaderPointer &p: shaderPointers_) {
        std::shared_ptr<DataPointer> const &keyPointer = p.GetKeyPointer();
        std::shared_ptr<DataPointer> const &valuePointer = p.GetValuePointer();
        size_t keySize = keyPointer->GetSize();
        size_t valueSize = valuePointer->GetSize();
        size_t pairSize = sizeof(ShaderData) + keySize + valueSize;
        size_t alignedSize = Align4(pairSize);
        if (byteOffset + alignedSize > size) {
            LOGE("abandon because of insufficient buffer space.");
            return -EINVAL;
        }
        
        ShaderData *shaderBuffer = reinterpret_cast<ShaderData *>(&byteBuffer[byteOffset]);
        shaderBuffer->keySize_ = keySize;
        shaderBuffer->valueSize_ = valueSize;
        if (!memcpy_s(shaderBuffer->data_, keySize, keyPointer->GetData(), keySize)) {
            LOGE("abandon, failed to copy key");
        }
        if (!memcpy_s(shaderBuffer->data_ + keySize, valueSize, valuePointer->GetData(), valueSize)) {
            LOGE("abandon, failed to copy value");
        }
        if (alignedSize > pairSize) {
            memset_s(shaderBuffer->data_ + keySize + valueSize, alignedSize - pairSize, 0, alignedSize - pairSize);
        }
        byteOffset += alignedSize;
    }
    return 0;
}

int CacheData::DeSerialize(uint8_t const *buffer, const size_t size)
{
    shaderPointers_.clear();
    if (size < sizeof(Header)) {
        LOGE("abandon, not enough room for cache header");
    }
    
    const Header *header = reinterpret_cast<const Header *>(buffer);
    size_t numShaders = header->numShaders_;
    off_t byteOffset = Align4(sizeof(Header));
    
    const uint8_t *byteBuffer = reinterpret_cast<const uint8_t *>(buffer);
    for (size_t i = 0; i < numShaders; i++) {
        if (byteOffset + sizeof(ShaderData) > size) {
            shaderPointers_.clear();
            LOGE("abandon because of insufficient buffer space");
            return -EINVAL;
        }
        const ShaderData *shaderBuffer = reinterpret_cast<const ShaderData *>(&byteBuffer[byteOffset]);
        size_t keySize = shaderBuffer->keySize_;
        size_t valueSize = shaderBuffer->valueSize_;
        size_t pairSize = sizeof(ShaderData) + keySize + valueSize;
        size_t alignedSize = Align4(pairSize);
        if (byteOffset + alignedSize > size) {
            shaderPointers_.clear();
            LOGE("not enough room for cache headers");
            return -EINVAL;
        }

        const uint8_t *data = shaderBuffer->data_;
        Rewrite(data, keySize, data + keySize, valueSize);
        byteOffset += alignedSize;
    }
    return 0;
}

void CacheData::CheckClean(const size_t newSize)
{
    if (maxTotalSize_ < newSize) {
        if (!cleanThreshold_) {
            RandClean(maxTotalSize_ / CLEAN_TO);
        } else {
            LOGE("abandon, totalSize too large");
            return;
        }
    }
}

void CacheData::RandClean(const size_t cleanThreshold)
{
    if (cleanThreshold == 0) {
        LOGE("CleanThreshold must be > 0");
        return;
    }
    if (cleanThreshold_ == 0) {
        long int now = std::chrono::steady_clock::now().time_since_epoch().count();
        for (int indexRand = 0; indexRand < RAND_LENGTH; ++indexRand) {
            cleanInit_[indexRand] = (now >> (indexRand * RAND_SHIFT)) & 0xFFFF;
            cleanInit_[indexRand] = (now >> (indexRand * RAND_SHIFT)) & 0xFFFF;
            cleanInit_[indexRand] = (now >> (indexRand * RAND_SHIFT)) & 0xFFFF;
        }
    }
    cleanThreshold_ = cleanThreshold;

    while (totalSize_ > cleanThreshold_) {
        long int randIndex = nrand48(cleanInit_);
        int removeIndex = randIndex % (shaderPointers_.size());
        if (!Clean(removeIndex)) {
            break;
        }
    }
}

size_t CacheData::Clean(const int removeIndex)
{
    const ShaderPointer &shader(shaderPointers_[removeIndex]);
    size_t reducedSize = shader.GetKeyPointer()->GetSize() + shader.GetValuePointer()->GetSize();
    totalSize_ -= reducedSize;
    shaderPointers_.erase(shaderPointers_.begin() + removeIndex);
    return reducedSize;
}

CacheData::DataPointer::DataPointer(const void *data, size_t size, bool ifOccupy)
    : pointer_(nullptr),
    size_(size),
    toFree_(ifOccupy)
{
    if (ifOccupy) {
        pointer_ = malloc(size);
    } else {
        pointer_ = data;
    }

    if (data != nullptr && ifOccupy) {
        if (!memcpy_s(const_cast<void *>(pointer_), size, data, size)) {
            LOGE("abandon: failed to copy data");
        }
    }
}

CacheData::DataPointer::~DataPointer()
{
    if (toFree_) {
        free(const_cast<void *>(pointer_));
        pointer_ = nullptr;
    }
}

CacheData::ShaderPointer::ShaderPointer() {}

CacheData::ShaderPointer::ShaderPointer(const std::shared_ptr <DataPointer> &key,
                                        const std::shared_ptr <DataPointer> &value)
    : keyPointer_(key),
    valuePointer_(value) {}

CacheData::ShaderPointer::ShaderPointer(const ShaderPointer &sp)
    : keyPointer_(sp.keyPointer_),
    valuePointer_(sp.valuePointer_) {}
}   // namespace Rosen
}   // namespace OHOS