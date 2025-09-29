/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "typeface_map.h"

#include <mutex>
#include <unistd.h>

namespace OHOS::Rosen {
TypefaceMap& TypefaceMap::GetInstance()
{
    static TypefaceMap instance;
    return instance;
}

std::shared_ptr<Drawing::Typeface> TypefaceMap::GetTypeface(uint32_t id)
{
    std::unique_lock guard(GetInstance().mutex_);
    auto iter = GetInstance().typefaceMap_.find(id);
    if (iter != GetInstance().typefaceMap_.end()) {
        if (auto tf = iter->second.lock()) {
            return tf;
        } else {
            GetInstance().typefaceMap_.erase(iter);
        }
    }
    return nullptr;
}

void TypefaceMap::InsertTypeface(uint32_t hash, const std::shared_ptr<Drawing::Typeface>& tf)
{
    if (tf == nullptr) {
        return;
    }
    std::unique_lock guard(GetInstance().mutex_);
    GetInstance().typefaceMap_.insert_or_assign(hash, tf);
}
} // namespace OHOS::Rosen