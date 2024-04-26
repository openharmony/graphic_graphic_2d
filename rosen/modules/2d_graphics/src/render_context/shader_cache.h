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

#ifndef OHOS_SHADER_CACHE_H
#define OHOS_SHADER_CACHE_H
#include "image/gpu_context.h"
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include "cache_data.h"

namespace OHOS {
namespace Rosen {
class ShaderCache : public Drawing::GPUContextOptions::PersistentCache {
public:
    static ShaderCache& Instance();

    struct OptionalLockGuard {
        explicit OptionalLockGuard(std::mutex& m): mtx(m), status(m.try_lock()) {}

        ~OptionalLockGuard()
        {
            if (status) {
                mtx.unlock();
            }
        }

        OptionalLockGuard(const OptionalLockGuard&) = delete;
        OptionalLockGuard& operator=(const OptionalLockGuard&) = delete;

        std::mutex& mtx;
        bool status = false;
    };

    virtual void InitShaderCache(const char *identity, const size_t size, bool isUni);
    virtual void InitShaderCache()
    {
        InitShaderCache(nullptr, 0, false);
    }

    virtual void SetFilePath(const std::string& filename);

    std::shared_ptr<Drawing::Data> Load(const Drawing::Data &key) override;
    void Store(const Drawing::Data &key, const Drawing::Data &data) override;

    bool IfInitialized() const
    {
        return initialized_;
    }

    size_t QuerryShaderSize() const;

    size_t QuerryShaderNum() const;

    void CleanAllShaders() const;

private:
    ShaderCache() = default;
    ~ShaderCache();
    ShaderCache(const ShaderCache &) = delete;
    void operator=(const ShaderCache &) = delete;

    void WriteToDisk();

    bool initialized_ = false;
    std::unique_ptr<CacheData> cacheData_;
    std::string filePath_;
    std::vector<uint8_t> idHash_;
    mutable std::mutex mutex_;

    bool savePending_ = false;
    unsigned int saveDelaySeconds_ = 3;

    size_t bufferSize_ = 16 * 1024;
    bool cacheDirty_ = false;

    static constexpr uint8_t ID_KEY = 0;

    static constexpr size_t MAX_KEY_SIZE = 1024;
    static constexpr size_t MAX_VALUE_SIZE = MAX_KEY_SIZE * 1024;
    static constexpr size_t MAX_TOTAL_SIZE = MAX_VALUE_SIZE * 4;
    static constexpr size_t MAX_UNIRENDER_SIZE = MAX_VALUE_SIZE * 10;
};
}   // namespace Rosen
}   // namespace OHOS
#endif // OHOS_SHADER_CACHE_H
