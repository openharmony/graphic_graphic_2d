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

#include <include/gpu/GrContext.h>
#include <include/gpu/GrContextOptions.h>
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include "cache_data.h"

namespace OHOS {
namespace Rosen {
class ShaderCache : public GrContextOptions::PersistentCache {
public:
    static ShaderCache& Instance();

    virtual void InitShaderCache(const char *identity, const size_t size, bool isUni);
    virtual void InitShaderCache()
    {
        InitShaderCache(nullptr, 0, false);
    }

    virtual void SetFilePath(const std::string& filename);

    sk_sp<SkData> load(const SkData &key) override;
    void store(const SkData &key, const SkData &data) override;

private:
    ShaderCache() = default;
    ShaderCache(const ShaderCache &) = delete;
    void operator=(const ShaderCache &) = delete;

    CacheData *GetCacheData()
    {
        return cacheData_.get();
    }

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
    static constexpr size_t MAX_VALUE_SIZE = MAX_KEY_SIZE * 512;
    static constexpr size_t MAX_TOTAL_SIZE = MAX_VALUE_SIZE * 4;
    static constexpr size_t MAX_UNIRENDER_SIZE = MAX_VALUE_SIZE * 10;
};
}   // namespace Rosen
}   // namespace OHOS
#endif // OHOS_SHADER_CACHE_H
