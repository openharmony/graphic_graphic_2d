/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "utils/typeface_hash_map.h"
#include <mutex>

namespace OHOS {
namespace Rosen {
namespace Drawing {
TypefaceHashMap& TypefaceHashMap::GetInstance()
{
    static TypefaceHashMap instance;
    return instance;
}

std::shared_ptr<Drawing::Typeface> TypefaceHashMap::GetTypefaceByFullHash(uint64_t fullHash)
{
    std::unique_lock guard(GetInstance().mutex_);
    return GetTypefaceByFullHashInner(fullHash);
}

std::shared_ptr<Drawing::Typeface> TypefaceHashMap::GetTypefaceByFullHashInner(uint64_t fullHash)
{
    auto iter = GetInstance().typefaceMap_.find(fullHash);
    if (iter != GetInstance().typefaceMap_.end()) {
        auto& [weakPtr, refCount] = iter->second;
        if (auto tf = weakPtr.lock()) {
            refCount++;
            return tf;
        } else {
            GetInstance().typefaceMap_.erase(iter);
        }
    }
    return nullptr;
}

void TypefaceHashMap::InsertTypefaceByFullHash(uint64_t fullHash, const std::shared_ptr<Drawing::Typeface>& tf)
{
    if (tf == nullptr) {
        return;
    }
    std::unique_lock guard(GetInstance().mutex_);
    GetInstance().typefaceMap_[fullHash] = std::make_tuple(tf, 1);
}

void TypefaceHashMap::Release(uint64_t fullHash)
{
    std::unique_lock guard(GetInstance().mutex_);
    auto iter = GetInstance().typefaceMap_.find(fullHash);
    if (iter != GetInstance().typefaceMap_.end()) {
        auto& [weakPtr, refCount] = iter->second;
        if (weakPtr.lock() == nullptr || --refCount == 0) {
            GetInstance().typefaceMap_.erase(iter);
        }
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS