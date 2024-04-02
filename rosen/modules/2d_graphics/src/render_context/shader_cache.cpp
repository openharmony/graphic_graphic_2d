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
#include <cstddef>
#include <openssl/sha.h>
#include <random>
#include <thread>
#include <tuple>
#include "rs_trace.h"
#include "render_context_log.h"

namespace OHOS {
namespace Rosen {
ShaderCache& ShaderCache::Instance()
{
    static ShaderCache cache_;
    return cache_;
}

ShaderCache::~ShaderCache()
{
    LOGD("ShaderCache: destroying Shadercache");
}

void ShaderCache::InitShaderCache(const char* identity, const size_t size, bool isUni)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (filePath_.length() <= 0) {
        LOGD("abandon, illegal cacheDir length");
        return;
    }
    cacheData_.reset();
    size_t totalSize = isUni ? MAX_UNIRENDER_SIZE : MAX_TOTAL_SIZE;
    cacheData_ = std::make_unique<CacheData>(MAX_KEY_SIZE, MAX_VALUE_SIZE, totalSize, filePath_);
    cacheData_->ReadFromFile();
    if (identity == nullptr || size == 0) {
        LOGD("abandon, illegal cacheDir length");
        cacheData_->Clear();
    }

    SHA256_CTX sha256Ctx;
    SHA256_Init(&sha256Ctx);
    SHA256_Update(&sha256Ctx, identity, size);
    idHash_.resize(SHA256_DIGEST_LENGTH);
    SHA256_Final(idHash_.data(), &sha256Ctx);
    std::array<uint8_t, SHA256_DIGEST_LENGTH> shaArray;
    auto key = ID_KEY;

    auto [errorCode, loaded] = cacheData_->Get(&key, sizeof(key), shaArray.data(), shaArray.size());
    if (!(loaded && std::equal(shaArray.begin(), shaArray.end(), idHash_.begin()))) {
        cacheData_->Clear();
        LOGD("abandon, bad hash value, cleared for future regeneration");
    }

    LOGD("shadercache initiation success");
    initialized_ = true;
}

void ShaderCache::SetFilePath(const std::string& filename)
{
    if (filename.size() == 0) {
        LOGD("abandon, empty filename");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    filePath_ = filename + "/shader_cache";
}

std::shared_ptr<Drawing::Data> ShaderCache::Load(const Drawing::Data& key)
{
    RS_TRACE_NAME("Load shader");
    size_t keySize = key.GetSize();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) {
        LOGD("load: failed because ShaderCache is not initialized");
        return nullptr;
    }

    void* valueBuffer = malloc(bufferSize_);
    if (!valueBuffer) {
        LOGD("load: failed because unable to map memory");
        return nullptr;
    }
    if (!cacheData_) {
        LOGD("load: cachedata has been destructed");
        free(valueBuffer);
        valueBuffer = nullptr;
        return nullptr;
    }
    CacheData::ErrorCode errorCode = CacheData::ErrorCode::NO_ERR;
    size_t valueSize = 0;
    std::tuple<CacheData::ErrorCode, size_t> res = {errorCode, valueSize};
    res = cacheData_->Get(key.GetData(), keySize, valueBuffer, bufferSize_);
    errorCode = std::get<0>(res);
    valueSize = std::get<1>(res);
    if (errorCode == CacheData::ErrorCode::VALUE_SIZE_TOO_SAMLL) {
        free(valueBuffer);
        valueBuffer = nullptr;
        if (valueSize <= 0) {
            LOGD("valueSize size error");
            return nullptr;
        }
        void* newValueBuffer = malloc(valueSize);
        if (!newValueBuffer) {
            LOGD("load: failed to reallocate valueSize:%zu", valueSize);
            return nullptr;
        }
        valueBuffer = newValueBuffer;
        // Get key data with updated valueSize
        res = cacheData_->Get(key.GetData(), keySize, valueBuffer, valueSize);
        // update res after realloc and Get key
        errorCode = std::get<0>(res);
    }

    if (errorCode != CacheData::ErrorCode::NO_ERR) {
        LOGD("load: failed to get the cache value with the given key");
        free(valueBuffer);
        valueBuffer = nullptr;
        return nullptr;
    }
    auto data = std::make_shared<Drawing::Data>();
    if (!data->BuildFromMalloc(valueBuffer, valueSize)) {
        LOGD("load: failed to build drawing data");
        free(valueBuffer);
        valueBuffer = nullptr;
        return nullptr;
    }
    return data;
}

void ShaderCache::WriteToDisk()
{
    if (!(initialized_ && cacheData_ && savePending_)) {
        LOGD("abandon: failed to check prerequisites");
        return;
    }
    if (!idHash_.size()) {
        LOGD("abandon: illegal hash size");
        return;
    }
    auto key = ID_KEY;
    cacheData_->Rewrite(&key, sizeof(key), idHash_.data(), idHash_.size());
    cacheData_->WriteToFile();
    savePending_ = false;
}

void ShaderCache::Store(const Drawing::Data& key, const Drawing::Data& data)
{
    RS_TRACE_NAME("Store shader");
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        LOGD("stored: failed because ShaderCache is not initialized");
        return;
    }

    size_t valueSize = data.GetSize();
    size_t keySize = key.GetSize();
    if (keySize == 0 || valueSize == 0 || valueSize >= MAX_VALUE_SIZE) {
        LOGD("store: failed because of illegal cache sizes");
        return;
    }

    const void* value = data.GetData();
    cacheDirty_ = true;
    if (!cacheData_) {
        LOGD("store: cachedata has been destructed");
        return;
    }
    cacheData_->Rewrite(key.GetData(), keySize, value, valueSize);

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
size_t ShaderCache::QuerryShaderSize() const
{
    if (!cacheData_) {
        LOGD("QuerryShaderSize: cachedata has been destructed");
        return 0;
    }
    return cacheData_->GetTotalSize();
}

size_t ShaderCache::QuerryShaderNum() const
{
    if (!cacheData_) {
        LOGD("QuerryShaderNum: cachedata has been destructed");
        return 0;
    }
    return cacheData_->GetShaderNum();
}

void ShaderCache::CleanAllShaders() const
{
    if (cacheData_) {
        cacheData_->Clear();
    }
}
}   // namespace Rosen
}   // namespace OHOS