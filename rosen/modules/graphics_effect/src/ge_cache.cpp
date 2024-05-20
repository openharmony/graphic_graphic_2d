/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "ge_cache.h"

namespace OHOS {
namespace GraphicsEffectEngine {

GECache& GECache::GetInstance()
{
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    static GECache instance;
    return instance;
}

void GECache::Set(size_t key, const std::shared_ptr<Drawing::Image>& value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = value;
}

const std::shared_ptr<Drawing::Image>& GECache::Get(size_t key) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
