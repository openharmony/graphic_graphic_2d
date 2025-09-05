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

namespace {
#ifdef _WIN32
#include <windows.h>
#endif
pid_t GetRealPid()
{
#ifdef _WIN32
    return GetCurrentProcessId();
#elif defined(OHOS_LITE) || defined(__APPLE__) || defined(__gnu_linux__)
    return getpid();
#else
    return getprocpid();
#endif
}

uint64_t GenGlobalUniqueId(uint32_t id)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsigned number shift
    return (shiftedPid | id);
}
}

namespace OHOS::Rosen {
TypefaceMap& TypefaceMap::GetInstance()
{
    static TypefaceMap instance;
    return instance;
}

std::shared_ptr<Drawing::Typeface> TypefaceMap::GetTypefaceByUniqueId(uint64_t uniqueId)
{
    std::unique_lock guard(GetInstance().mutex_);
    auto iter = GetInstance().typefaceMap_.find(uniqueId);
    if (iter != GetInstance().typefaceMap_.end()) {
        if (auto tf = iter->second.lock()) {
            return tf;
        } else {
            GetInstance().typefaceMap_.erase(iter);
        }
    }
    return nullptr;
}

void TypefaceMap::InsertTypeface(uint32_t uniqueId, const std::shared_ptr<Drawing::Typeface>& tf)
{
    if (tf == nullptr) {
        return;
    }
    std::unique_lock guard(GetInstance().mutex_);
    GetInstance().typefaceMap_.emplace(GenGlobalUniqueId(uniqueId), tf);
}
} // namespace OHOS::Rosen