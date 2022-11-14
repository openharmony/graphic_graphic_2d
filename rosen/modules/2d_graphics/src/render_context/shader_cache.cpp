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

#include "shader_cache.h"

#include <algorithm>
#include <array>
#include <openssl/sha.h>
#include <random>
#include <thread>
#include "rs_trace.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
ShaderCache& ShaderCache::Instance()
{
    static ShaderCache cache_;
    return cache_;
}

void ShaderCache::InitShaderCache(const char* identity, const size_t size, bool isUni)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (filePath_.length() <= 0) {
        LOGE("abandon, illegal cacheDir length");
        return;
    }
    cacheData_.reset();
    size_t totalSize = isUni ? MAX_UNIRENDER_SIZE : MAX_TOTAL_SIZE;
    cacheData_ = std::make_unique<CacheData>(MAX_KEY_SIZE, MAX_VALUE_SIZE, totalSize, filePath_);
    cacheData_->ReadFromFile();
    if (identity == nullptr || size == 0) {
        LOGE("abandon, illegal cacheDir length");
        cacheData_->Clear();
    }

    SHA256_CTX sha256Ctx;
    SHA256_Init(&sha256Ctx);
    SHA256_Update(&sha256Ctx, identity, size);
    idHash_.resize(SHA256_DIGEST_LENGTH);
    SHA256_Final(idHash_.data(), &sha256Ctx);
    std::array<uint8_t, SHA256_DIGEST_LENGTH> shaArray;
    auto key = ID_KEY;
    auto loaded = cacheData_->Get(&key, sizeof(key), shaArray.data(), shaArray.size());
    if (!(loaded && std::equal(shaArray.begin(), shaArray.end(), idHash_.begin()))) {
        cacheData_->Clear();
        LOGW("abandon, bad hash value, cleared for future regeneration");
    }
    initialized_ = true;
}

void ShaderCache::SetFilePath(const std::string& filename)
{
    if (filename.size() == 0) {
        LOGE("abandon, empty filename");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    filePath_ = filename + "/shader_cache";
}

sk_sp<SkData> ShaderCache::load(const SkData& key)
{
    RS_TRACE_NAME("load shader");
    size_t keySize = key.size();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) {
        LOGW("load: failed because ShaderCache is not initialized");
        return nullptr;
    }

    void* valueBuffer = malloc(bufferSize_);
    if (!valueBuffer) {
        LOGE("load: failed because unable to map memory");
        return nullptr;
    }
    CacheData* cacheData = GetCacheData();
    size_t valueSize = cacheData->Get(key.data(), keySize, valueBuffer, bufferSize_);
    if (!valueSize) {
        free(valueBuffer);
        valueBuffer = nullptr;
        void* newValueBuffer = realloc(valueBuffer, MAX_VALUE_SIZE);
        if (!newValueBuffer) {
            LOGE("load: failed to reallocate maxValueSize");
            return nullptr;
        }
        valueBuffer = newValueBuffer;
        valueSize = cacheData->Get(key.data(), keySize, valueBuffer, bufferSize_);
    }

    if (!valueSize || valueSize > bufferSize_) {
        LOGE("load: failed to get the cache value with the given key");
        free(valueBuffer);
        valueBuffer = nullptr;
        return nullptr;
    }
    return SkData::MakeFromMalloc(valueBuffer, valueSize);
}

void ShaderCache::WriteToDisk()
{
    if (!(initialized_ && cacheData_ && savePending_)) {
        LOGE("abandon: failed to check prerequisites");
        return;
    }
    if (!idHash_.size()) {
        LOGE("abandon: illegal hash size");
        return;
    }
    auto key = ID_KEY;
    cacheData_->Rewrite(&key, sizeof(key), idHash_.data(), idHash_.size());
    cacheData_->WriteToFile();
    savePending_ = false;
}

void ShaderCache::store(const SkData& key, const SkData& data)
{
    RS_TRACE_NAME("store shader");
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        LOGW("stored: failed because ShaderCache is not initialized");
        return;
    }

    size_t valueSize = data.size();
    size_t keySize = key.size();
    if (keySize == 0 || valueSize == 0 || valueSize >= MAX_VALUE_SIZE) {
        LOGE("store: failed because of illegal cache sizes");
        return;
    }

    const void* value = data.data();
    CacheData* cacheData = GetCacheData();
    cacheDirty_ = true;
    cacheData->Rewrite(key.data(), keySize, value, valueSize);

    if (!savePending_ && saveDelaySeconds_ > 0) {
        savePending_ = true;
        std::thread deferredSaveThread([this]() {
            sleep(saveDelaySeconds_);
            std::lock_guard<std::mutex> lock(mutex_);
            WriteToDisk();
            cacheDirty_ = false;
        });
        deferredSaveThread.detach();
    }
}
}   // namespace Rosen
}   // namespace OHOS